/*
 * camera.c
 *
 * Created: 2/23/2020 8:10:02 PM
 *  Author: ece-lab3
 */ 

#include "camera.h"
#include "asf.h"
#include "ov2640.h"
#include "wifi.h"
#include "timer_interface.h"
#include <stdio.h>	/*printf*/
#include <stdlib.h>	/*malloc*/
#include <string.h> /*memset*/

/* Uncomment this macro to work in black and white mode */
#define DEFAULT_MODE_COLORED

#ifndef PIO_PCMR_DSIZE_WORD
#  define PIO_PCMR_DSIZE_WORD PIO_PCMR_DSIZE(2)
#endif

/* TWI clock frequency in Hz (400KHz) */
#define TWI_CLK     (400000UL)

/* Image data destination buffer */
uint8_t image_buffer[IMAGE_BUFFER_SIZE];

//Image length
uint32_t image_len = 0;

//Image start pointer
uint32_t img_start_pointer = 0;


/* Vsync signal information (true if it's triggered and false otherwise) */
static volatile uint32_t g_ul_vsync_flag = false;


/**
 * \brief Handler for vertical synchronisation using by the OV7740 image
 * sensor.
 */
static void vsync_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);

	g_ul_vsync_flag = true;
}


/**
 * \brief Intialize Vsync_Handler.
 */
 void init_vsync_interrupts(void)
{
	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(OV_VSYNC_PIO, OV_VSYNC_ID, OV_VSYNC_MASK,
			OV_VSYNC_TYPE, vsync_handler);

	/* Enable PIO controller IRQs */
	NVIC_EnableIRQ(PIOA_IRQn);//(IRQn_Type)OV_VSYNC_ID);
}



/**
 * \brief Initialize PIO capture for the OV7740 image sensor communication.
 *
 * \param p_pio PIO instance to be configured in PIO capture mode.
 * \param ul_id Corresponding PIO ID.
 */
 void pio_capture_init(Pio *p_pio, uint32_t ul_id)
{
	/* Enable periphral clock */
	pmc_enable_periph_clk(ul_id);

	/* Disable pio capture */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	/* Disable rxbuff interrupt */
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	/* 32bit width*/
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);

#if !defined(DEFAULT_MODE_COLORED)
	/* Samples only data with even index */
	p_pio->PIO_PCMR |= PIO_PCMR_HALFS;
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_FRSTS);
#endif
}


/**
 * \brief Capture OV7740 data to a buffer.
 *
 * \param p_pio PIO instance which will capture data from OV7740 image sensor.
 * \param p_uc_buf Buffer address where captured data must be stored.
 * \param ul_size Data frame size.
 */
 uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf,
		uint32_t ul_size)
{
	/* Check if the first PDC bank is free */
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)uc_buf;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
	} else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)uc_buf;
		p_pio->PIO_RNCR = ul_size;
		return 1;
	} else {
		return 0;
	}
}


/**
 * \brief Start picture capture.
 */
uint8_t start_capture(void)
{
	
	/* Set capturing destination address*/
	uint8_t *g_p_uc_cap_dest_buf = (uint8_t *)image_buffer;
	
	//Clear buffer
	memset(image_buffer,0,IMAGE_BUFFER_SIZE);

	/* Enable vsync interrupt*/
	pio_enable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	/* Capture acquisition will start on rising edge of Vsync signal.
	 * So wait g_vsync_flag = 1 before start process
	 */
	while (!g_ul_vsync_flag) {
	}

	/* Disable vsync interrupt*/
	pio_disable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	/* Enable pio capture*/
	pio_capture_enable(OV_DATA_BUS_PIO);

	/* Capture data and send it to external iRAM memory thanks to PDC
	 * feature */
	pio_capture_to_buffer(OV_DATA_BUS_PIO, g_p_uc_cap_dest_buf,IMAGE_BUFFER_SIZE>>2);

	/* Wait end of capture*/
	while (!((OV_DATA_BUS_PIO->PIO_PCISR & PIO_PCIMR_RXBUFF) == PIO_PCIMR_RXBUFF))  
	{
		
	}
	
	
	/* Disable pio capture*/
	pio_capture_disable(OV_DATA_BUS_PIO);

	/* Reset vsync flag*/
	g_ul_vsync_flag = false;
	
	//Find image length
	if(!find_image_len()){
		
		return 0;
	}
	else{
		
		return 1;
	}
	
	
}


