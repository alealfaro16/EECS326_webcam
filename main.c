/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "timer_interface.h"
#include "wifi.h"
#include "camera.h"
#include "conf_board.h"
#include "conf_clock.h"




int main (void)
{

	
	/* Insert system clock initialization code here (sysclk_init()). */
	//osc_enable(OSC_MAINCK_XTAL);
	sysclk_init();  //Enable System Clock Service in ASF Wizard and choose clock in config/conf_clock.h (Use PLLA for SYSCLK and 120MHz)
	board_init();
	ioport_init();  //Enable IOPORT and GPIO service in the ASFW
	
	//Configure Timer and start it. Configure WiFi, USART, Command pin and Web Setup pin
	configure_tc();  //Use timer_interface.c function and enable Timer Clock drivers in the ASFW
	
	configure_usart_wifi(); //Enable USART drivers and services 
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();
	init_camera(); //Enable PIO 
	configure_camera();
	
	wifi_chip_init(); // Reset the WiFi and wait for it to connect, disable command prompt (>) and echo 
	
	//Set all flags to false:
	web_setup_flag = false;
	command_complete = false;
	open_streams = false;
	
	//Wait for connection while listening to wifi web set up flag
	while(!ioport_get_pin_level(NET_STATUS_PIN)){
		
		//check for web setup flag 
	if(web_setup_flag){
		
			//web setup
			web_setup();
			//clear the flag
			web_setup_flag = false;
		} 
	} 
	
	int reset_time = 5; //seconds to wait for reset 
	//command_complete =false;
	//usart_write_line(BOARD_USART, "Start of while loop \r\n");
	
	/* Infinite while loop */
	while(1){
		
		
		ioport_toggle_pin_level(MCU_STATUS_LED);
		
		delay_ms(300);
		//command_complete = false;
		//start_capture();
		
		//process_data_wifi();
		
		//uint8_t capture = start_capture();
		//usart_putchar(BOARD_USART,capture);
		//write_image_to_file();
		
		//check for web setup flag
		/*if(web_setup_flag){ //If 1
			//web setup
			web_setup();
			//clear the flag
			web_setup_flag = false;
		
		}
		else{ //If 1
			delay_ms(1000);
			//Check that the for connections, if no connections, reset and wait for connection (check for high signal on GPIO 14 of wifi chip)
			if(!ioport_get_pin_level(NET_STATUS_PIN)){ //if 2
				//Reset wifi chip
				ioport_set_pin_level(WIFI_RST_PIN,false);
				counts = 0;
				ioport_set_pin_level(WIFI_RST_PIN,true);
				while(counts>reset_time){
					
					//Wait for x amount of seconds and then break the loop
					}
					
				}
				else{ 
				//send poll all and check response
					write_wifi_command("poll all \r\n", 1);
					if(!open_streams){  //(send "poll all" to the wifi chip and check response (!none)) If 3
						delay_ms(1000); //delay 1s
						continue; //Reset the loop
					
						}
						//Succesful connection, take picture and send it over wifi
						else{ //If 3
							//Take picture, if succesful capture send over wifi
							start_capture();

							} //else 3
			
				} //else 2
			 
			} //else 1 */
		
		} //while loop
} //main