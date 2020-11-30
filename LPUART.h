#ifndef LPUART_H_
#define LPUART_H_

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "sdk_project_config.h"
#include <interrupt_manager.h>
#include "pin_mux.h"
#include "phy.h"
#include "phy_tja110x.h"
#include "enet_driver.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "sdk_project_config.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "S32K148.h" /* include peripheral declarations S32K144 */
#include "LPUART.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define EVB

#ifdef EVB
    #define GPIO_PORT   PTE
    #define PCC_CLOCK   PCC_PORTE_CLOCK
    #define LED1        21U
    #define LED2        22U
#else
    #define GPIO_PORT   PTC
    #define PCC_CLOCK   PCC_PORTC_CLOCK
    #define LED1        0U
    #define LED2        1U
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++






void LPUART1_init(void) /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b001) /* Clock Src= 1 (SOSCDIV2_CLK) */
| PCC_PCCn_CGC_MASK; /* Enable clock for LPUART1 regs */
LPUART1->BAUD = 0x0F000034; /* Initialize for 9600 baud, 1 stop: */

LPUART1->CTRL=0x000C0000; /* Enable transmitter & receiver, no parity, 8 bit char: */

}

void LPUART1_transmit_char(char send) { /* Function to Transmit single Char */
while((LPUART1->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
/* Wait for transmit buffer to be empty */
LPUART1->DATA=send; /* Send data */
}

void LPUART1_transmit_string(char data_string[]) { /* Function to Transmit whole string */
uint32_t i=0;
while(data_string[i] != '\0') { /* Send chars one at a time */
LPUART1_transmit_char(data_string[i]);
i++;
}
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#############
char LPUART1_receive_char(void) { /* Function to Receive single Char */
char receive;
while((LPUART1->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
/* Wait for received buffer to be full */
receive= LPUART1->DATA; /* Read received data*/
return receive;
}
void LPUART1_receive_and_echo_char(void) { /* Function to echo received char back */
char send = LPUART1_receive_char(); /* Receive Char */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SIM->MISCTRL0 |= SIM_MISCTRL0_RMII_CLK_OBE_MASK;
CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
	                 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);


	     /* Turn off LED 1 and LED 2 */
	     PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
	     PINS_DRV_ClearPins(GPIO_PORT, (1 << LED2));

	     /* Disable MPU */
	     MPU->CESR = 0x00815200;

	     /* Initialize ENET instance */
	     ENET_DRV_Init(INST_ENET, &enetState, &enetInitConfig, enet_buffConfigArr, enet_MacAddr);


if(send=='r')
{
   	LPUART1_transmit_string("Red LED1 \n");
    PINS_DRV_SetPins(LED0_PORT, 1 << LED0_PIN);
     PINS_DRV_ClearPins(LED1_PORT, 1 << LED1_PIN);
}
else if (send=='g')
{
	LPUART1_transmit_string("Green LED \n");
	PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
	  PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

}
else
{
	LPUART1_transmit_string("Incorrect Input");
	PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);
	PINS_DRV_ClearPins(LED1_PORT, 1 << LED1_PIN);
}

//+++++++++++++++++++++++++++++++++++++++######################################




LPUART1_transmit_char(send); /* Transmit same char back to the sender */
LPUART1_transmit_char('\n'); /* New line */
}
#endif /* LPUART_H_ */
