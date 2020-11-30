
#include "sdk_project_config.h"
#include <interrupt_manager.h>
#include "pin_mux.h"
#include "phy.h"
#include "phy_tja110x.h"
#include "enet_driver.h"

volatile int exit_code = 0;

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "S32K148.h" /* include peripheral declarations S32K148 */
#include "clocks_and_modes.h"
#include "LPUART.h"

char data=0;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

void PORT_init (void) {
PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
PORTC->PCR[6]|=PORT_PCR_MUX(2); /* Port C6: MUX = ALT2,UART1 TX */
PORTC->PCR[7]|=PORT_PCR_MUX(2); /* Port C7: MUX = ALT2,UART1 RX */
}
void WDOG_disable (void){
WDOG->CNT=0xD928C520; /* Unlock watchdog */
WDOG->TOVAL=0x0000FFFF; /* Maximum timeout value */
WDOG->CS = 0x00002100; /* Disable watchdog */
}
uint8_t packet;




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct {
    uint8_t destAddr[6];
    uint8_t srcAddr[6];
    uint16_t etherType;
    uint16_t length;
    uint8_t payload[1500];
} mac_frame_t;

mac_frame_t frame;
//mac_frame_t TxFrame;	// get one buffer on heap
mac_frame_t *RxFrame;	// buffer data comes from ENET driver
bool PhyLinkUp;

uint8_t ethMacAddrBroadcast[6] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
uint8_t ethMacAddrTgt[6] = {0x11U, 0x22U, 0x33U, 0x77U, 0x88U, 0x99U};

void link_up(uint8_t phy)
{
	// this demo only supports 1 external phy (TJA1100 or TJA1101)
	if (phy == 0)
		PhyLinkUp = true;
}


void link_down(uint8_t phy)
{
	// this demo only supports 1 external phy (TJA1100 or TJA1101)
	if (phy == 0)
		PhyLinkUp = false;
}

void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}

