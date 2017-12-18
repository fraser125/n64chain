# The Serial Interface
## About
## Usage
There are only 2 api calls to manage the button and analog input for all of the standard controller types. Generally the code would be placed in the following locations. 

Define the number of controller ports to monitor. This can be a variable, constant or a define depending if there is a need to change it at runtime.

```C
	uint32_t num_controller_ports = 4;

	void main() {
	...
```

The controller data structs are global therefore available for use in any method or function. Below is a simple example.
Near the bottom of the main while loop is where the VI Interrupt is generally processed. By wrapping the Serial Interface calls around this call we have a consistent point so the Controllers are polled 60 times / second.

```C
	while(1) {
	// Other Game/Program Code
		// Read Structs, 0 if not read yet so no errors.
		uint32_t controller_idx 0;
		if (si_port[controller_idx].device.dev_type.id == N64_CONTROLLER)
		{
			if (si_port[controller_idx].status.controller.buttons & BUTTON_A)
			{
				// DO Something
			}
			if (si_port[controller_idx].status.controller.joyx != 0
			{
				// Move Something
			}
		}
		
	// Other Game/Program Code
	
		si_prepare_request(num_controller_ports); // SI 
		libn64_recv_message(); // Standard Code
		// Maybe process Audio here, again for consistent loading of the Audio Buffer
		si_process_response(); // SI
	}
```

## Train Controller i.e. Densha de Go! custom Controller

The Train controller identifies itself as a standard controller, the best way to tell the game that it's a Train controller is asking the user to press Start + A at the same time. In the button #defines it's clear that the A button has a different value between the Train and standard controllers. Using the Start button makes it a very concious action on the part of the user. 

As a programmer present a (Controller Selection) screen that prompts all users to press "Start + A". while calling this api.

```C
	si_controller_manual_identify()
```

FYI: Pressing "Start + B" while calling the API will restore the Standard Controller setting. Effectively toggling modes by pressing "Start + A" then "Start + B". 

## Supported Controller Struct formats

	The struct holding the controller input data is 'si_port', it's a 4 element array of device and status information. The supported devices each have their own nested struct. 
	
	The device id (i.e. si_port[0].device.dev_type.id) may have the following values:
		N64_CONTROLLER the standard "Tri-wing" controller.
			si_port[0].status.controller.buttons
			si_port[0].status.controller.joyx
			si_port[0].status.controller.joyy
		N64_DANCEPAD the Disney Dancepad (Id's the same as a controller)
		N64_VRU Voice Recognition Unit
			Handled seperately only identified by this api.
		N64_MOUSE The mouse that is included with the 64DD Mario Artist game.
			si_port[0].status.mouse.buttons
			si_port[0].status.mouse.movex
			si_port[0].status.mouse.movey
		N64_KEYBOARD The RandNET keyboard
			Identified by this api.
			TODO:
		N64_DENSHA The Train Controller
			si_port[controller_idx].status.densha_de_go.buttons
		
	The accessory value (i.e. si_port[0].device.dev_type.acc) may have the following values
		0x02 Nothing in the Accessory port
		0x01 Something is in the Accessory port
		
	The device error field is available and has defined values for it, but just by testing the device id it's possible to tell if the data is valid.

	See the example project or the Header file for button bit masks.
	
## Performance
A lot of thought and research went into making this api simple, flexible and perform well.
1. Simple 2 api calls, with a couple of helper methods
2. Flexible the 2 api's manage the different states of a controller from first being plugged in to idenfying and querying them for their button or analog status. 
3. Performance the two apis take less than 150 CPU cycles to complete, which is good considering they are called on every frame.

If the game/program only needs 1 controller port, set the num_controller_ports value to 1. It's nice to be flexible but the API will be much faster if only 1 controllers input is needed. 

The secret to maximizing the performance is to do some other work between the calling of these two apis. If all 4 ports are used at least 3,300 CPU cycles are available between the api calls.