/*
 **************************************************************
 * MCP7940M.c
 * Author: Tom
 * Date: 17/11/2023
 * AVR Library for the real time clock and calendar chip,
 * MCP7940M. This library uses the Peter Fleury i2cmaster.h
 * interface to drive the i2c comms.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * RTC_init() - Initialise the RTC.
 * RTC_set_time() - Set the current time on the RTC.
 * RTC_get_current_time() - Get the current time on the RTC.
 **************************************************************
*/

#include <avr/io.h>

#include "MCP7940M.h"
#include "i2cmaster.h"

uint8_t RTC_TIME[6];

/*
 * RTC_init()
 * -----------
 * Initialise the RTC and enable external oscillator.
*/
void RTC_init() {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(RTC_SECONDS_REGISTER);
	i2c_write(RTC_OSCILLATOR_ENABLE);
	i2c_stop();
}

/*
 * RTC_read_one_register()
 * -----------------------
 * Internal function to read a single register on the RTC.
*/
uint8_t RTC_read_one_register(uint8_t regAddr) {
	uint8_t data;
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(regAddr);
	i2c_rep_start(RTC_ADDR | RTC_READ);
	data = i2c_readNak();
	i2c_stop();
	return data;
}

/*
 * RTC_read_multiple_register()
 * -----------------------------
 * Internal function to read multiple registers on the RTC. The number of reads
 * required must be known.
*/
void RTC_read_multiple_register(uint8_t startAddr, uint8_t* data, uint8_t numOfReads) {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(startAddr);
	i2c_stop();
	
	i2c_start(RTC_ADDR | RTC_READ);
	
	for (uint8_t i = 0; i < numOfReads - 1; i++) {
		data[i] = i2c_readAck();
	}
	data[numOfReads - 1] = i2c_readNak();
	i2c_stop();
	
}

/*
 * RTC_write_register()
 * ---------------------
 * Internal function to write a value to a register on the RTC.
*/
void RTC_write_register(uint8_t regAddr, uint8_t data) {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(regAddr);
	i2c_write(data);
	i2c_stop();
}

/*
 * RTC_set_time()
 * ---------------
 * Set the time on the RTC.
*/
void RTC_set_time(uint8_t hour, uint8_t min, uint8_t sec) {
	RTC_write_register(RTC_SECONDS_REGISTER, sec | RTC_OSCILLATOR_ENABLE);
	RTC_write_register(RTC_MINUTES_REGISTER, min);
	RTC_write_register(RTC_HOURS_REGISTER, hour);
}

/*
 * RTC_get_current_time()
 * ----------------------
 * Read the time keeping registers on the RTC and store the values in the RTC_TIME array.
*/
void RTC_get_current_time() {
	uint8_t rawTimeData[] = {0, 0, 0};	// Seconds, minutes, hours (needs to be in this order because of how the registers are set up on the MCP7940M)
	
	RTC_read_multiple_register(RTC_SECONDS_REGISTER, rawTimeData, 3);
	
	RTC_TIME[0] = (rawTimeData[0] & 0x70) >> 4;
	RTC_TIME[1] = (rawTimeData[0] & 0x0F);
	RTC_TIME[2] = (rawTimeData[1] & 0x70) >> 4;
	RTC_TIME[3] = (rawTimeData[1] & 0x0F);
	RTC_TIME[4] = (rawTimeData[2] & 0x30) >> 4;
	RTC_TIME[5] = (rawTimeData[2] & 0x0F);
}

