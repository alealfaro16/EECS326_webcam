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

	board_init();
	sysclk_init();  //Enable System Clock Service in ASF Wizard and choose clock in config/conf_clock.h
	ioport_init();  //Enable IOPORT and GPIO service in the ASFW
	
	//Configure Timer and start it. Configure WiFi, USART, Command pin and Web Setup pin
	configure_tc();  //Use timer_interface.c function and enable Timer Clock drivers in the ASFW
	
	configure_usart_wifi(); //Enable USART drivers and services + PDC for buffer (Unsure about it)
	//wifi_init(); Reset the WiFi and wait for it to connect, disable command prompt (>) and echo 
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();
	init_camera(); //Enable PIO and OV7740 drivers
	configure_camera();
	
	//Wait for connection while listening to wifi web set up flag
	while(!reset_wifi()){
		
		//check for web setup flag 
		if(web_setup_flag){
			//web setup
		}
	}
	
	
	/* Infinite while loop */
	while(1){
		
		/*
		//check for web setup flag
		if(web_setup_flag){
			//web setup
		
		}
		else{
			//Check that the for connections, if no connections, reset and wait for connection
			if(!wifi_connected){
				while(!reset_wifi()){
					//Wait for x amount of seconds and then reset the loop
					if(timeout){
						continue(); //Reset the loop
					}
					
				}
			else{
				if(!open_streams_available){
					delay(1); //delay 1s
					continue(); //Reset the loop
					
				}
				//Succesful connection, take picture and send it over wifi
				else{
					
					if(start_capture){
					//Succesful camera capture, send over wifi
						write_image_to_file();  //Writes an image from the SAM4S8B to the AMW136
					}
					
				
				}
					
					
			}
			
			}
		}
		
		
		*/
		
	}
}
