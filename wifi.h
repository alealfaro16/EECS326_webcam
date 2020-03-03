/*
 * wifi.h
 *
 * Created: 2/29/2020 12:33:37 PM
 *  Author: ece-lab3
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#include "asf.h"

/** Size of the receive buffer used by the PDC, in bytes. */
#define UART_RX_BUFFER_SIZE 128 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff

/** Timer counter frequency in Hz. */
#define TC_FREQ             1


#define BOARD_ID_USART             ID_USART0   //Using USART 0 (RX - PA5, TX - PA6) could use USART1 too (RX - PA21, TX - PA22)

#define BOARD_USART                USART0

#define BOARD_USART_BAUDRATE       115200

#define wifi_usart_handler         USART0_Handler

#define USART_IRQn                 USART0_IRQn


/** USART0 pin RX */
#define PIN_USART0_RXD    {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RXD_IDX        (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/** USART0 pin TX */
#define PIN_USART0_TXD    {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_TXD_IDX        (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/** USART0 pin CTS */
#define PIN_USART0_CTS    {PIO_PA8A_CTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_CTS_IDX        (PIO_PA8_IDX)
#define PIN_USART0_CTS_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)


/** USART0 pin RTS (Configure as an output and drive to low) */
#define PIN_USART0_RTS			PIO_PA7_IDX



/* Web setup button board defines (using PA23). */
#define WEB_SETUP_BTN_ID                 ID_PIOA
#define WEB_SETUP_BTN_PIO                PIOA
#define WEB_SETUP_BTN_MSK				 PIO_PA23
#define WEB_SETUP_BTN_IDX                (PIO_PA23_IDX)
#define WEB_SETUP_BTN_ATTR               (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)


/* Using PB2 as a command pin  */
#define CMD_PIN_ID                 ID_PIOB
#define CMD_PIN_PIO                PIOB
#define CMD_PIN_MSK				   PIO_PB2
#define CMD_PIN_IDX				   (PIO_PB2_IDX)
#define CMD_PIN_ATTR               (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#define NET_STATUS_PIN				PIO_PB1_IDX
#define WIFI_RST_PIN				PIO_PB3_IDX

void wifi_usart_handler(void);
void process_incoming_byte_wifi(uint8_t in_byte);
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
void process_data_wifi(void);
void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask);
void configure_usart_wifi(void);
void configure_wifi_comm_pin(void);
void configure_wifi_web_setup_pin(void);
void write_wifi_command(const char* comm, uint8_t cnt);
//void write_image_to_file(void);
void wifi_chip_init(void);
void web_setup(void);


/* Web setup flag (true if it's triggered and false otherwise) */
volatile uint32_t web_setup_flag;

/* Command complete flag (true if it's triggered and false otherwise) */
volatile uint32_t command_complete;

/* Set to true when open stream is avalible for webcam (check wifi chip response for it) */
volatile uint32_t open_streams;

#endif /* WIFI_H_ */