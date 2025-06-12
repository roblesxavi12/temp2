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
#include "String.h"

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define MAX_STRING_SIZE

/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
QueueHandle_t q1;
QueueHandle_t q2;
QueueHandle_t q3;

typedef struct {
	uint8_t ID;
	uint8_t r, g, b;
}Params;

uint8_t isr_flag = 0;
char buf[MAX_STRING_SIZE];
char buf2[MAX_STRING_SIZE];
uint8_t proximity_data = 0;

static void handleGesture(){
  
  if ( isGestureAvailable() ) {
    switch ( readGesture() ) {
      case DIR_UP:
        printf("UP\n");
        strcpy(buf, "UP", MAX_STRING_SIZE);
        break;
      case DIR_DOWN:
        printf("DOWN\n");
        strcpy(buf, "DOWN", MAX_STRING_SIZE);
        break;
      case DIR_LEFT:
        printf("LEFT\n");
        strcpy(buf, "LEFT", MAX_STRING_SIZE);
        break;
      case DIR_RIGHT:
        printf("RIGHT\n");
        strcpy(buf, "RIGHT", MAX_STRING_SIZE);
        break;
      case DIR_NEAR:
        printf("NEAR\n");
        strcpy(buf, "NEAR", MAX_STRING_SIZE);
        break;
      case DIR_FAR:
        printf("FAR\n");
        strcpy(buf, "FAR", MAX_STRING_SIZE);
        break;
      default:
        printf("NONE\n");
        strcpy(buf, "NONE", MAX_STRING_SIZE);
    }
  } 
  if (xQueueSend(q2, buf, pdMS_TO_TICKS(10)) != pdPASS) {
        printf("Error al enviar string a la cola\n");
        return errQUEUE_FULL;
  }
}

static void printGestureResults(){
  uint8_t i = 4;
  
  while(i != 0){
    if(xQueueReceive( q2, buf2, (TickType_t)10 ) != pdPASS ){
      printf("Error al recibir el dato de la cola\n");
      return errQUEUE_EMPTY;
    }
    printf("Gesto recibido: %s\n", buf2);
    vTaskDelay(pdMS_TO_TICKS(1000))
    i--;
  }
}

static void readGestureSensor(){
  // los delays se hacen en las funciones internas. Aqui se añade un pequeño delay solo
  uint8_t i = 4;

  // xQueueSend en handleGesture
  while(i != 0){
    if( isr_flag == 1 ) {
      detachInterrupt(0); // FUNCION DE ARDUINO
      handleGesture();
      isr_flag = 0;
      attachInterrupt(0, interruptRoutine, FALLING); // FUNCION DE ARDUINO
    }
    i--;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }  
}


static void printLightResults(){
	uint16_t arr_recv[4];

  // Esto se deberia cambiar, la cola solo recibe un array en vez de los elementos 1 a 1
	while(1){
    if(xQueueReceive( q1, arr_recv, (TickType_t)10 ) == pdPASS )
      printf("Llum total, components RGB: %5u, %5u, %5u, %5u\n", arr_recv[0], arr_recv[1], arr_recv[2], arr_recv[3]);
    vTaskDelay(pdMS_TO_TICKS(1000))
  }	
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
      if(xQueueSend(q1, v_arr, 0)!= pdPASS){
        printf("Error enviando a la cola de ints\n");
        return errQUEUE_FULL;
      }
      // printf("Llum total, components RGB: %5d, %5d, %5d, %5d\n", al, rl, gl, bl);
    }
	  vTaskDelay(pdMS_TO_TICKS(1000)); // Porsiaca esperem
  }
}

static void readProximitySensor(){
  uint8_t i = 4;

  while(i != 0){
    if(!readProximity(proximity_data)){
      printf("Error reading proximity\n");
      return READ_PROX_DATA_ERROR;
    }
    if(xQueueSend(q3, proximity_data, 0) != pdPASS){
      printf("Error enviando datos del sensor de proximidad\n");
      return errQUEUE_FULL;
    }
    i--;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

static void printProximityResults(){
  uint8_t i = 4;
  uint8_t prox_receiv = -1;
  while(i != 0){
    if(!xQueueReceive( q3, prox_receiv, (TickType_t)10 ) == pdPASS ){
      printf("Error recibiendo valor de la cola de lecturas de proximidad\n");
      return errQUEUE_EMPTY;
    }
    printf("Valor de proximidad: %u", prox_receiv);
    i--;
    vTaskDelay(pdMS_TO_TICKS(1000));
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
  // BSP_LedsInit();
  /* Setting state of leds*/
  // BSP_LedSet(0);
  // BSP_LedSet(1);

  /*
    DOCS: https://cdn.sparkfun.com/assets/learn_tutorials/3/2/1/Avago-APDS-9960-datasheet.pdf
  */

  if (!Init()) {
    printf("Error a l'inicialitzar l'APDS9960!\n");
    return INIT_ERROR;
  }
  if (!enableLightSensor(false)){ // interrupts -> false
    printf("Error a l'inicialitzar en sensor de llum!\n");
    return ENABLE_LIGHT_ERRROR;
  }

  if(!enableProximity(false)){ // interrupts -> false
    printf("Error inicializando el sensor de proximidad\n");
    return ENABLE_PROX_ERROR;
  }

  if(!enableGesture(false)){ // interrupts -> false
    printf("Error inicializando el sensor de gestos\n")
    return ENABLE_GEST_ERROR
  }

  if((q1 = xQueueCreate(7, sizeof(uint16_t) * 4)) == NULL){ // cola light sensor
	  printf("Error creando la cola de ints\n");
	  return ERROR_CREATING_XQUEUE;
  }

  if((q2 = xQueueCreate(4, sizeof(char) * MAX_STRING_SIZE)) == NULL){ // cola gesture sensor (lista para almacenar 4 lecturas)
	  printf("Error creando la cola de strings\n");
	  return ERROR_CREATING_XQUEUE;
  }

  if((q3 = xQueueCreate(4, sizeof(uint8_t) * 4)) == NULL){ // cola proximity sensor (lista para almacenar 4 lecturas)
	  printf("Error creando la cola de strings\n");
	  return ERROR_CREATING_XQUEUE;
  }

  xTaskCreate(readLightSensor, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(printLightResults, (const char *) "PrintResults", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  xTaskCreate(readGestureSensor, (const char *) "gestureSensor", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(printGestureResults, (const char *) "gestureResults", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  xTaskCreate(readProximitySensor, (const char *) "proximitySensor", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(printGProximityResults, (const char *) "proximityResults", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();
  // free(p1);
  return 0;
}
