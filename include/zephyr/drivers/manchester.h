/**
 * @file
 *
 * @brief Public APIs for the Manchester drivers.
 */

/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_MANCHESTER_H_
#define ZEPHYR_INCLUDE_DRIVERS_MANCHESTER_H_

/**
 * @brief DALI Interface
 * @defgroup dali_interface DALI Interface
 * @since 3.7
 * @version 1.0.0
 * @ingroup io_interfaces
 * @{
 */

#include <errno.h>

#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DALI_ADDR_BROADCAST   (0x3f << 1)
#define DALI_ADDR_GRP         BIT(7)
#define DALI_ADDR_IND         (0 << 7)
#define DALI_ADDR_CMD         BIT(0)
#define DALI_ADDR_PWR_LVL     (0 << 0)

/*
 * Output level instructions
 */
#define  DALI_OUT_OFF            0x00
#define  DALI_OUT_UP             0x01
#define  DALI_OUT_DOWN           0x02
#define  DALI_OUT_STEP_UP        0x03
#define  DALI_OUT_STEP_DOWN      0x04
#define  DALI_OUT_RECALL_MAX     0x05
#define  DALI_OUT_RECALL_MIN     0x06
#define  DALI_OUT_STEP_DOWN_OFF  0x07
#define  DALI_OUT_ON_STEP_UP     0x08
#define  DALI_OUT_EN_DAPC_SEQ    0x09
#define  DALI_OUT_LAST_ACTIVE    0x0a
#define  DALI_OUT_GO_TO_SCENE    0x10

/*
 * Configuration instructions
 */
#define  DALI_CMD_RESET                 0x20
#define  DALI_CMD_STORE_IN_DTR0         0x21
#define  DALI_SAVE_PERS_VAR             0x22
#define  DALI_SET_OP_MODE_DTR0          0x23
#define  DALI_RESET_MEM_BANK_DTR0       0x24
#define  DALI_IDENTIFY_DEV              0x25
#define  DALI_SET_MAX_LVL_DTR0          0x2a
#define  DALI_SET_MIN_LVL_DTR0          0x2b
#define  DALI_SET_SYS_FAILURE_LVL_DTR0  0x2c
#define  DALI_SET_POWER_ON_LVL_DTR0     0x2d
#define  DALI_SET_FADE_TIME_DTR0        0x2e
#define  DALI_SET_FADE_RATE_DTR0        0x2f
#define  DALI_SET_EXT_FADE_TIME_DTR0    0x30
#define  DALI_SET_SCENE                 0x40
#define  DALI_REMOVE_FROM_SCENE         0x50
#define  DALI_ADD_TO_GROUP              0x60
#define  DALI_REMOVE_FROM_GROUP         0x70
#define  DALI_SET_SHORT_ADDR_DTR0       0x80
#define  DALI_ENABLE_WRITE_MEMORY       0x81

/**
 * Query instructions
 */
#define  DALI_QUERY_STATUS                      0x90
#define  DALI_QUERY_CTRL_GEAR_PRESENT           0x91
#define  DALI_QUERY_LAMP_FAILURE                0x92
#define  DALI_QUERY_LAMP_ON                     0x93
#define  DALI_QUERY_LIMIT_ERROR                 0x94
#define  DALI_QUERY_RESET_STATE                 0x95
#define  DALI_QUERY_MISSING_SHORT_ADDR          0x96
#define  DALI_QUERY_VERSION_NUMBER              0x97
#define  DALI_QUERY_CONTENT_DTR0                0x98
#define  DALI_QUERY_DEVICE_TYPE                 0x99
#define  DALI_QUERY_PHYSICAL_MIN                0x9a
#define  DALI_QUERY_POWER_FAILURE               0x9b
#define  DALI_QUERY_CONTENT_DTR1                0x9c
#define  DALI_QUERY_CONTENT_DTR2                0x9d
#define  DALI_QUERY_OPERATING_MODE              0x9e
#define  DALI_QUERY_LIGHT_SOURCE_TYPE           0x9f
#define  DALI_QUERY_ACTUAL_LEVEL                0xa0
#define  DALI_QUERY_MAX_LEVEL                   0xa1
#define  DALI_QUERY_MIN_LEVEL                   0xa2
#define  DALI_QUERY_PWR_ON_LVL                  0xa3
#define  DALI_QUERY_SYS_FAILURE_LVL             0xa4
#define  DALI_QUERY_FADE_TIME_FADE_RATE         0xa5
#define  DALI_QUERY_MANUFACTURER_SPECIFIC_MODE  0xa6
#define  DALI_QUERY_NEXT_DEVICE_TYPE            0xa7
#define  DALI_QUERY_EXT_FADE_TIME               0xa8
#define  DALI_QUERY_CTRL_GEAR_FAILURE           0xaa
#define  DALI_QUERY_SCENE_LVL                   0xb0
#define  DALI_QUERY_GROUPS_0_7                  0xc0
#define  DALI_QUERY_GROUPS_8_15                 0xc1
#define  DALI_QUERY_RAND_ADDR_H                 0xc2
#define  DALI_QUERY_RAND_ADDR_M                 0xc3
#define  DALI_QUERY_RAND_ADDR_L                 0xc4
#define  DALI_READ_MEM_LOCATION                 0xc5
#define  DALI_QUERY_EXT_VERSION_NUMBER          0xff

