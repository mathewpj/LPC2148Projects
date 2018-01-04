#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "support.h"
	

void led_pattern(void* p_void)
{
	int i;
	int pattern1[8] = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};
	int pattern2[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
    // Ideally need to configure PINSEL0 register.
	// Since system powers up with default config as P0.0
	// no need to explicitly configure PINSEL0. Anyway doing it...

	PINSEL0 = 0x00000000;
			
	while(1){ 	 
		// Setting 0xFF is sufficient since we control only P0.0 to P0.7
		IO0DIR |= 0X000000FF;
		for (i=0;i<1000;i++);  /* Power-On Delay (15 mS)    */

 		/* Turning on LEDS */
		for(i=0;i<=7;i++){
			IO0SET = pattern1[i];  /*	  pin   P0.[0...7]   goes HIGH  */ 
			delay(9000000);
		}
      		
		/* Turning off LEDS */
		delay(9000000);
		for(i=0;i<=7;i++){
 			IO0CLR = pattern2[i];  /*	  pin   P0.[7...0]   goes LOW  */ 
   			delay(9000000);
   		}
   	}
}

		 
int main()
{
	 void* ptr;
	 xTaskCreate(led_pattern, "LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL); 
     vTaskStartScheduler();
	 while(1); // We shoudnt be reaching here 
}
