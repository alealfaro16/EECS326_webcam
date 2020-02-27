/*
 * timer_interface.h
 *
 * Created: 2/26/2020 4:13:02 PM
 *  Author: ece-lab3
 */ 


#ifndef TIMER_INTERFACE_H_
#define TIMER_INTERFACE_H_


#include <asf.h>
#include <string.h>
#include <stdio.h>

/** Timer counter frequency in Hz. */
#define TC_FREQ             1

volatile uint8_t counts;

void configure_tc(void);


#endif /* TIMER_INTERFACE_H_ */