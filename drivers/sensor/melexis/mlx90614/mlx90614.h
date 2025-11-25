/*
 * Copyright (c) 2025 Christian Hirsch <christian@hirsch.zone>
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MLX90614_MLX90614_H_
#define ZEPHYR_DRIVERS_SENSOR_MLX90614_MLX90614_H_

#include <zephyr/device.h>

#define MLX90614_OPCODE_RAM_ACCESS           0x00
#define MLX90614_OPCODE_EEPROM_ACCESS        0x20
#define MLX90614_OPCODE_READ_FLAGS           0xf0
#define MLX90614_OPCODE_SLEEP_MODE           0xff

#define MLX90614_ADDR_RAM_RAW_DATA_IR_CHAN_1 0x04
#define MLX90614_ADDR_RAM_RAW_DATA_IR_CHAN_2 0x05
#define MLX90614_ADDR_RAM_TA                 0x06
#define MLX90614_ADDR_RAM_TOBJ1              0x07
#define MLX90614_ADDR_RAM_TOBJ2              0x08

#define MLX90614_ADDR_EEPROM_TO_MAX          0x00
#define MLX90614_ADDR_EEPROM_TO_MIN          0x01
#define MLX90614_ADDR_EEPROM_PWMCTRL         0x02
#define MLX90614_ADDR_EEPROM_TA_RANGE        0x03
#define MLX90614_ADDR_EEPROM_EMISSIVITY      0x04
#define MLX90614_ADDR_EEPROM_CONFIG_REG_1    0x05
#define MLX90614_ADDR_EEPROM_ADDR            0x0e
#define MLX90614_ADDR_EEPROM_ID_NUM_1        0x1c
#define MLX90614_ADDR_EEPROM_ID_NUM_2        0x1d
#define MLX90614_ADDR_EEPROM_ID_NUM_3        0x1e
#define MLX90614_ADDR_EEPROM_ID_NUM_4        0x1f

struct mlx90614_config {
	struct i2c_dt_spec i2c;
};

struct mlx90614_data {
	int16_t t_sample;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_MLX90614_MLX90614_H_ */
