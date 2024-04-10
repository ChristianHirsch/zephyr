/*
 * Copyright (c) 2024 Research Studios Austria Forschungsgesellschaft mbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Functions for setting and getting the state of the DALI lines.
 *
 * These need to be implemented by the user of this library.
 */
struct manchester_bitbang_io {
	/* Return the state of the RX line (zero/non-zero value) */
	int (*get_rx)(const void *io_context);
	/* Set the state of the TX line (zero/non-zero value) */
	void (*set_tx)(const void *io_context, int state);

	/* Enable the reception on the RX line (zero/non-zero value) */
	int (*enable_rx)(const void *io_context);
	/* Disable the reception on the RX line (zero/non-zero value) */
	int (*disable_rx)(const void *io_context);
};

/**
 * @brief Instance data for dali_bitbang
 *
 * A driver or other code wishing to use the dali_bitbang library should
 * create one of these structures then use it via the library APIs.
 * Structure members are private, and shouldn't be accessed directly.
 */
struct manchester_bitbang {
	const struct manchester_bitbang_io	*io;
	void				*io_context;
	uint32_t 		delays[5];
	uint32_t 		last;
	uint16_t 		rx_buf;
	uint8_t 		rx_buf_rdy;
	uint8_t 		received_half_bits;
	uint8_t 		last_bit;
};

/**
 * @brief Initialize an dali_bitbang instance
 *
 * @param bitbang	The instance to initialize
 * @param io		Functions to use for controlling DALI bus lines
 * @param io_context	Context pointer to pass to i/o functions when then are
 *			called.
 */
void manchester_bitbang_init(struct manchester_bitbang *bitbang,
			const struct manchester_bitbang_io *io, void *io_context);

/**
 * Implementation of the functionality required by the 'configure' function
 * in struct dali_driver_api.
 */
int manchester_bitbang_configure(struct manchester_bitbang *bitbang);

/**
 * Implementation of the functionality required by the 'transfer' function
 * in struct dali_driver_api.
 */
int manchester_bitbang_transfer(const struct manchester_bitbang *bitbang,
			   const struct manchester_msg *msgs, uint8_t num_msgs,
			   uint8_t device_address);

/**
 * Implementation of the callback function used for Manchester bit decoding.
 * This function is called when the edge on the RX line changes.
 */
void manchester_bitbang_toggle_rx(struct manchester_bitbang *context);
