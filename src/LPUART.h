#ifndef LPUART_H_
#define LPUART_H_

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

#include "S32K148.h" /* include peripheral declarations S32K144 */
#include "LPUART.h"

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

char LPUART1_receive_char(void) { /* Function to Receive single Char */
char receive;
while((LPUART1->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
/* Wait for received buffer to be full */
receive= LPUART1->DATA; /* Read received data*/
return receive;
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



void LPUART1_eth_sender(void) {

for(;;)
{

		//to select packet
	uint8_t packet;
	LPUART1_transmit_string("Enter number of packets to transmit \n\r "
			"\n\r Press 1: Send 50 packets \n\r"
			"\n\r Press 2: Send 100 packets \n\r"
			"\n\r Press 3: Send 200 packets \n\r");


packet= (uint8_t)(LPUART1_receive_char());
uint8_t a;



	if(packet=='1')
	{
		a=49;
		LPUART1_transmit_string("50 packets will be transmitted\n\r");
		LPUART1_transmit_string("  \n\r");
	}
	else if(packet=='2')
		{
			a=99;
			LPUART1_transmit_string("100 packets will be transmitted\n\r");
			LPUART1_transmit_string("  \n\r");
		}
	else if(packet=='3')
			{
				a=199;
				LPUART1_transmit_string("200 packets will be transmitted\n\r");
				LPUART1_transmit_string("  \n\r");
			}
	else
	{
		LPUART1_transmit_string("Invalid Input\n\r");
	}


	//to select delay
	LPUART1_transmit_string("Select the transmission delay: Slow or Fast\n\r "
					"\n\r Press s: To send Slow  \n\r"
					"\n\r Press f: To send Fast  \n\r");
	 LPUART1_transmit_string("  \n\r");
	uint8_t delay_select= (uint8_t)(LPUART1_receive_char());

	LPUART1_transmit_string("Select the Ether Types from below option\n\r");
	LPUART1_transmit_string("Press 4 for 0x0800- Internet Protocol version 4 (IPv4)\n\r");
	LPUART1_transmit_string("  \n\r");
	LPUART1_transmit_string("Press 5 for 0x22F0- EtherType Audio Video Transport Protocol (AVTP)\n\r ");
	LPUART1_transmit_string("  \n\r");
	LPUART1_transmit_string("Press 6 for  0x0806	Address Resolution Protocol (ARP) \n\r ");
	LPUART1_transmit_string("  \n\r");
//	for(;;)
//			{
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
	    uint16_t j=0;

	    char send = LPUART1_receive_char(); /* Receive Char */


	        if(send=='4')
	    	  {

	    	   	LPUART1_transmit_string("IPv4 running... \n");
	    	    PINS_DRV_SetPins(LED0_PORT, 1 << LED0_PIN);
	    	    PINS_DRV_ClearPins(LED1_PORT, 1 << LED1_PIN);

	    	    frame.etherType = 0x0008;
	    	     for (i = 0; i < 64U; i++)
	    	       {
	    	           frame.payload[i] = i;
	    	           //LPUART1_transmit_string("0800");
	    	       }
	    	     }
	    	else if (send=='5')
	    	{
	    		LPUART1_transmit_string("\nAVTP running...\n");
	    		PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
	    		PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

	    		frame.etherType = 0xF022;
	    			     for (i = 0; i < 64U; i++)
	    			       {
	    			           frame.payload[i] = i;
	    			         //  LPUART1_transmit_string("22F0");
	    			      }
	    	}
	    	else if (send=='6')
	    		{
	    			LPUART1_transmit_string("\n ARP running...\n");
	    			PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
	    			PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

	    			frame.etherType = 0x0608;
	    				     for (i = 0; i < 64U; i++)
	    				       {
	    				           frame.payload[i] = i;
	    				          // LPUART1_transmit_string("0806");
	    				      }
	    		}

	    	else
	    	{
	    		LPUART1_transmit_string("Invalid input\n");

	         }


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

	       while(j<=a)
//	            	 while (1)

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
	            	 		j ++;

	            	 			  }
	            	 if(delay_select== 's')
	            	 	{
	            		 delay(7000000);

	            	 	}
	            	 	else if(delay_select=='f')
	            	 	{
	            	 		delay(70000);

	            	 	}
	            	 	else
	            	 	{
	            	 	LPUART1_transmit_string("Invalid Input\n\r");
	            	 	}

//	            	delay(70000);	// 1000 ms
	            }


}
}
#endif /* LPUART_H_ */
