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

#include "APDS9960.h"


#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)

/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
QueueHandle_t q1;
typedef struct {
	uint8_t ID;
	uint8_t r, g, b;
}Params;

static void printResults(uint16_t* _al, uint16_t* _rl, uint16_t* _gl, uint16_t* _bl){
	uint16_t arr_recv[4];

	while((xQueueReceive( q1, &arr_recv, (TickType_t)10 ) == pdPASS ));

	// procesar valores y printarlos en caso de ser correctos
	// imitar funcionamiento dl bucle vaya
	if(!readAmbientLight(&_al) || !readRedLight(&_rl) || !readGreenLight(&_gl) || !readBlueLight(&_bl))
		printf("Error llegint el registre de llum!!\n");
	else
		printf("Llum total, components RGB: %5d, %5d, %5d, %5d\n", al, rl, gl, bl);
}

static void readLightSensor()
{
  vTaskDelay(pdMS_TO_TICKS(500)); // Esperar a que s'inicialitzi el xip

  uint16_t al = 255; //Total
  uint16_t rl = 255; //Red
  uint16_t gl = 255; //Green
  uint16_t bl = 255; //Blue

  uint16_t v_arr[4];
  while(true) {

    if(!readAmbientLight(&al) || !readRedLight(&rl) || !readGreenLight(&gl) || !readBlueLight(&bl))
      printf("Error llegint el registre de llum!!\n");
    else{
      v_arr[0] = al; v_arr[1] = rl; v_arr[2] = gl; v_arr[3] = bl;
      int del = 1000;
      xQueueSend(q1, &v_arr, 0);
      printf("Llum total, components RGB: %5d, %5d, %5d, %5d\n", al, rl, gl, bl);
    }
    // vTaskDelay(pdMS_TO_TICKS(1000)); // Porsiaca esperem


	 vTaskDelay(pdMS_TO_TICKS(1000)); // Porsiaca esperem
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

  // Params* p1 =(Params*)malloc(sizeof(Params));
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

  /*
  p1->ID = 1;
  p1->r = 0;
  p1->g = 0;
  p1->b = 0;*/
  //p2->ID = 1;
  /*Create two task for blinking leds*/
  //p1->queue1 = xQueueCreate(2, sizeof(int));
  //p1->queue2 = xQueueCreate(2, sizeof(int));
  //p2->queue1 = p1->queue1;
  //p2->queue2 = p1->queue2;

  if (!Init()) {
    printf("Error a l'inicialitzar l'APDS9960!\n");
    return 1;
  }
  if (!enableLightSensor(false)) {
    printf("Error a l'inicialitzar en sensor de llum!\n");
    return 1;
  }

  if((q1 = xQueueCreate(7, sizeof(int))) == NULL){
	  printf("Error creando la cola\n");
	  return -1;
  }
  // xQueueCreate(3, sizeof(int)); // 3 tasques

  xTaskCreate(readLightSensor, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(printResults, (const char *), "PrintResults", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  //xTaskCreate(readLightSensor, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, (void*)p2, TASK_PRIORITY, NULL);

  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();
  // free(p1);
  return 0;
}
