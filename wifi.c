/*
 * wifi.c
 *
 * Created: 2/6/2020 1:59:06 PM
 *  Author: ece-lab3
 */ 

#include <string.h>
#include "asf.h"
//#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"
#include "camera.h"
#include "timer_interface.h"
#include <stdio.h>	/*printf*/
#include <stdlib.h>	/*malloc*/
#include <string.h> /*memset*/


/** USART FIFO transfer type definition. */

/** Byte mode read buffer. */
static uint8_t byte_buffer = 0;

static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static RxBuff_count = 0;

/* Web setup flag (true if it's triggered and false otherwise) */
volatile uint32_t web_setup_flag = true;

/* Command complete flag (true if it's triggered and false otherwise) */
volatile uint32_t command_complete = true;

/* Set to true when open stream is avalible for webcam (check wifi chip response for it) */
uint32_t open_streams = false;


/**
 *  \brief Interrupt handler for USART.
 *
 * Increment the number of bytes received in the current second and start
 * another transfer if the desired bps has not been met yet.
 *
 */
 void wifi_usart_handler(void)  //Handler for incoming data from the WiFi. Should call process incoming byte wifi when a new byte arrives.
 {
	 
	 uint32_t ul_status;

	 /* Read USART Status. */
	 ul_status = usart_get_status(BOARD_USART);
	 
	 /* Transfer without PDC, store byte in buffer */
	 if (ul_status & US_CSR_RXRDY) {
		 //Get byte from USART
		 usart_getchar(BOARD_USART, (uint32_t *)&byte_buffer);
		 process_incoming_byte_wifi(byte_buffer);
		 process_data_wifi();
	 }


}

/**
 *  Configure board USART communication with PC or other terminal.
 */
void configure_usart_wifi(void)
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

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
	

	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY); //Not sure which interrupt to use, currently interrupt happen when RX is ready
}


void configure_wifi_comm_pin(void){ //Configuration of “command complete” rising-edge interrupt used for serial communication with wifi chip (see Appendix B for more details)

	/* Configure PIO clock. */
	//pmc_enable_periph_clk(CMD_PIN_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	//pio_set_debounce_filter(WEB_SETUP_BTN_PIO, CMD_PIN_MSK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(CMD_PIN_PIO, CMD_PIN_ID, CMD_PIN_MSK,
			CMD_PIN_ATTR, wifi_command_response_handler);
			
   /* Enable PIO controller IRQs. */
   NVIC_EnableIRQ((IRQn_Type)CMD_PIN_ID);

   /* Enable PIO interrupt lines. */
   //pio_enable_interrupt(CMD_PIN_PIO, WEB_SETUP_BTN_MSK);
}


void configure_wifi_web_setup_pin(void){  //Configuration of button interrupt to initiate web setup.
	
	/* Configure PIO clock. */
	//pmc_enable_periph_clk(WEB_SETUP_BTN_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	//pio_set_debounce_filter(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_MSK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_ID, WEB_SETUP_BTN_MSK,
			WEB_SETUP_BTN_ATTR, wifi_web_setup_handler);
			
   /* Enable PIO controller IRQs. */
   NVIC_EnableIRQ((IRQn_Type)WEB_SETUP_BTN_ID);

   /* Enable PIO interrupt lines. */
   //pio_enable_interrupt(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_MSK);
	
}


void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask){ //Should set a flag indicating a request to initiate web setup.
	
	unused(ul_id);
	unused(ul_mask);

	web_setup_flag = true;
}

void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask){//Handler for “command complete” rising-edge interrupt from AMW136. When this is triggered, it is time to process the response of the AMW136.
	
	unused(ul_id);
	unused(ul_mask);

	command_complete = true;
	
}

void process_incoming_byte_wifi(uint8_t in_byte){  //Stores every incoming byte (in byte) from the AMW136 in a buffer.
	
	
	UART_RxBuf[RxBuff_count++] = in_byte; /* store received data in buffer */
	usart_putchar(BOARD_USART, in_byte); //echo 
	
	
}

