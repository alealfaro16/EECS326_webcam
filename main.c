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

	
	//Wait for connection while listening to wifi web set up flag
	while(!ioport_get_pin_level(NET_STATUS_PIN)){
		
		//check for web setup flag 
	if(get_flag(WEB_SETUP)){
		
			//web setup
			web_setup();
			//clear the flag
			set_flag(WEB_SETUP, false);
		} 
	} 
	
	int reset_time = 5; //seconds to wait for reset 
	
	/* Infinite while loop */
	while(1){
		
		
		ioport_toggle_pin_level(MCU_STATUS_LED);

		
		//check for web setup flag
		if(get_flag(WEB_SETUP)){ //If 1
			//web setup
			web_setup();
			//clear the flag
			set_flag(WEB_SETUP, false);
		
		}
		else{ //If 1

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
					counts = 0;
					if(!get_flag(OPEN_STREAMS)){
						while(!get_flag(OPEN_STREAMS)){  //(send "poll all" to the wifi chip and check response (!none)) If 3
						
							if(counts>reset_time){ 
								continue; //Reset the loop
							}
							write_wifi_command("poll all \r\n", 1);
							delay_ms(100);	
						}
					}
					//Succesful connection, take picture and send it over wifi
					//Take picture, if succesful capture send over wifi
					set_flag(CMD_CMPLT, false);
					start_capture();
			
				} //else 2
			 
			} //else 1 
		
		} //while loop
} //main