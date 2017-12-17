//
// libn64/include/os/si.h: SI helper functions.
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

void si_init()
{
	si_buffer_tx_ram_addr = (((uint32_t)&si_buffer_tx) & 0x1FFFFF);
	si_buffer_rx_ram_addr = (((uint32_t)&si_buffer_rx) & 0x1FFFFF);
	si_init_tx_buffer();
	si_init_devices();
}

void si_init_tx_buffer()
{
	for(uint32_t idx = 0; idx < 16; idx++)
		si_buffer_tx.raw.uints[idx] = 0x0;
}

void si_init_devices()
{
	for(uint32_t idx = 0;idx < MAX_CONTROLLERS; idx++)
	{
		si_port[idx].device.raw = 0x0;
		si_port[idx].status.raw[0] = 0x0;
		si_port[idx].status.raw[1] = 0x0;
	}
}

void si_dma_wait()
{
	uint32_t status = 0;
	do
	{
		status = si_status();
	}
	while (status > 0);
}

void si_send_request(uint32_t buffer_addr)
{
	si_dma_wait();	
	si_set_dram_addr(buffer_addr);
	si_dma_write(0x1FC007C0);
}

void si_recv_response(uint32_t buffer_addr)
{
	si_dma_wait();
	si_set_dram_addr(buffer_addr);
	si_dma_read(0x1FC007C0);
}