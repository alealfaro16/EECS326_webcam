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


//USART with PDC Enable definintions:

/** Number of bytes received between two timer ticks. */
//volatile uint32_t g_ul_bytes_received = 0;

/** Receive buffer. */
//uint8_t g_uc_buffer;

/** PDC data packet. */
//pdc_packet_t g_st_packet, g_st_nextpacket;

/** Pointer to PDC register base. */
//Pdc *g_p_pdc;

/** String g_uc_buffer (might not need this) */
//uint8_t g_puc_string[BUFFER_SIZE];

/** Receive buffer. */
//static uint8_t gs_puc_buffer[BUFFER_SIZE],gs_puc_nextbuffer[BUFFER_SIZE];

/** Dump buffer. */
//static uint8_t gs_dump_buffer[BUFFER_SIZE];

/** USART FIFO transfer type definition. */

/** Byte mode read buffer. */
static uint8_t byte_buffer = 0;

static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;

/* Push button information (true if it's triggered and false otherwise) */
static volatile uint32_t web_setup = false;




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
	 }
	

}

/**
 *  Configure board USART communication with PC or other terminal.
 */
static void configure_usart_wifi(void)
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
	
	
	/* Get board USART PDC base address and enable receiver. */
	//g_p_pdc = usart_get_pdc_base(BOARD_USART);
	//g_st_packet.ul_addr = (uint32_t)gs_puc_buffer;
	//g_st_packet.ul_size = BUFFER_SIZE;
	//g_st_nextpacket.ul_addr = (uint32_t)gs_puc_nextbuffer;
	//g_st_nextpacket.ul_size = BUFFER_SIZE;
	//pdc_rx_init(g_p_pdc, &g_st_packet, &g_st_nextpacket);
	//pdc_enable_transfer(g_p_pdc, PERIPH_PTCR_RXTEN);
	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY); //Not sure which interrupt to use, currently interrupt happen when RX is ready
}


void configure_wifi_comm_pin(void){ //Configuration of “command complete” rising-edge interrupt used for serial communication with wifi chip (see Appendix B for more details)

	/* Configure PIO clock. */
	pmc_enable_periph_clk(CMD_PIN_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	pio_set_debounce_filter(WEB_SETUP_BTN_PIO, CMD_PIN_MSK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(CMD_PIN_PIO, CMD_PIN_ID, CMD_PIN_MSK,
			CMD_PIN_ATTR, wifi_command_response_handler);
			
   /* Enable PIO controller IRQs. */
   NVIC_EnableIRQ((IRQn_Type)CMD_PIN_ID);

   /* Enable PIO interrupt lines. */
   pio_enable_interrupt(CMD_PIN_PIO, WEB_SETUP_BTN_MSK);
}


void configure_wifi_web_setup_pin(void){  //Configuration of button interrupt to initiate web setup.
	
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WEB_SETUP_BTN_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	pio_set_debounce_filter(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_MSK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_ID, WEB_SETUP_BTN_MSK,
			WEB_SETUP_BTN_ATTR, wifi_web_setup_handler);
			
   /* Enable PIO controller IRQs. */
   NVIC_EnableIRQ((IRQn_Type)WEB_SETUP_BTN_ID);

   /* Enable PIO interrupt lines. */
   pio_enable_interrupt(WEB_SETUP_BTN_PIO, WEB_SETUP_BTN_MSK);
	
}


void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask){ //Should set a flag indicating a request to initiate web setup.
	
	unused(ul_id);
	unused(ul_mask);

	web_setup = true;
}

void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask){//Handler for “command complete” rising-edge interrupt from AMW136. When this is triggered, it is time to process the response of the AMW136.
	
}

void process_incoming_byte_wifi(uint8_t in_byte){  //Stores every incoming byte (in byte) from the AMW136 in a buffer.
	
	unsigned char tmphead;
	/* calculate buffer index */
	tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK;
	UART_RxHead = tmphead; /* store new index */
	if ( tmphead == UART_RxTail )
	{
		/* ERROR! Receive buffer overflow */
	}
	
	UART_RxBuf[tmphead] = in_byte; /* store received data in buffer */
	
	
}

void write_wifi_command(char* comm, uint8_t cnt){
	
	/*Writes a command (comm) to the AMW136,
	and waits either for an acknowledgment or a timeout. The timeout can be created by setting the
	global variable counts to zero, which will automatically increment every second, and waiting
	while counts < cnt. */
	
	//Start timer counter 
	counts = 0;
	
	//Write command to AMW136
	usart_write_line(BOARD_USART, comm);
	
	//Wait until timeout or ack 
	while(counts<cnt){
		
		if(strncmp(UART_RxBuf, "ack", strlen("ack")) == 0) {
			printf("AMW136 received the data")
			break;
		}
	}
	
	if(counts>cnt){
		printf("timeout happened")
	}
	
}

void process_data_wifi(void){  
	
	/*Processes the response of the AMW136, which should be stored
	in the buffer filled by process incoming byte wifi. This processing should be looking for certain
	responses that the AMW136 should give, such as “start transfer” when it is ready to receive the
	image. */
	
	if (strncmp(UART_RxBuf, "start transfer", strlen("start transfer")) == 0){
		
		//Start transfer
	}
	
}

void write_image_to_file(void){
	
	/*Writes an image from the SAM4S8B to the AMW136. If the
	length of the image is zero (i.e. the image is not valid), return. Otherwise, follow this protocol
	(illustrated in Appendix B):
	
	1. Issue the command “image transfer xxxx”, where xxxx is replaced by the length of the
	image you want to transfer.
	
	2. After the AMW136 acknowledges that it received your command, start streaming the image.
	
	3. After the image is done sending, the AMW136 should say “Complete”. However, the “command
	complete” pin will not have a rising edge, so it will be hard to sense. You can still try
	to sense it before moving on, or simply insert a slight delay. */
	
	//timeout limit (in secs)
	int cnt = 5;
	
	if(image_len == 0){
		printf("No image");
		return;
	}
	else{
		//Send command to signal image transfer as well as the length of the image
		char* comm = strcat("image_transfer ",char*(image_len));
		write_wifi_command(comm,cnt);
		//verify that AMW136 has gotten the command 
		while(counts < cnt){}
		
		if(ack == 0){
			//AMW136 did not receive the command
			return;
		}
		else{
			//Start the image transfer
			
		}
		
	}
}
