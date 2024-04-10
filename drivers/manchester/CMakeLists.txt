# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_MANCHESTER_BITBANG		manchester_bitbang.c)
zephyr_library_sources_ifdef(CONFIG_MANCHESTER_GPIO		    manchester_gpio.c)
