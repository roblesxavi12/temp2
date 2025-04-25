#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "bsp_i2c.h"


#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)

/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
typedef struct {
	uint8_t ID;
	uint8_t r, g, b;
	//QueueHandle_t* queue1;
	//QueueHandle_t* queue2;
}Params;
static void LedBlink(void *pParameters)
{
  //(void) pParameters;
  //int* pParameters2 = (int*)pParameters;

  Params* params = (Params*)pParameters;
  uint32_t del = 500;

  for (;;) {
	//BSP_LedToggle(LED);
	//params->ID = 10;
	bool check;
	//check = BSP_I2C_ReadRegister((uint8_t)0x92, &params->ID);
	check = BSP_I2C_WriteRegister((uint8_t)0x97, (uint8_t)0x00); // Prueba de reset del registro
	check = BSP_I2C_ReadRegister((uint8_t)0x97, &params->r);
	check = BSP_I2C_ReadRegister((uint8_t)0x98, &params->g);
	check = BSP_I2C_ReadRegister((uint8_t)0x9A, &params->b);
	printf("Error: %d\n", check);
	//check = 0;
	printf("red: %d\n", params->r);
	//printf("green: %d\n", params->g);
	//printf("blue: %d\n", params->b);
	vTaskDelay(pdMS_TO_TICKS(del));
	//xQueueSend( params->queue1, &del, 0 );
  }
  /*
  if (LED == 1) {
	  del = 2000;
	  xQueueSend( params->queue1, &del, (TickType_t)10 );

	  for (;;) {
		if( xQueueReceive( params->queue2, &del, (TickType_t)10 ) == pdPASS )
		{
			BSP_LedToggle(LED);
			printf("Task %d\n", LED);
			vTaskDelay(pdMS_TO_TICKS(del));
			xQueueSend( params->queue1, &del, 0 );
		}
	  }
  }
  else {
	  xQueueSend( params->queue2, &del, (TickType_t)10 );

	  for (;;) {
		if( xQueueReceive( params->queue1, &del, (TickType_t)10 ) == pdPASS )
		{
			BSP_LedToggle(LED);
			printf("Task %d\n", LED);
			vTaskDelay(pdMS_TO_TICKS(del));
			xQueueSend( params->queue2, &del, 0 );
		}
	  }
  }*/
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize LED driver */
  BSP_LedsInit();
  /* Setting state of leds*/
  BSP_LedSet(0);
  BSP_LedSet(1);

  Params* p1 =(Params*)malloc(sizeof(Params));
  //Params* p2 =(Params*)malloc(sizeof(Params));
  // Registro de dispositivo -> 0x72
  /*
   * DOCS: https://cdn.sparkfun.com/assets/learn_tutorials/3/2/1/Avago-APDS-9960-datasheet.pdf
   	0x96 RDATAL R Low byte of red channel data 0x00
    0x97 - RDATAH R High byte of red channel data - 0x00
	0x98 - GDATAL R Low byte of green channel data - 0x00
	0x99 - GDATAH R High byte of green channel data - 0x00
	0x9A - BDATAL R Low byte of blue channel data - 0x00
	0x9B - BDATAH R High byte of blue channel data - 0x00
	0x9D POFFSET_UR R/W Proximity offset for UP and RIGHT photodiodes 0x00
	0x9E POFFSET_DL R/W Proximity offset for DOWN and LEFT photodiodes 0x00

   */
  BSP_I2C_Init((uint8_t)0x72);
  p1->ID = 1;
  p1->r = 0;
  p1->g = 0;
  p1->b = 0;
  //p2->ID = 1;
  /*Create two task for blinking leds*/
  //p1->queue1 = xQueueCreate(2, sizeof(int));
  //p1->queue2 = xQueueCreate(2, sizeof(int));
  //p2->queue1 = p1->queue1;
  //p2->queue2 = p1->queue2;


  xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, (void*)p1, TASK_PRIORITY, NULL);
  //xTaskCreate(LedBlink, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, (void*)p2, TASK_PRIORITY, NULL);


  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();
  free(p1);
  return 0;
}
