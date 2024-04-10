/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/manchester.h>
#include <string.h>
#include <zephyr/internal/syscall_handler.h>

static inline int z_vrfy_manchester_transfer(const struct device *dev,
          struct manchester_msg *msgs, uint8_t num_msgs,
          uint8_t addr)
{
	return z_impl_manchester_transfer((const struct device *)dev, msgs, num_msgs, addr);
}
#include <syscalls/manchester_transfer_mrsh.c>
