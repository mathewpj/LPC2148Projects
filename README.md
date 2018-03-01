# LPC2148Projects
Projects for LPC 2148 
1) Ensure the following setting in KEL uVision IDE when compiling the programs
  Options for Target -> Linker -> "Use Memory Layout from Target Dialog"
2) Enable Hex file o/p in Options for Target -> Output -> Create HEX File

RTOS Port for LPC2148
1) Ensure that FreeRTOS 10.0.0 or greater is installed on your system

Folder : Peripherals
	1) LED_Blink: 	Basic program to check the IDE settings, 
 	            	toolchain, programmer is working as expected.
		      	Flashes a LED is everything is correct.

	2) REAL_Time_Clock_with_ISR: Implements a Real Time Clock
			in ISR mode. Displays the running clock on LCD.

	3) Real_Time_Counter: Implements a Real Time Clock in polling
			   mode. Displays the running clokc on LCD.

	4) SDCard:



