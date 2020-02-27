/*
 * camera.h
 *
 * Created: 2/26/2020 4:10:58 PM
 *  Author: ece-lab3
 */ 


#ifndef CAMERA_H_
#define CAMERA_H_

#include "asf.h"

/* TWI board defines. */
#define ID_BOARD_TWI                   ID_TWI0
#define BOARD_TWI                      TWI0
#define BOARD_TWI_IRQn                 TWI0_IRQn

/* iRAM board defines. */
#define SRAM_BASE                      (0x60000000UL) // SRAM adress
#define SRAM_CS                        (0UL)
#define CAP_DEST                       SRAM_BASE

/* Image sensor board defines. */
#define IMAGE_WIDTH                    (640UL) 
#define IMAGE_HEIGHT                   (480UL)
// Image sensor VSYNC pin.
#define OV_VSYNC_GPIO              PIO_PA15_IDX
#define OV_VSYNC_PIO	           PIOA
#define OV_VSYNC_ID		           ID_PIOA
#define OV_VSYNC_MASK              PIO_PA15 
#define OV_VSYNC_TYPE              PIO_PULLUP
#define OV_VSYNC_FLAGS            (PIO_PULLUP | PIO_IT_RISE_EDGE)
/** OV_HSYNC pin definition */
#define OV_HSYNC_GPIO                  PIO_PA16_IDX
#define OV_HSYNC_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define OV_HSYNC_MASK                  PIO_PA16
#define OV_HSYNC_PIO                   PIOA
#define OV_HSYNC_ID                    ID_PIOA
#define OV_HSYNC_TYPE                  PIO_PULLUP

// Image sensor data pin.
#define OV_DATA_BUS_PIO            PIOA
#define OV_DATA_BUS_ID             ID_PIOA


static void vsync_handler(uint32_t ul_id, uint32_t ul_mask);
void init_vsync_interrupts(void);
void configure_twi(void);
void pio_capture_init(Pio *p_pio, uint32_t ul_id);
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);
void init_camera(void);
void configure_camera(void);
void start_capture(void);
uint8_t find_image_len(void);

#endif /* CAMERA_H_ */