/*
 * wifi.c
 *
 * Created: 2/6/2020 1:59:06 PM
 *  Author: ece-lab3
 */ 

#include <string.h>
#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"

/** Number of bytes received between two timer ticks. */
volatile uint32_t g_ul_bytes_received = 0;

/** Receive buffer. */
uint8_t g_uc_buffer;

/** PDC data packet. */
pdc_packet_t g_st_packet, g_st_nextpacket;

/** Pointer to PDC register base. */
Pdc *g_p_pdc;

/** String g_uc_buffer. */
uint8_t g_puc_string[BUFFER_SIZE];

/** Receive buffer. */
static uint8_t gs_puc_buffer[BUFFER_SIZE],gs_puc_nextbuffer[BUFFER_SIZE];

/** Dump buffer. */
static uint8_t gs_dump_buffer[BUFFER_SIZE];


/**
 *  \brief Interrupt handler for USART.
 *
 * Increment the number of bytes received in the current second and start
 * another transfer if the desired bps has not been met yet.
 *
 */
 void USART_Handler(void)
 {
	uint32_t ul_status;

	tc_stop(TC0, 0);

	/* Read USART status. */
	ul_status = usart_get_status(BOARD_USART);

	/* Receive buffer is full. */
	if (ul_status & US_CSR_RXBUFF) {
		g_ul_bytes_received += 2 * BUFFER_SIZE;
		if (g_ul_bytes_received < MAX_BPS) {
			/* Restart transfer if BPS is not high enough. */
			g_st_packet.ul_addr = (uint32_t)gs_puc_buffer;
			g_st_packet.ul_size = BUFFER_SIZE;
			g_st_nextpacket.ul_addr = (uint32_t)gs_puc_nextbuffer;
			g_st_nextpacket.ul_size = BUFFER_SIZE;
			pdc_rx_init(g_p_pdc, &g_st_packet, &g_st_nextpacket);
	} else {
			/* Otherwise disable interrupt. */
			usart_disable_interrupt(BOARD_USART, US_IDR_RXBUFF);
		}
		memcpy(gs_dump_buffer, gs_puc_buffer, BUFFER_SIZE);
	}
	tc_start(TC0, 0);
}

/**
 *  Configure board USART communication with PC or other terminal.
 */
static void configure_usart(void)
{
	static uint32_t ul_sysclk;
	const sam_usart_opt_t usart_console_settings = {
		BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Get peripheral clock. */
	ul_sysclk = sysclk_get_peripheral_hz();

	/* Enable peripheral clock. */
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

	/* Configure USART. */
	usart_init_hw_handshaking(BOARD_USART, &usart_console_settings, ul_sysclk);

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);
	
	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	 * emits one character at a time.
	 */
#endif

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
}
