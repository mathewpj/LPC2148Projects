/* A simple example	to check if pre-emptive scheduling works
*  led_pattern1() & led_pattern2() are called alternatively
*  To keep this simple mutexes have not been used to serialize
*  accesses to the leds (shared resource).  
*  mathew.p.joseph@gmail.com
*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "support.h"


void led_pattern1(void* p_void)
{
	int i;
	int pattern1 = 0x01;
	int pattern2 = 0xFF;
  
	while(1){ 	 
 		/* Turning on LEDS */
		for(i=0;i<=7;i++){
			IO0SET = pattern1;  /*	  pin   P0.[0...7]   goes HIGH  */ 
			delay(900000);
		}
    
		/* Turning off LEDS */
		for(i=0;i<=7;i++){
 			IO0CLR = pattern2;  /*	  pin   P0.[7...0]   goes LOW  */ 
   			delay(900000);
   		}
   	}
}

void led_pattern2(void* p_void)
{
	int i;
	int pattern1 = 0x80;
	int pattern2 = 0xFF;
    
	while(1){ 	 
		/* Turning on LEDS */
		for(i=0;i<=7;i++){
			IO0SET = pattern1;  /*	  pin   P0.[0...7]   goes HIGH  */ 
			delay(900000);
		}
      		
		/* Turning off LEDS */
		for(i=0;i<=7;i++){
 			IO0CLR = pattern2;  /*	  pin   P0.[7...0]   goes LOW  */ 
   			delay(900000);
   		}
   	}
}

		 
int main()
{
	 void* ptr;
	/******* HW initialization common to both led_pattern1 & led_pattern1 ******/
	// Ideally need to configure PINSEL0 register.
	// Since system powers up with default config as P0.0
	// no need to explicitly configure PINSEL0. Anyway doing it...
	PINSEL0 = 0x00000000;
	// Setting 0xFF is sufficient since we control only P0.0 to P0.7
	IO0DIR |= 0X000000FF;	
	/***************************************************************************/

	xTaskCreate(led_pattern1, "LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL); 
	xTaskCreate(led_pattern2, "LED2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL); 
    vTaskStartScheduler();
	while(1); // We shoudnt be reaching here 
}
