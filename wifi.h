/*
 * wifi.h
 *
 * Created: 2/6/2020 1:59:28 PM
 *  Author: ece-lab3
 */ 


#ifndef WIFI_H_
#define WIFI_H_

/** Maximum Bytes Per Second (BPS) rate that will be forced using the CTS pin. */
#define MAX_BPS             500

/** Size of the receive buffer used by the PDC, in bytes. */
#define BUFFER_SIZE         125

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff

/** Timer counter frequency in Hz. */
#define TC_FREQ             1

#define STRING_EOL    "\r"
#define STRING_HEADER "--USART Hardware Handshaking Example --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define BOARD_ID_USART             ID_USART1

#define BOARD_USART                USART1

#define BOARD_USART_BAUDRATE       115200

#define USART_Handler              USART1_Handler

#define USART_IRQn                 USART1_IRQn

void wifi_usart_handler(void);
void process_incoming_byte_wifi(uint8_t in_byte);
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
void process_data_wifi(void);
void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask);
void configure_usart_wifi(void);
void configure_wifi_comm_pin(void);
void configure_wifi_web_setup_pin(void);
void write_wifi_command(char* comm, uint8_t cnt);
void write_immage_to_file(void);



#endif /* WIFI_H_ */
