/*
 * wifi.h
 *
 * Created: 2/6/2020 1:59:28 PM
 *  Author: ece-lab3
 */ 


#ifndef WIFI_H_
#define WIFI_H_

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