//
// libn64/include/si.h: SI helper functions.
//
// n64chain: A (free) open-source N64 development toolchain.
// Copyright 2017 James Wall <jkwall@hotmail.com>
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef LIBN64_INCLUDE_RCP_SI_H
#define LIBN64_INCLUDE_RCP_SI_H

#include <libn64.h>
#include <stdint.h>

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#define MAX_CONTROLLERS 4
#define INVALID_CONTROLLER_COUNT -1
#define NO_ERROR 0

#define BUTTON_A		0x8000
#define BUTTON_B		0x4000
#define BUTTON_Z		0x2000
#define BUTTON_START	0x1000

#define BUTTON_DUP		0x0800
#define BUTTON_DDOWN	0x0400
#define BUTTON_DLEFT	0x0200
#define BUTTON_DRIGHT	0x0100

#define BUTTON_RESET	0x0080
//#define BUTTON_UNUSED	0x0040
#define BUTTON_TLEFT	0x0020
#define BUTTON_TRIGHT	0x0010

#define BUTTON_CUP		0x0008
#define BUTTON_CDOWN	0x0004
#define BUTTON_CLEFT 	0x0002
#define BUTTON_CRIGHT 	0x0001

#define MOUSE_LEFT		BUTTON_A
#define MOUSE_RIGHT		BUTTON_B

#define STANDARD_TYPE_TEST (BUTTON_START | BUTTON_A)

// https://assemblergames.com/threads/densha-de-go-64-train-controller-info.56965/
#define DENSHA_MASK_BRK	0x000F
#define DENSHA_MASK_MST 0x2900

#define DENSHA_MASTER_0	0x2800	// 0010 1000 0000 0000
#define DENSHA_MASTER_1	0x2100	// 0010 0001 0000 0000
#define DENSHA_MASTER_2	0x2000	// 0010 0000 0000 0000
#define DENSHA_MASTER_3	0x0900	// 0000 1001 0000 0000
#define DENSHA_MASTER_4	0x0800	// 0000 1000 0000 0000
#define DENSHA_MASTER_5	0x0100	// 0000 0001 0000 0000								

#define DENSHA_BRK_INVAL	0x000F
#define DENSHA_BRK_14	0x000E
#define DENSHA_BRK_13	0x000D
#define DENSHA_BRK_12	0x000C
#define DENSHA_BRK_11	0x000B
#define DENSHA_BRK_10	0x000A
#define DENSHA_BRK_09	0x0009
#define DENSHA_BRK_08	0x0008
#define DENSHA_BRK_07	0x0007
#define DENSHA_BRK_06	0x0006
#define DENSHA_BRK_05	0x0005
#define DENSHA_BRK_04	0x0004
#define DENSHA_BRK_03	0x0003
#define DENSHA_BRK_02	0x0002
#define DENSHA_BRK_01	0x0001
#define DENSHA_BRK_OFF	0x0000

#define DENSHA_B_SELECT	0x0010
#define DENSHA_B_START	0x1000

#define DENSHA_BUTTON_A	0x4000
#define DENSHA_BUTTON_B	0x8000
#define DENSHA_BUTTON_C	0x0020

#define DENSHA_TYPE_TEST (DENSHA_B_START | DENSHA_BUTTON_A)

#define SI_STATUS_DMA_BUSY		(1 << 0)
#define SI_STATUS_IO_BUSY		(1 << 1)
#define SI_STATUS_DMA_ERROR		(1 << 3)
#define SI_STATUS_BUSY_ERROR_6	(1 << 6) // Not sure but they would flip high while waiting for PIF
#define SI_STATUS_BUSY_ERROR_8	(1 << 8) // Not sure but they would flip high while waiting for PIF
#define SI_STATUS_INTERRUPT		(1 << 12)

#define N64_UNPLUGGED	0xFFFF
#define N64_UNKNOWN		0x0000
#define N64_CONTROLLER	0x0500
#define N64_DANCEPAD	0x0500
#define N64_VRU			0x0001
#define N64_MOUSE		0x0200
#define N64_KEYBOARD	0x0002
#define N64_DENSHA		0x2004
#define GBA_HANDHELD	0x0004

