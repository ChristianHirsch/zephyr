/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/dali.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

void dali_on(const struct device *const dev)
{
	uint8_t command = DALI_OUT_RECALL_MAX;

	dali_write(dev, &command, 1, DALI_ADDR_BROADCAST_CMD);
}

void dali_off(const struct device *const dev)
{
	uint8_t command = DALI_OUT_OFF;

	dali_write(dev, &command, 1, DALI_ADDR_BROADCAST_CMD);
}

int main(void)
{
	LOG_INF("starting application");

	const struct device *const dev = DEVICE_DT_GET(DT_PATH(dali));
	LOG_INF("Got DALI device %p", dev);

	if (dev == NULL) {
		LOG_ERR("Could not get DALI device");
		return -ENODEV;
	}

	while (1) {
		LOG_INF("on");
		dali_on(dev);
		k_sleep(K_MSEC(5000));

		LOG_INF("off");
		dali_off(dev);
		k_sleep(K_MSEC(5000));
	}

	return 0;
}