void copyBuff(uint8_t *dest, uint8_t *src, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

void rx_callback(uint8_t instance, enet_event_t event, uint8_t ring)
{
    (void)instance;

    if (event == ENET_RX_EVENT)
    {
        enet_buffer_t buff;
        status_t status;

        status = ENET_DRV_ReadFrame(INST_ENET, ring, &buff, NULL);
        if (status == STATUS_SUCCESS)
        {
            mac_frame_t *frame;

            frame = (mac_frame_t *) buff.data;

            /* You can process the payload here */
            (void)frame->payload;

            /* We successfully received a frame -> turn on LED 2 */
            PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
            PINS_DRV_SetPins(GPIO_PORT, (1 << LED2));

            ENET_DRV_ProvideRxBuff(INST_ENET, ring, &buff);
        }
    }
}


int main(void)
{

	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
	                 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	  status_t error;
	  	  /* Configure clocks for PORT */
	  	  error = CLOCK_DRV_Init(&clockMan1_InitConfig0);
	  	  DEV_ASSERT(error == STATUS_SUCCESS);
	  	  /* Set pins as GPIO */
	  	  error = PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
	  	  DEV_ASSERT(error == STATUS_SUCCESS);
	  	WDOG_disable(); /* Disable WDGO*/
	  	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	  	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	  	//RUNmode_80MHz(); /* Init clocks: 80 MHz SPLL & core, 40 MHz bus, 20 MHz flash */
	  	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz SPLL & core, 40 MHz bus, 20 MHz flash */
	  	PORT_init(); /* Configure ports */
	  	LPUART1_init(); /* Initialize LPUART @ 9600*/
	  	LPUART1_transmit_string("Running LPUART\n\r"); /* Transmit char string */
	  	LPUART1_transmit_string("Tx_Rx_Bluetooth\n\r"); /* Transmit char string */
	  	LPUART1_transmit_string("Press 1 for 0x0800- Internet Protocol version 4 (IPv4)\n\r");
		LPUART1_transmit_string("Press 2 for 0x22F0- EtherType Audio Video Transport Protocol (AVTP)\n\r ");
		LPUART1_transmit_string("Press 3 for  0x0806	Address Resolution Protocol (ARP) \n\r ");
		LPUART1_transmit_string("Enter number of packets to be transmited \n\r ");
		packet=LPUART1_receive_char();
		LPUART1_transmit_char('+');
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		   for(;;)
		{
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);


    /* Turn off LED 1 and LED 2 */
    PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
    PINS_DRV_ClearPins(GPIO_PORT, (1 << LED2));

    /* Disable MPU */
    MPU->CESR = 0x00815200;

    /* Initialize ENET instance */
    ENET_DRV_Init(INST_ENET, &enetState, &enetInitConfig, enet_buffConfigArr, enet_MacAddr);



    enet_buffer_t buff, rxBuff;
    enet_tx_enh_info_t txInfo;
    enet_rx_enh_info_t rxInfo;
   // mac_frame_t frame;
    uint8_t i;
    uint8_t j=0;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void LPUART1_receive_and_echo_char(void)
//{

	char send = LPUART1_receive_char(); /* Receive Char */


    if(send=='1')
	  {

	   	LPUART1_transmit_string("IPv4 \n");
	    PINS_DRV_SetPins(LED0_PORT, 1 << LED0_PIN);
	    PINS_DRV_ClearPins(LED1_PORT, 1 << LED1_PIN);

	    frame.etherType = 0x0008;
	     for (i = 0; i < 2U; i++)
	       {
	           frame.payload[i] = i;
	           LPUART1_transmit_string("0800");
	       }
	     }
	else if (send=='2')
	{
		LPUART1_transmit_string("AVTP\n");
		PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
		PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

		frame.etherType = 0xF022;
			     for (i = 0; i < 64U; i++)
			       {
			           frame.payload[i] = i;
			           LPUART1_transmit_string("22F0");
			      }
	}
	else if (send=='3')
		{
			LPUART1_transmit_string("ARP\n");
			PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
			PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

			frame.etherType = 0x0608;
				     for (i = 0; i < 64U; i++)
				       {
				           frame.payload[i] = i;
				           LPUART1_transmit_string("0806");
				      }
		}

	else
	{
		LPUART1_transmit_string("Invalid input\n");

     }
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    copyBuff(frame.destAddr, enet_MacAddr, 6U);
    copyBuff(frame.srcAddr, enet_MacAddr, 6U);
 //   frame.length = 50U;

    buff.data = (uint8_t *)&frame;
    /* Length == 12 bytes MAC addresses + 2 bytes length + 50 bytes payload */
    buff.length = 64U + 14;

    ENET_DRV_EnableMDIO(INST_ENET, false);
    status_t state;

    state = PHY_FrameworkInit(phyConfig, phyDrivers);
    state = PHY_Init(0);

    bool linkStatus;

    /* Infinite loop::
     *    - Send frames
     */
         while (j <= packet)
    {
        	 state = PHY_GetLinkStatus(0, &linkStatus);
        	 if (linkStatus)
        	 	  {
        	 		  PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
        	 		  PINS_DRV_SetPins(GPIO_PORT, (1 << LED2));
        	 	  }
        	 	  else
        	 	  {
        	 	      PINS_DRV_ClearPins(GPIO_PORT, (1 << LED2));
        	 	      PINS_DRV_SetPins(GPIO_PORT, (1 << LED1));
        	 	  }



        	 if (linkStatus)
        	 	  {
        	 		  state = ENET_DRV_SendFrame(INST_ENET, 0U, &buff, NULL);
        	 		  if (state != STATUS_SUCCESS)
        	 		  {
        	 			  PINS_DRV_SetPins(GPIO_PORT, (1 << LED1));
        	 			  PINS_DRV_SetPins(GPIO_PORT, (1 << LED2));
        	 		  }
        	 		 state = ENET_DRV_GetTransmitStatus(INST_ENET,0U, &buff, &txInfo);

        	 		state = ENET_DRV_ReadFrame(INST_ENET, 0U, &rxBuff, &rxInfo);

        	 		if (state==STATUS_SUCCESS)
        	 				  {
        	 					  /*
        	 					   * do something with received data here
        	 					   */

        	 					  RxFrame = (mac_frame_t *)rxBuff.data;

        	 					  // pull internal data-buffer back to driver pool
        	 					  ENET_DRV_ProvideRxBuff(INST_ENET,0U, &rxBuff);
        	 				  }
        	 			  }
        	 		      delay(700000);	// 1000 ms
        	 		  j ++;
    					}



           }
}