#define KEYBOARD_NUMLCK	0x01000000
#define KEYBOARD_CAPLCK	0x02000000
#define KEYBOARD_POWER	0x04000000

#define SI_REQ_FILL_FF 	0xFFFFFFFF
#define SI_REQ_CONT_QY	0xFF010300
#define SI_REQ_CONT_ST	0xFF010401
#define SI_REQ_KYBD_ST	0xFF020713
#define SI_REQ_FINL_CH	0xFE000000

#define SI_PSTAT_ZERO	0x00
#define SI_PSTAT_ERROR	0x01
#define SI_PSTAT_CHANG	0x02
#define SI_PSTAT_IGNOR	0x04
#define SI_PSTAT_ERR_1	0x08
#define SI_PSTAT_READY	0x10
#define SI_PSTAT_GSTAT	0x20
#define SI_PSTAT_PERR4	0x40 // Port Overrun?
#define SI_PSTAT_NORES	0x80 // No response
#define true 1
#define false 0
// TODO bool?

typedef struct si_request_t {
	union
	{
		uint8_t bytes[64];
		uint16_t shorts[32];
		uint32_t uints[16];
	} raw;
} _si_request_t __attribute__ ((aligned (16)));

typedef struct si_port_t {
	union
	{
		struct si_device_type_t {
			uint16_t id;
			uint8_t acc;
			uint8_t state;		
		} dev_type;
		uint32_t raw;
	} device;
	union 
	{
		struct si_controller_state_t {
			uint16_t buttons;
			uint8_t joyx;
			uint8_t joyy;
		} controller;
		
		struct si_mouse_state_t {
			uint16_t buttons;
			uint8_t movex;
			uint8_t movey;
		} mouse;
		
		struct si_keyboard_state_t {
			uint8_t raw[7];
			uint8_t led;
		} keyboard;
		
		struct si_densha_de_go_t {
			uint16_t buttons;
		} densha_de_go;
		
		struct si_dancepad_t {
			uint32_t raw;
		} dancepad;
		
		struct si_fishing_rod_t {
			uint32_t raw;
		} fishing_rod;
		uint32_t raw[2];
	} status;
} _si_port_t;

typedef struct si_io_port_t {
	struct si_io_device_type_t {
		uint16_t identifier;
		uint8_t acc_state;
		uint8_t errors;
	} _si_io_device_type_t;
	union
	{
		struct controller_pak_t {
			uint8_t raw[32];
		} _controller_pak_t;
		
		struct si_voice_recognition_t {
			uint32_t raw;
		} _si_voice_recognition_t;
		
	} _data_t;
} _si_io_port_t;

struct si_port_t si_port[MAX_CONTROLLERS];

 struct si_request_t si_buffer_tx __attribute__ ((aligned (16)));
 struct si_request_t si_buffer_rx __attribute__ ((aligned (16)));		
 uint32_t si_buffer_tx_ram_addr; // = (((uint32_t)&si_buffer_tx) & 0x1FFFFF);
 uint32_t si_buffer_rx_ram_addr;// = (((uint32_t)&si_buffer_rx) & 0x1FFFFF);

void si_init();
void si_init_tx_buffer();
void si_init_devices();
void si_dma_wait();
void si_send_request(uint32_t buffer_addr);
void si_recv_response(uint32_t buffer_addr);
uint32_t si_prepare_request(uint32_t controller_count);
uint32_t si_process_response();
void si_controller_user_identify();

libn64func
static inline uint32_t si_status(void) {
	return *(volatile const uint32_t *) 0xA4800018;
}

libn64func
static inline void si_set_dram_addr(uint32_t buffer_addr) {		
	*(volatile uint32_t *) 0xA4800000 = buffer_addr;
}

libn64func
static inline void si_dma_read(uint32_t addr) {
	*(volatile uint32_t *) 0xA4800004 = addr;
}

libn64func
static inline void si_dma_write(uint32_t addr) {
	*(volatile uint32_t *) 0xA4800010 = addr;
}

#endif