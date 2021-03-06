/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "camera.h"
#include "user_board.h"
#include "wifi.h"

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	/* Disable the watchdog */
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* Configure TWI pins */

	gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);

	/* Configure PCK1 pins */
	gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS);
	
	/* Configure Image sensor pins */
	gpio_configure_pin(OV_RST_GPIO, OV_RST_FLAGS);
	gpio_configure_pin(OV_HSYNC_GPIO, OV_HSYNC_FLAGS);
	gpio_configure_pin(OV_VSYNC_GPIO, OV_VSYNC_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D0, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D1, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D2, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D3, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D4, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D5, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D6, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D7, OV_DATA_BUS_FLAGS);

	
	/* Configure USART RXD pin */
	gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	
	/* Configure USART TXD pin */
	gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);
	
	/* Configure USART CTS pin */
	gpio_configure_pin(PIN_USART0_CTS_IDX, PIN_USART0_CTS_FLAGS);
	
	/*Configure USART RTS pin as output and drive to low*/
	ioport_set_pin_dir(PIN_USART0_RTS,IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_USART0_RTS, false);
	
	/* Configure Command Complete pin */
	gpio_configure_pin(CMD_PIN_IDX, CMD_PIN_ATTR);
	
	/* Configure web setup pin */
	gpio_configure_pin(WEB_SETUP_BTN_IDX, WEB_SETUP_BTN_ATTR);
	
	/*Configure network status pin*/
	ioport_set_pin_dir(NET_STATUS_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(NET_STATUS_PIN,IOPORT_MODE_PULLDOWN);
	
	/*Configure wifi reset pin (drive LOW to reset wifi chip)*/
	ioport_set_pin_dir(WIFI_RST_PIN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(WIFI_RST_PIN, true);
	
	/*Configure LED MCU status pin*/
	ioport_set_pin_dir(MCU_STATUS_LED, IOPORT_DIR_OUTPUT);
	
	
	//Configure other pins:

}