/**
 * \brief Initialize PIO capture and the OV7740 image sensor.
 */
 void init_camera(void)
{
	
	/* OV7740 send image sensor data at 24 Mhz. For best performances, PCK0
	 * which will capture OV7740 data, has to work at 24Mhz. It's easier and
	 * optimum to use one PLL for core (PLLB) and one other for PCK0 (PLLA).
	 */
	pmc_enable_pllbck(7, 0x1, 1); /* PLLB work at 96 Mhz */

	/* Init Vsync handler*/
	init_vsync_interrupts();

	/* Init PIO capture*/
	pio_capture_init(OV_DATA_BUS_PIO, OV_DATA_BUS_ID);

	/* Init PCK1 to work at 24 Mhz */
	/* 96/4=24 Mhz */
	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK); 
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {
	}

	configure_twi();

}

void configure_twi(void){
	
	twi_options_t opt;
	
	/* Enable TWI peripheral */
	pmc_enable_periph_clk(ID_BOARD_TWI);

	/* Init TWI peripheral */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = TWI_CLK;
	twi_master_init(BOARD_TWI, &opt);

	/* Configure TWI interrupts */
	NVIC_DisableIRQ(BOARD_TWI_IRQn);
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn);
	NVIC_SetPriority(BOARD_TWI_IRQn, 0);
	NVIC_EnableIRQ(BOARD_TWI_IRQn);
}

void configure_camera(void){
	
	/* ov7740 Initialization */
	while (ov_init(BOARD_TWI) == 1) {
	}

	/* ov7740 configuration */
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_640x480);
	
	/* Wait 3 seconds to let the image sensor to adapt to environment */
	delay_ms(3000);
}


uint8_t find_image_len(void){  //Finds image length based on JPEG protocol. Returns 1 on success (i.e. able to find “end of image” and “start of image” markers), 0 on error.
	
	//Look at example on they access the PIO buffer 
	
	//checks that image in buffer starts with FF D8 FF
	uint8_t *capt_ptr =  (uint8_t *)image_buffer;  

	
	image_len = 0;
	uint32_t found_start = false;
	uint32_t found_end = false;
	uint32_t i; uint32_t j = 0;
	uint8_t real_image_buffer[25000];
	
	
	//Iterate through each hex value until finding the end of the JPEG protocol signature
	for(i=0;i<IMAGE_BUFFER_SIZE;i++){
		
		//Start of JPEG (FF D8 FF)
		if(image_buffer[i] == 0xff  && image_buffer[i+1] == 0xd8 && image_buffer[i+2] == 0xff ){

			found_start = true;
			//img_start_pointer = i;
		}
		
		//Start counting when start has been found
		if(found_start){
			//usart_putchar(BOARD_USART, image_buffer[i]);
			real_image_buffer[j] = image_buffer[i];
			j++;
			image_len++;
		}
		
		
		//Loop until finding the end markers FF D9 of a jpeg image
		if(found_start && image_buffer[i] == 0xff  && image_buffer[i+1] == 0xd9){
			found_end = true;
			//usart_putchar(BOARD_USART, image_buffer[i+1]);
			real_image_buffer[j] = image_buffer[i+1];
			image_len++;
			break;
		}
		
	}
	
	//delay_ms(1000);
	
	//If capture was succesful, attempt to send over to the wifi chip
	if(found_end  && found_start){
		write_image_to_file(real_image_buffer);
	}
	
	
}
