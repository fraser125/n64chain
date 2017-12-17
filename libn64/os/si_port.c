//
// libn64/os/si_port.c: controller input functions.
//
// n64chain: A (free) open-source N64 development toolchain.
// Copyright 2017 James Wall <jkwall@hotmail.com>
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//
#include <libn64.h>
#include <stdint.h>
#include "si.h"

uint32_t si_prepare_request(uint32_t controller_count)
{	
	if (controller_count > MAX_CONTROLLERS)
		return INVALID_CONTROLLER_COUNT;

	if (controller_count <= 0x0)
		controller_count = 1;
	
	uint32_t new_request = false;
	uint32_t result = NO_ERROR;
	uint32_t buffer_offset = 0;	// uints so 4 bytes / offset
	for( uint32_t idx = 0; idx < controller_count; idx++)
	{				
		if ((si_port[idx].device.dev_type.state & SI_PSTAT_CHANG) > 0)
		{
			new_request = true;	 
			si_port[idx].device.dev_type.state = SI_PSTAT_ZERO;
		}
		
		switch(si_port[idx].device.dev_type.id)
		{
			case N64_CONTROLLER:
			case N64_MOUSE:
			case N64_DENSHA: // Query Controller Data			
			{		
				si_buffer_tx.raw.uints[buffer_offset] = SI_REQ_CONT_ST;											
				buffer_offset++;			
				si_buffer_tx.raw.uints[buffer_offset] = SI_REQ_FILL_FF;								
				buffer_offset++;				
			}
			break;			
			default: // Type Query or Reset
			{				
				si_buffer_tx.raw.uints[buffer_offset] = SI_REQ_CONT_QY;
				buffer_offset++;
				si_buffer_tx.raw.uints[buffer_offset] = SI_REQ_FILL_FF;				
				buffer_offset++;
			}
			break;
		}
	}
	si_buffer_tx.raw.uints[buffer_offset] = SI_REQ_FINL_CH;
	si_buffer_tx.raw.bytes[63] = 0x01;

	if (new_request)
	{
		si_send_request(si_buffer_tx_ram_addr);  // Time 0x06			
	}
	
	si_recv_response(si_buffer_rx_ram_addr); // Time 0x06	

	return result;
}

uint32_t si_process_response()
{
	si_dma_wait(); // Longest delay is potentially right here.
	uint32_t port_idx = 0;
	
	uint32_t result = NO_ERROR;
	for(uint32_t rx_uint_idx = 0;rx_uint_idx < 16; rx_uint_idx++)
	{		
		uint32_t requestCommand = si_buffer_rx.raw.uints[rx_uint_idx];
		uint32_t error = (requestCommand & 0xC000) >> 8;
		rx_uint_idx++;
		if (error > 0)
		{
			si_port[port_idx].device.raw = 0xFFFFFF00 | error | SI_PSTAT_CHANG;
			si_port[port_idx].status.raw[0] = 0x0;
			si_port[port_idx].status.raw[1] = 0x0;
		}
		else if (requestCommand == SI_REQ_CONT_QY)
		{
			uint32_t responseValue = si_buffer_rx.raw.uints[rx_uint_idx];
			si_port[port_idx].device.raw = (responseValue & 0xFFFFFF00) | SI_PSTAT_CHANG;
		}
		else if (requestCommand == SI_REQ_CONT_ST)
		{			
			si_port[port_idx].status.raw[0] = si_buffer_rx.raw.uints[rx_uint_idx];			
			result = SI_REQ_CONT_ST;
		}		
		else if (requestCommand == SI_REQ_KYBD_ST)
		{
			si_port[port_idx].status.raw[0] = si_buffer_rx.raw.uints[rx_uint_idx];			
			si_port[port_idx].status.raw[1] = si_buffer_rx.raw.uints[++rx_uint_idx];
			result = SI_REQ_KYBD_ST;
		}
		else if (requestCommand == SI_REQ_FINL_CH)
		{
			break; // exit for loop when 0xFE000000 is found. 
		}	
		else if (requestCommand == 0x0)
		{
			// Ignored
		}
		else
		{
			result = requestCommand;
		}
		port_idx++;
		if (port_idx > (MAX_CONTROLLERS - 1))
			break;
	}
	return result;
}

// If you want to offer the use of Non-Standard controllers
// 1. Promper user to Hold "Start + A" buttons
// 2. Call in a loop for 5-10 seconds or whatever you think your user needs.
void si_controller_user_identify()
{
	for(uint32_t idx = 0; idx < MAX_CONTROLLERS; idx++)
	{
		uint32_t buttons = (si_port[idx].status.controller.buttons);
		if ((buttons & DENSHA_TYPE_TEST) == DENSHA_TYPE_TEST)
			si_port[idx].device.raw = (N64_DENSHA << 16) | 0x0200;
		else if ((buttons & STANDARD_TYPE_TEST) == STANDARD_TYPE_TEST)
			si_port[idx].device.raw = (N64_CONTROLLER << 16) | 0x0200;
	}
}