void write_wifi_command(const char* comm, uint8_t cnt){
	
	/*Writes a command (comm) to the AMW136,
	and waits either for an acknowledgment or a timeout. The timeout can be created by setting the
	global variable counts to zero, which will automatically increment every second, and waiting
	while counts < cnt. */
	
	//Start timer counter 
	counts = 0;
	
	//Write command to AMW136
	usart_write_line(BOARD_USART, comm);
	
	//Wait until timeout or response of the AMW104
	while(counts<cnt){
		
		if(!UART_RxBuf[RxBuff_count+1] == 0) { //find better way to know when chip has sent command back
			//printf("AMW136 received the data");
			break;
		}
	}
	if(counts>cnt){
		//printf("timeout happened");
		usart_write_line(BOARD_USART, "Timeout!");
	}
	
}

void process_data_wifi(void){  
	
	/*Processes the response of the AMW136, which should be stored
	in the buffer filled by process incoming byte wifi. This processing should be looking for certain
	responses that the AMW136 should give, such as “start transfer” when it is ready to receive the
	image. */
	
	/*Commands to parse and check:
		-Chip has a connection
		-Web setup flag
		-... */
	
	if (!strstr(UART_RxBuf, "start transfer") == NULL){
		
		//Start transfer flag?
		//write_image_to_file();
		//usart_write_line(BOARD_USART,"Success!");
		memset(UART_RxBuf,"0",UART_RX_BUFFER_SIZE);
		
	}
	//Check for open streams
	else if(!strstr(UART_RxBuf, "[Opened: 0]") == 0){
		
		open_streams = true;
		memset(UART_RxBuf,"0",UART_RX_BUFFER_SIZE);
	}
	else if(!strstr(UART_RxBuf, "Complete") == 0){
		
		//Image transfer has been completed, delay and restart loop
		delay_ms(50);
	}
	
	
}

// void write_image_to_file(void)
	
	
void wifi_chip_init(void){
		
		//brief send commands to setup the wifi chip for webcam application (disable echo, command prompt ,etc) and reset it
		
		//Turn off UART0 flow control 
		write_wifi_command("set uart.flow 0 off \r\n",1);
		
		delay_ms(1000);
		
		//Set the status indicator LEDs
		write_wifi_command("set sy i g network 18 \r\n",1);
		write_wifi_command("set sy i g wlan 20 \r\n",1);
		write_wifi_command("set sy i g softap 21 \r\n",1);
		
		delay_ms(1000);
		
		//Disable echo and command prompt
		write_wifi_command("set sy c e off \r\n",1);
		write_wifi_command("set sy c p 0 \r\n",1);
		
		delay_ms(1000);
		
		//Set the rx/tx buffer size to 10000 bytes and the rx:tx ratio to 50%
		write_wifi_command("set ne b s 10000 \r\n",1);
		write_wifi_command("set ne b r 50 \r\n",1);
		
		delay_ms(1000);
		
		//Set the command complete and network gpios and configure them 
		//write_wifi_command("set gp a 13 cmdCmplt \r\n",1);
		//write_wifi_command("set gp a 14 netStatus \r\n",1);
	
		write_wifi_command("set sy c g 13 \r\n",1);
		write_wifi_command("set wl n o 14 \r\n",1);
		
		delay_ms(1000);
		
		//Necessary to save settings
		write_wifi_command("save \r\n",1);
		write_wifi_command("reboot \r\n",1);
		
		delay_ms(1000);
		
		//Reset
		ioport_set_pin_level(WIFI_RST_PIN,false);
		delay_ms(300);
		ioport_set_pin_level(WIFI_RST_PIN,true);
	
		
		
}

void web_setup(void){
	
	//Enter web setup mode
	write_wifi_command("setup web \r\n",1);
	
	//Do other stuff if needed
} 

