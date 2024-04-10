/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Software driven 'bit-banging' library for serial communication
 * based on Manchester encoding
 *
 * Timings and protocol are based Microchip's DALI specification:
 * https://onlinedocs.microchip.com/pr/GUID-0CDBB4BA-5972-4F58-98B2-3F0408F3E10B-en-US-1/index.html
 */

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/manchester.h>
#include "manchester_bitbang.h"

#define LOG_LEVEL CONFIG_MANCHESTER_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(manchester_bitbang);

#define T_HALF_BIT 		0
#define T_FULL_BIT 		1
#define T_3TO4_BIT 		2
#define T_STOP_BIT 		3
#define T_BACKWARD_STOP_MIN 		4

#define NS_TO_SYS_CLOCK_HW_CYCLES(ns) \
	((uint64_t)sys_clock_hw_cycles_per_sec() * (ns) / NSEC_PER_SEC + 1)

#define US_TO_SYS_CLOCK_HW_CYCLES(us) \
	((uint64_t)sys_clock_hw_cycles_per_sec() * (us) / USEC_PER_SEC + 1)

int manchester_bitbang_configure(struct manchester_bitbang *context)
{
	context->delays[T_HALF_BIT]  = NS_TO_SYS_CLOCK_HW_CYCLES(416667);
	context->delays[T_3TO4_BIT]  = NS_TO_SYS_CLOCK_HW_CYCLES(625000);
	context->delays[T_FULL_BIT]  = NS_TO_SYS_CLOCK_HW_CYCLES(833333); /* 105000 */
	context->delays[T_STOP_BIT] = NS_TO_SYS_CLOCK_HW_CYCLES(5500000);
	context->delays[T_BACKWARD_STOP_MIN] = US_TO_SYS_CLOCK_HW_CYCLES(5500); /* 200000 */

	return 0;
}

static int manchester_get_rx(const struct manchester_bitbang *context)
{
  return context->io->get_rx(context->io_context);
}

void manchester_bitbang_toggle_rx(struct manchester_bitbang *context)
{
        uint32_t now = k_cycle_get_32();
        uint32_t diff = now - context->last;

        if (diff > context->delays[T_BACKWARD_STOP_MIN])
        {
                context->rx_buf = 0x00;
                context->rx_buf_rdy = 0;
                context->received_half_bits = 0;
                context->last_bit = 1;
        }
        else if (diff > context->delays[T_3TO4_BIT])
        {
                context->rx_buf <<= 1;
                // long: bit shift
                context->last_bit = !context->last_bit;
                context->rx_buf |= context->last_bit;

                context->received_half_bits+=2;
                LOG_DBG("long");
        }
        else
        {
                context->received_half_bits++;
                if (context->received_half_bits % 2 == 1)
                {
                        context->rx_buf <<= 1;
                        context->rx_buf |= context->last_bit;
                }
                // short: same bit
                LOG_DBG("short");
        }

        if (context->received_half_bits >= 17)
        {
                context->rx_buf_rdy = 1;
                LOG_DBG("received data: %hu", context->rx_buf & 0x00ff);
        }

        context->last = now;
}

static void manchester_set_tx(const struct manchester_bitbang *context, int state)
{
  context->io->set_tx(context->io_context, state);
}

static void manchester_delay(uint32_t cycles_to_wait)
{
	uint32_t start = k_cycle_get_32();

	/* Wait until the given number of cycles have passed */
	while (k_cycle_get_32() - start < cycles_to_wait) {
	}
}

static void write_bit(const struct manchester_bitbang *context, int bit)
{
	manchester_set_tx(context, !bit);
	manchester_delay(context->delays[T_HALF_BIT]);
	manchester_set_tx(context, bit);
	manchester_delay(context->delays[T_HALF_BIT]);
}

static void write_idle(const struct manchester_bitbang *context)
{
	manchester_set_tx(context, 1);
	manchester_delay(context->delays[T_STOP_BIT]);
}

static void write_byte(const struct manchester_bitbang *context, uint8_t data)
{
	uint8_t mask = 1 << 7;

	do {
			write_bit(context, data & mask);
	} while (mask >>= 1);
}

static void manchester_enable_rx(const struct manchester_bitbang *context)
{
  context->io->enable_rx(context->io_context);
}

static void manchester_disable_rx(const struct manchester_bitbang *context)
{
  context->io->disable_rx(context->io_context);
}

int manchester_bitbang_transfer(const struct manchester_bitbang *context,
                const struct manchester_msg *msg, uint8_t num_msgs,
                uint8_t device_address)
{
  LOG_INF("manchester_bitbang_transfer(%u, %u)", device_address, *msg->buf);

  manchester_disable_rx(context);

  write_bit(context, 1);
  write_byte(context, device_address);
  write_byte(context, *msg->buf);
  write_idle(context);

  manchester_enable_rx(context);

  return 0;
}

void manchester_bitbang_init(struct manchester_bitbang *context,
			const struct manchester_bitbang_io *io, void *io_context)
{
	context->io = io;
	context->io_context = io_context;
	manchester_bitbang_configure(context);
}
