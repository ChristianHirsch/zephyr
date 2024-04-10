/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT gpio_manchester

/**
 * @file
 * @brief Driver for software driven MANCHESTER using GPIO lines
 *
 * This driver implements an MANCHESTER interface by driving two GPIO lines under
 * software control.
 *
 * The GPIO pins used must be configured (through devicetree and pinmux) with
 * suitable flags, i.e. the SDA pin as open-collector/open-drain with a pull-up
 * resistor (possibly as an external component attached to the pin).
 *
 * When the SDA pin is read it must return the state of the physical hardware
 * line, not just the last state written to it for output.
 *
 * The SCL pin should be configured in the same manner as SDA, or, if it is
 * known that the hardware attached to pin doesn't attempt clock stretching,
 * then the SCL pin may be a push/pull output.
 */

#include <zephyr/device.h>
#include <errno.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/manchester.h>

#define LOG_LEVEL CONFIG_MANCHESTER_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(manchester_gpio);

#include "manchester_bitbang.h"

/* Driver config */
struct manchester_gpio_config {
	struct gpio_dt_spec rx_gpio;
	struct gpio_dt_spec tx_gpio;
};

/* Driver instance data */
struct manchester_gpio_context {
	struct gpio_callback cb_data;
	struct manchester_bitbang bitbang;	/* Bit-bang library data */
	struct k_mutex mutex;
};

struct manchester_gpio_callback {
	struct manchester_gpio_context *context;
};

static void manchester_gpio_rx_toggle_cb(const struct device *dev, struct gpio_callback *cb,
        uint32_t pins)
{
	struct manchester_gpio_context *context = (struct manchester_gpio_context *)cb;
	manchester_bitbang_toggle_rx(&context->bitbang);
}

static int manchester_gpio_get_rx(const void *io_context)
{
	const struct manchester_gpio_config *config = io_context;
	int rc = gpio_pin_get_dt(&config->rx_gpio);

	/* Default high as that would be a NACK */
	return rc != 0;
}

static void manchester_gpio_set_tx(const void *io_context, int state)
{
	const struct manchester_gpio_config *config = io_context;

	gpio_pin_set_dt(&config->tx_gpio, state);
}

static int manchester_gpio_enable_rx(const void *io_context)
{
	const struct manchester_gpio_config *config = io_context;
	int rc = gpio_pin_interrupt_configure_dt(&config->rx_gpio,
                GPIO_INT_EDGE_BOTH);
	return rc;
}

static int manchester_gpio_disable_rx(const void *io_context)
{
	const struct manchester_gpio_config *config = io_context;
	int rc = gpio_pin_interrupt_configure_dt(&config->rx_gpio,
                GPIO_INT_DISABLE);
	return rc;
}

static const struct manchester_bitbang_io io_fns = {
	.get_rx = &manchester_gpio_get_rx,
	.set_tx = &manchester_gpio_set_tx,
	.enable_rx = manchester_gpio_enable_rx,
	.disable_rx = manchester_gpio_disable_rx,
};

static int manchester_gpio_configure(const struct device *dev)
{
	struct manchester_gpio_context *context = dev->data;
	int rc;

	k_mutex_lock(&context->mutex, K_FOREVER);

	rc = manchester_bitbang_configure(&context->bitbang);

	k_mutex_unlock(&context->mutex);

	return rc;
}

static int manchester_gpio_transfer(const struct device *dev, struct manchester_msg *msgs,
				uint8_t num_msgs, uint8_t device_address)
{
	struct manchester_gpio_context *context = dev->data;
	int rc;

	LOG_INF("manchester_gpio_transfer");

	k_mutex_lock(&context->mutex, K_FOREVER);

	rc = manchester_bitbang_transfer(&context->bitbang, msgs, num_msgs, device_address);

	k_mutex_unlock(&context->mutex);

	return rc;
}

static struct manchester_driver_api api = {
	.configure = manchester_gpio_configure,
	.transfer = manchester_gpio_transfer,
};

static int manchester_gpio_init(const struct device *dev)
{
	struct manchester_gpio_context *context = dev->data;
	const struct manchester_gpio_config *config = dev->config;
	int err;

	if (!gpio_is_ready_dt(&config->rx_gpio)) {
		LOG_ERR("RX GPIO device not ready");
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&config->rx_gpio,
				    GPIO_INPUT | GPIO_ACTIVE_LOW);
	if (err) {
		LOG_ERR("failed to configure RX GPIO pin (err %d)", err);
		return err;
	}

	err = gpio_pin_interrupt_configure_dt(&config->rx_gpio,
                GPIO_INT_EDGE_BOTH);
  if (err != 0) {
    LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n",
      err, config->rx_gpio.port->name, config->rx_gpio.pin);
    return err;
  }

  gpio_init_callback(&context->cb_data, manchester_gpio_rx_toggle_cb,
			BIT(config->rx_gpio.pin));
  gpio_add_callback(config->rx_gpio.port, &context->cb_data);

	if (!gpio_is_ready_dt(&config->tx_gpio)) {
		LOG_ERR("TX GPIO device not ready");
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&config->tx_gpio, GPIO_OUTPUT_HIGH);
	if (err) {
		LOG_ERR("failed to configure TX GPIO pin (err %d)", err);
		return err;
	}

	manchester_bitbang_init(&context->bitbang, &io_fns, (void *)config);

	err = k_mutex_init(&context->mutex);
	if (err) {
		LOG_ERR("Failed to create the manchester lock mutex : %d", err);
		return err;
	}

	return 0;
}

#define	DEFINE_MANCHESTER_GPIO(_num)						\
	\
	static struct manchester_gpio_context manchester_gpio_dev_data_##_num;		\
	\
	static const struct manchester_gpio_config manchester_gpio_dev_cfg_##_num = {		\
		.rx_gpio	= GPIO_DT_SPEC_INST_GET(_num, rx_gpios),	\
		.tx_gpio	= GPIO_DT_SPEC_INST_GET(_num, tx_gpios),	\
	};									\
	\
	MANCHESTER_DEVICE_DT_INST_DEFINE(_num,						\
			manchester_gpio_init,						\
			NULL,							\
			&manchester_gpio_dev_data_##_num,					\
			&manchester_gpio_dev_cfg_##_num,					\
			POST_KERNEL, CONFIG_MANCHESTER_INIT_PRIORITY, &api);

DT_INST_FOREACH_STATUS_OKAY(DEFINE_MANCHESTER_GPIO)
