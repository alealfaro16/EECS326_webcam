/*
 * camera.h
 *
 * Created: 2/23/2020 7:47:09 PM
 *  Author: ece-lab3
 */ 


#ifndef CAMERA_H_
#define CAMERA_H_

/* TWI board defines. */
#define ID_BOARD_TWI                   ID_TWI0
#define BOARD_TWI                      TWI0
#define BOARD_TWI_IRQn                 TWI0_IRQn

/* SRAM board defines. */
#define SRAM_BASE                      (0x60000000UL) // SRAM adress
#define SRAM_CS                        (0UL)
#define CAP_DEST                       SRAM_BASE

/* Image sensor board defines. */
#define IMAGE_WIDTH                    (320UL) //What should this be?
#define IMAGE_HEIGHT                   (240UL)
// Image sensor VSYNC pin.
#define OV7740_VSYNC_PIO	           PIOA
#define OV7740_VSYNC_ID		           ID_PIOA
#define OV7740_VSYNC_MASK              PIO_PA15 //Need to change pin to actual pin
#define OV7740_VSYNC_TYPE              PIO_PULLUP
// Image sensor data pin.
#define OV7740_DATA_BUS_PIO            PIOA
#define OV7740_DATA_BUS_ID             ID_PIOA


void vsync_handler(uint32_t ul_id, uint32_t ul_mask);
void init_vsync_interrupts(void);
void configure_twi(void);
void pio_capture_init(Pio *p_pio, uint32_t ul_id);
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);
void init_camera(void);
void configure_camera(void);
uint8_t start_capture(void);
uint8_t find_image_len(void);


#endif /* CAMERA_H_ */
