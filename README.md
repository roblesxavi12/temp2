# temp2


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
	uint32_t ID;
	QueueHandle_t* queue1;
	QueueHandle_t* queue2;
}Params;
static void LedBlink(void *pParameters)
{
  //(void) pParameters;
  //int* pParameters2 = (int*)pParameters;

  Params* params = (Params*)pParameters;
  uint32_t LED = params->ID, del = 5000;


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
  }
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
  Params* p2 =(Params*)malloc(sizeof(Params));
  p1->ID = 0;
  p2->ID = 1;
  /*Create two task for blinking leds*/
  p1->queue1 = xQueueCreate(2, sizeof(int));
  p1->queue2 = xQueueCreate(2, sizeof(int));
  p2->queue1 = p1->queue1;
  p2->queue2 = p1->queue2;


  xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, (void*)p1, TASK_PRIORITY, NULL);
  xTaskCreate(LedBlink, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, (void*)p2, TASK_PRIORITY, NULL);


  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();
  free(p1); free(p2);
  return 0;
}