/**
 * Special commands
 */
#define  DALI_TERMINATE                    0xa1
#define  DALI_DTR0_DATA                    0xa3
#define  DALI_CMD_INITIALIZE               0xa5
#define  DALI_CMD_RANDOMIZE                0xa7
#define  DALI_CMD_COMPARE                  0xa9
#define  DALI_CMD_WITHDRAW                 0xab
#define  DALI_CMD_PING                     0xad
#define  DALI_SEARCH_ADDRH                 0xb1
#define  DALI_SEARCH_ADDRM                 0xb3
#define  DALI_SEARCH_ADDRL                 0xb5
#define  DALI_PROGRAM_SHORT_ADDR           0xb7
#define  DALI_VERIFY_SHORT_ADDR            0xb9
#define  DALI_QUERY_SHORT_ADDR             0xbb
#define  DALI_ENABLE_DEV_TYPE              0xc1
#define  DALI_DTR1_DATA                    0xc3
#define  DALI_DTR2_DATA                    0xc5
#define  DALI_WRITE_MEM_LOCATION           0xc7
#define  DALI_WRITE_MEM_LOCATION_NO_REPLY  0xc9

/**
 * @brief Complete DALI DT information
 *
 * @param bus Pointer to the DALI bus device.
 */
struct manchester_dt_spec {
	const struct device *bus;
};

#define MANCHESTER_DT_SPEC_GET(node_id)         \
  .bus = DEVICE_DT_GET(DT_BUS(node_id))

/**
 * @brief One DALI message.
 *
 * This defines a DALI message to transmit on the DALI bus.
 */
struct manchester_msg {
  /** Data buffer in bytes */
  uint8_t   *buf;

  /** Length of buffer in bytes */
  uint32_t  len;

  /** Flags for this message */
  uint8_t   flags;
};

typedef int (*manchester_api_configure_t)(const struct device *dev);
typedef int (*manchester_api_get_config_t)(const struct device *dev);
typedef int (*manchester_api_full_io_t)(const struct device *dev,
         struct manchester_msg *msgs,
         uint8_t num_msgs,
         uint8_t addr);

__subsystem struct manchester_driver_api {
  manchester_api_configure_t configure;
  manchester_api_full_io_t transfer;
};

#define MANCHESTER_DEVICE_DT_DEFINE(node_id, init_fn, pm, data, config, level, \
           prio, api, ...)        \
  DEVICE_DT_DEFINE(node_id, init_fn, pm, data, config, level, \
       prio, api, __VA_ARGS__)

#define MANCHESTER_DEVICE_DT_INST_DEFINE(inst, ...)    \
  MANCHESTER_DEVICE_DT_DEFINE(DT_DRV_INST(inst), __VA_ARGS__)

/**
 * @brief Perform data transfer to a DALI device.
 *
 * This routine transmits data to a DALI (control) device.
 *
 * @param dev Pointer to the device structure for an DALI controller
 *            driver.
 * @param addr Address of the DALI target device.
 * @param buf Array of bytes to transfer.
 * @param num_bytes Number of bytes to transfer.
 *
 * @retval 0 If successful.
 * @retval negative On error.
 */
static inline int manchester_write(const struct device *dev,
          uint8_t addr,
          const uint8_t *buf,
          uint8_t num_bytes)
{
  struct manchester_msg msg;

  const struct manchester_driver_api *api =
    (const struct manchester_driver_api *)dev->api;

  msg.buf = (uint8_t *)buf;
  msg.len = num_bytes;

  int res =  api->transfer(dev, &msg, num_bytes, addr);

  return res;
}

/**
 * @brief Perform data transfer to a DALI device.
 *
 * This is equivalent to:
 *
 *     dali_write(spec->bus, addr, buf, num_bytes);
 *
 * @param spec DALI specification from devicetree.
 * @param addr Address of the DALI target device.
 * @param buf Array of bytes to transfer.
 * @param num_bytes Number of bytes to transfer.
 *
 * @return a value from dali_write()
 */
static inline int manchester_write_dt(const struct manchester_dt_spec *spec,
          uint8_t addr,
          const uint8_t *buf,
          uint8_t num_bytes)
{
  return manchester_write(spec->bus, addr, buf, num_bytes);
}

/**
 * @brief Perform a single byte data transfer to a DALI device.
 *
 * This routine transmits a single byte to a DALI (control) device.
 *
 * @param dev Pointer to the device structure for an DALI controller
 *            driver.
 * @param addr Address of the DALI target device.
 * @param value Array of bytes to transfer.
 *
 * @return a value from dali_write()
 */
static inline int manchester_write_byte(const struct device *dev,
          uint8_t addr,
          uint8_t value)
{
  return manchester_write(dev, addr, &value, 1);
}

/**
 * @brief Perform a single byte data transfer to a DALI device.
 *
 * This is equivalent to:
 *
 *     dali_write_byte(spec->bus, addr, value);
 *
 * @param spec DALI specification from devicetree.
 * @param addr TBA.
 * @param value TBA.
 *
 * @return A value from dali_write_byte()
 */
static inline int manchester_write_byte_dt(const struct manchester_dt_spec *spec,
          uint8_t addr,
          uint8_t value)
{
  return manchester_write_byte(spec->bus, addr, value);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_DRIVERS_DALI_H_ */
