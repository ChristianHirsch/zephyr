/*
 * Copyright (c) 2025 Christian Hirsch
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT melexis_mlx90614

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/sys/crc.h>

//#include <zephyr/drivers/sensor/sht4x.h>
#include "mlx90614.h"

#define CRC8_POLY   0x07

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MLX90614, CONFIG_SENSOR_LOG_LEVEL);

static uint8_t mlx90614_compute_pec(uint8_t *buf, size_t len)
{
	return crc8(buf, len, CRC8_POLY, 0x00, false);
}

/**
 * update a register on the device and @param old_value as well
 */
//static inline int mlx90394_update_register(const struct device *dev, const uint8_t reg_addr,
//					   const uint8_t new_val, uint8_t *old_value)
//{
//	const struct mlx90394_config *cfg = dev->config;
//
//	if (new_val != *old_value) {
//		*old_value = new_val;
//		return i2c_reg_write_byte_dt(&cfg->i2c, reg_addr, new_val);
//	}
//	return 0;
//}


static inline int mlx90614_check_who_am_i(const struct i2c_dt_spec *i2c)
{
	uint8_t buffer[2];
	int rc;

	//rc = i2c_burst_read_dt(i2c, MLX90394_REG_CID, buffer, ARRAY_SIZE(buffer));
	//if (rc != 0) {
	//	LOG_ERR("Failed to read who-am-i register (rc=%d)", rc);
	//	return -EIO;
	//}

	//if (buffer[0] != MLX90394_CID || buffer[1] != MLX90394_DID) {
	//	LOG_ERR("Wrong who-am-i value");
	//	return -EINVAL;
	//}

	return 0;
}

static int mlx90614_write_read_dt(const struct i2c_dt_spec *i2c, uint8_t start_addr,
				  uint8_t *buffer_write, uint8_t *buffer_read, size_t cnt)
{
	int rc;

	rc = i2c_burst_write_dt(i2c, start_addr, buffer_write, cnt);
	if (rc != 0) {
		LOG_ERR("Failed to write %d bytes to register %d (rc=%d)", cnt, start_addr, rc);
		return -EIO;
	}
	rc = i2c_burst_read_dt(i2c, start_addr, buffer_read, cnt);
	if (rc != 0) {
		LOG_ERR("Failed to read %d bytes from register %d (rc=%d)", cnt, start_addr, rc);
		return -EIO;
	}

	return 0;
}

static int mlx90614_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	int rc;
	struct mlx90614_data *data = dev->data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_AMBIENT_TEMP) {
		LOG_DBG("Invalid channel %d", chan);
		return -ENOTSUP;
	}

	//rc = mlx90614_trigger_measurement_internal(dev, chan);
	//if (rc != 0) {
	//	return rc;
	//}

	//k_usleep(data->measurement_time_us);

	return rc;
}

static int mlx90614_channel_get(const struct device *dev, enum sensor_channel chan,
				struct sensor_value *val)
{
	struct mlx90614_data *data = dev->data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_AMBIENT_TEMP) {
		LOG_DBG("Invalid channel %d", chan);
		return -ENOTSUP;
	}

	return 0;
}

static int mlx90614_init(const struct device *dev)
{
	const struct mlx90614_config *cfg = dev->config;
	struct mlx90614_data *data = dev->data;
	int rc;

	uint8_t buf[4] = {
		0x00,
		0xff,
		0x00,
		0x00
	};

	if (!i2c_is_ready_dt(&cfg->i2c)) {
		LOG_ERR("I2C bus device not ready");
		return -ENODEV;
	}

	LOG_INF("initializing MLX90617");

	LOG_INF("0x00, 0xff, PEC = %x", mlx90614_compute_pec(buf, 2));

	buf[1] = 0x2e;

	LOG_INF("0x00, 0x2e, 0x00, 0x00, PEC = %x", mlx90614_compute_pec(buf, 4));

	//rc = i2c_reg_write_byte(cfg->i2c, 0x00, 0x2e);
	//if (rc < 0) {
	//	LOG_ERR("failed to write i2c message");
	//}

	rc = i2c_burst_read(cfg->i2c.bus, 0x00, 0x2e, buf, 3);
	if (rc < 0) {
		LOG_ERR("failed to read i2c data");
	}
	LOG_INF("buf = %02x %02x %02x %02x", buf[0], buf[1], buf[2], buf[3]);
	LOG_INF("PEC = %02x", mlx90614_compute_pec(buf, 1));
	LOG_INF("PEC = %02x", mlx90614_compute_pec(buf, 2));

	LOG_INF("\nreading address 0x07");

	rc = i2c_burst_read(cfg->i2c.bus, 0x5a, 0x07, buf, 3);
	if (rc < 0) {
		LOG_ERR("failed to read i2c data");
	}
	LOG_INF("buf = %02x %02x %02x %02x", buf[0], buf[1], buf[2], buf[3]);
	LOG_INF("PEC = %02x", mlx90614_compute_pec(buf, 1));
	LOG_INF("PEC = %02x", mlx90614_compute_pec(buf, 2));

	int16_t value = *(int16_t *)buf;
	LOG_INF("value = %i", value);
	LOG_INF("temp  = %i", (value / 5) * 10 - 27315);

	///*
	// * Soft reset the chip
	// */
	//rc = i2c_reg_write_byte_dt(&cfg->i2c, MLX90614_REG_RST, MLX90614_RST);
	//if (rc != 0) {
	//	LOG_ERR("Failed to soft reset");
	//	return -EIO;
	//}
	//// k_usleep(MLX90394_STARTUP_TIME_US);

	///*
	// * check chip ID
	// */
	//rc = mlx90614_check_who_am_i(&cfg->i2c);
	//if (rc != 0) {
	//	return rc;
	//}

	return 0;
}

static DEVICE_API(sensor, mlx90614_driver_api) = {
	.sample_fetch = mlx90614_sample_fetch,
	.channel_get = mlx90614_channel_get,
};

#define MLX90614_DEFINE(inst)                                                  \
	static struct mlx90614_data mlx90614_data_##inst;                          \
	                                                                           \
	static const struct mlx90614_config mlx90614_config_##inst = {             \
		.i2c = I2C_DT_SPEC_INST_GET(inst)                                      \
	};                                                                         \
	                                                                           \
	SENSOR_DEVICE_DT_INST_DEFINE(inst,                                         \
			mlx90614_init,                                                     \
			NULL,                                                              \
			&mlx90614_data_##inst,                                             \
			&mlx90614_config_##inst,                                           \
			POST_KERNEL,                                                       \
			CONFIG_SENSOR_INIT_PRIORITY,                                       \
			&mlx90614_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MLX90614_DEFINE)
