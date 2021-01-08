#ifndef APP_H_
#define APP_H_

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


#define EVB

#ifdef EVB
#define GPIO_PORT PTE
#define PCC_CLOCK PCC_PORTE_CLOCK
#define LED1 21U
#define LED2 22U
#else
#define GPIO_PORT PTC
#define PCC_CLOCK PCC_PORTC_CLOCK
#define LED1 0 U
#define LED2 1 U
#endif

typedef struct {
  uint8_t destAddr[6];
  uint8_t srcAddr[6];
  uint16_t etherType;
  uint16_t length;
  uint8_t payload[1500];
}
mac_frame_t;

mac_frame_t frame;
//mac_frame_t TxFrame;	// get one buffer on heap
mac_frame_t * RxFrame; // buffer data comes from ENET driver
bool PhyLinkUp;

uint8_t ethMacAddrBroadcast[6] = {0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU};
uint8_t ethMacAddrTgt[6] = {0x11U,0x22U,0x33U,0x77U,0x88U,0x99U};

void LPUART1_init(void) /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */ {
  PCC -> PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
  PCC -> PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b001) /* Clock Src= 1 (SOSCDIV2_CLK) */ |
    PCC_PCCn_CGC_MASK; /* Enable clock for LPUART1 regs */
  LPUART1 -> BAUD = 0x0F000034; /* Initialize for 9600 baud, 1 stop: */
  LPUART1 -> CTRL = 0x000C0000; /* Enable transmitter & receiver, no parity, 8 bit char: */
}

void LPUART1_transmit_char(char send) {
  /* Function to Transmit single Char */
  while ((LPUART1 -> STAT & LPUART_STAT_TDRE_MASK) >> LPUART_STAT_TDRE_SHIFT == 0);
  /* Wait for transmit buffer to be empty */
  LPUART1 -> DATA = send; /* Send data */
}

void LPUART1_transmit_string(char data_string[]) {
  /* Function to Transmit whole string */
  uint32_t i = 0;
  while (data_string[i] != '\0') {
    /* Send chars one at a time */
    LPUART1_transmit_char(data_string[i]);
    i++;
  }
}

char LPUART1_receive_char(void) {
  /* Function to Receive single Char */
  char receive;
  while ((LPUART1 -> STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT == 0);
  /* Wait for received buffer to be full */
  receive = LPUART1 -> DATA; /* Read received data*/
  return receive;
}

void delay(volatile int cycles)

{
  /* Delay function - do nothing for a number of cycles */

  while (cycles--);
}

void copyBuff(uint8_t * dest, uint8_t * src, uint32_t len) {
  uint32_t i;

  for (i = 0; i < len; i++) {
    dest[i] = src[i];
  }
}

#endif /* APP_H_ */
