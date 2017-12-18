//
// helloworld/src/main.c: Threading test cart (entry point).
//
// n64chain: A (free) open-source N64 development toolchain.
// Copyright 2017 James Wall <jkwall@hotmail.com>
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include <os/fbtext.h>
#include <rcp/vi.h>
#include <stdint.h>
#include <syscall.h>
#include <si.h>


// These pre-defined values are suitable for NTSC.
// TODO: Add support for PAL and PAL-M televisions.
static vi_state_t vi_state = {
  0x0000324E, // status
  0x00200000, // origin
  0x00000140, // width
  0x00000002, // intr
  0x00000000, // current
  0x03E52239, // burst
  0x0000020D, // v_sync
  0x00000C15, // h_sync
  0x0C150C15, // leap
  0x006C02EC, // h_start
  0x002501FF, // v_start
  0x000E0204, // v_burst
  0x00000200, // x_scale
  0x00000400, // y_scale
};


uint32_t num_controller_ports = 4;

void main(void *unused __attribute__((unused))) {

	struct libn64_fbtext_context context;
	
	// Setup the OS's private/hidden text rendering engine.
	// The 0 and ~0 are just fill colors (black and white).
	libn64_fbtext_init(&context, 0x80000000 | vi_state.origin, ~0, 0, 320, LIBN64_FBTEXT_16BPP);

	// Register VI interrupts on this thread. When registering a thread w/
	// interrupts, it causes the threads message queue to get populated w/
	// a message each time an interrupt fires.
	libn64_thread_reg_intr(libn64_thread_self(), LIBN64_INTERRUPT_VI);

	si_init();
	
	struct timeval startPrepareTime;
	struct timeval endPrepareTime;

	struct timeval startWaitTime;
	struct timeval endWaitTime;

	struct timeval startProcessTime;
	struct timeval endProcessTime;
	

	uint32_t preptime = 0, waittime = 0, processtime = 0;
	// For each frame...
	while (1) {
		unsigned i;

		// Point to VI to the last fb, swap the front and back fbs.
		vi_flush_state(&vi_state);
		vi_state.origin ^= 0x100000; // 1MB

		// Wipe the back-buffer to black.
		for (i = 0; i < 320 * 240 * 2; i += 16) {
		  __asm__ __volatile__(
			".set gp=64\n\t"
			"cache 0xD, 0x0(%0)\n\t"
			"sd $zero, 0x0(%0)\n\t"
			"sd $zero, 0x8(%0)\n\t"
			"cache 0x19, 0x0(%0)\n\t"
			".set gp=default\n\t"

			:: "r" (0x80000000 | (vi_state.origin + i))
			: "memory"
		  );
		}

		// Tell the text engine to render on the back buffer.
		context.fb_origin = 0x80000000 | vi_state.origin;
		context.x = 38;
		context.y = 13;
		libn64_fbtext_puts(&context, "X");

		si_controller_manual_identify();

		// X 1, 10, 21, 30
		context.x = 3;
		context.y = 1;
		libn64_fbtext_puts(&context, "Controller Demo - (Strt + A | B)");

		for(uint32_t controller_idx = 0; controller_idx < MAX_CONTROLLERS; controller_idx++)
		{
			uint32_t left = (controller_idx * 9)+ 1;
			context.y = 2;
			context.x = left;
			switch(si_port[controller_idx].device.dev_type.id)
			{
				case N64_CONTROLLER:
				{
					libn64_fbtext_puts(&context, "Standard");
					context.x = left;
					context.y = 3;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_A)
						libn64_fbtext_puts(&context, " A");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_B)
						libn64_fbtext_puts(&context, " B");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_Z)
						libn64_fbtext_puts(&context, " Z");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_START)
						libn64_fbtext_puts(&context, " S");
					else
						context.x += 2;
					context.x = left;
					context.y = 4;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_DUP)
						libn64_fbtext_puts(&context, "DU");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_DLEFT)
						libn64_fbtext_puts(&context, "DL");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_DDOWN)
						libn64_fbtext_puts(&context, "DD");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_DRIGHT)
						libn64_fbtext_puts(&context, "DR");
					else
						context.x += 2;
					context.x = left;
					context.y = 5;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_CUP)
						libn64_fbtext_puts(&context, "CU");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_CLEFT)
						libn64_fbtext_puts(&context, "CL");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_CDOWN)
						libn64_fbtext_puts(&context, "CD");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_CRIGHT)
						libn64_fbtext_puts(&context, "CR");
					else
						context.x += 2;
					context.x = left;
					context.y = 6;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_RESET)
						libn64_fbtext_puts(&context, "RS");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_TLEFT)
						libn64_fbtext_puts(&context, "TL");
					else
						context.x += 2;
					if (si_port[controller_idx].status.controller.buttons & BUTTON_TRIGHT)
						libn64_fbtext_puts(&context, "TR");
					else
						context.x += 2;
					
					context.y = 7;
					context.x = left;
					libn64_fbtext_putu32(&context, si_port[controller_idx].status.controller.joyx);
					context.x = left;					
					libn64_fbtext_puts(&context, " X    ");

					context.y = 8;
					context.x = left;
					libn64_fbtext_putu32(&context, si_port[controller_idx].status.controller.joyy);
					context.x = left;					
					libn64_fbtext_puts(&context, " Y    ");
					
					context.y = 9;
					context.x = left + 5;
					uint32_t acc = si_port[controller_idx].device.dev_type.acc;
					if (acc == 0x1)
						libn64_fbtext_puts(&context, "YES");
					else
						libn64_fbtext_puts(&context, "NO");
					context.x = left;
					libn64_fbtext_puts(&context, " ACC:");
				}
				break;
				case N64_MOUSE:
				{
					libn64_fbtext_puts(&context, "  Mouse");
					context.y = 3;
					if (si_port[controller_idx].status.mouse.buttons & MOUSE_LEFT)
					{
						context.x = left + 2;
						libn64_fbtext_puts(&context, "Left");
					}
					context.y = 4;
					if (si_port[controller_idx].status.mouse.buttons & MOUSE_RIGHT)
					{
						context.x = left + 2;
						libn64_fbtext_puts(&context, "Right");
					}
					context.y = 7;
					context.x = left;
					libn64_fbtext_putu32(&context, si_port[controller_idx].status.mouse.movex);
					context.x = left;					
					libn64_fbtext_puts(&context, " X    ");

					context.y = 8;
					context.x = left;
					libn64_fbtext_putu32(&context, si_port[controller_idx].status.mouse.movey);
					context.x = left;					
					libn64_fbtext_puts(&context, " Y    ");
				}
				break;
				case N64_KEYBOARD:
				{
					libn64_fbtext_puts(&context, "Keyboard");
				}
				break;
				case N64_DENSHA:
				{
					libn64_fbtext_puts(&context, "  Train");
					context.x = left;
					context.y = 3;
					if (si_port[controller_idx].status.densha_de_go.buttons & BUTTON_START)
						libn64_fbtext_puts(&context, "ST");
					else
						context.x += 2;
					if (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_B_SELECT)
						libn64_fbtext_puts(&context, "SL");
					else
						context.x += 2;
					context.x = left;
					context.y = 4;
					if (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_BUTTON_A)
						libn64_fbtext_puts(&context, " A");
					else
						context.x += 2;
					if (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_BUTTON_B)
						libn64_fbtext_puts(&context, " B");
					else
						context.x += 2;
					if (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_BUTTON_C)
						libn64_fbtext_puts(&context, " C");
					else
						context.x += 2;
					
					context.x = left + 1;
					context.y = 5;
					
					uint32_t master = (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_MASK_MST);
					libn64_fbtext_putu32(&context, master);
					context.x = left;
					libn64_fbtext_puts(&context, "MST: ");
					
					context.x = left + 1;
					context.y = 6;
					uint32_t brake = (si_port[controller_idx].status.densha_de_go.buttons & DENSHA_MASK_BRK);
					libn64_fbtext_putu32(&context, brake);
					context.x = left;
					libn64_fbtext_puts(&context, "BRK: ");					
				}
				break;
				case N64_VRU:
				{
					libn64_fbtext_puts(&context, "Voice");
				}
				break;
				case N64_UNKNOWN:
				default:
				{
					libn64_fbtext_puts(&context, " Unknown ");
				}
				break;
			}			
			
			context.y = 10;
			context.x = left;
			libn64_fbtext_putu32(&context, si_port[controller_idx].device.raw);
			
			uint32_t status_raw0 = si_port[controller_idx].status.raw[0];
			context.y = 11;
			context.x = left;
			libn64_fbtext_putu32(&context, status_raw0);
					
			// Only the keyboard has 2 raw uint32_t so hide unless relevant
			uint32_t status_raw1 = si_port[controller_idx].status.raw[1];
			if (status_raw1 > 0)
			{
				context.y = 12;
				context.x = left;
				libn64_fbtext_putu32(&context, status_raw1);
			}
		}
		
		//Prep: 0000  Wait: 0000  Proc: 0000
		context.x = 3;
		context.y = 13;
		libn64_fbtext_putu32(&context, preptime);
		context.x = 1;
		libn64_fbtext_puts(&context, "Prep: ");

		context.x = 15;
		libn64_fbtext_putu32(&context, waittime);
		context.x = 12;
		libn64_fbtext_puts(&context, " Wait: ");
		
		context.x = 27;
		libn64_fbtext_putu32(&context, processtime);
		context.x = 25;
		libn64_fbtext_puts(&context, "Proc: ");


		startPrepareTime = libn64_time();
		si_prepare_request(num_controller_ports);
		endPrepareTime = libn64_time();
		preptime = (endPrepareTime.tv_usec - startPrepareTime.tv_usec) << 1;

		startWaitTime = libn64_time();
		si_dma_wait();
		endWaitTime = libn64_time();
		waittime = (endWaitTime.tv_usec - startWaitTime.tv_usec) << 1;

		// Block until the next VI interrupt comes in.
		libn64_recv_message();

		startProcessTime = libn64_time();
		si_process_response();
		endProcessTime = libn64_time();
		processtime = (endProcessTime.tv_usec - startProcessTime.tv_usec) << 1;
	}
}