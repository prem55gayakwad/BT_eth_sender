#include "clocks_and_modes.h"

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

#include "S32K148.h"


void app_eth_sender(void); // function to transmit Ether-Type

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

volatile int exit_code = 0;

char data = 0;

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

void PORT_init(void) {
  PCC -> PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
  PORTC -> PCR[6] |= PORT_PCR_MUX(2); /* Port C6: MUX = ALT2,UART1 TX */
  PORTC -> PCR[7] |= PORT_PCR_MUX(2); /* Port C7: MUX = ALT2,UART1 RX */
}
void WDOG_disable(void) {
  WDOG -> CNT = 0xD928C520; /* Unlock watchdog */
  WDOG -> TOVAL = 0x0000FFFF; /* Maximum timeout value */
  WDOG -> CS = 0x00002100; /* Disable watchdog */
}

void link_up(uint8_t phy) {
  // this demo only supports 1 external phy (TJA1100 or TJA1101)
  if (phy == 0)
    PhyLinkUp = true;
}

void link_down(uint8_t phy) {
  // this demo only supports 1 external phy (TJA1100 or TJA1101)
  if (phy == 0)
    PhyLinkUp = false;
}

void rx_callback(uint8_t instance, enet_event_t event, uint8_t ring) {
  (void) instance;

  if (event == ENET_RX_EVENT) {
    enet_buffer_t buff;
    status_t status;

    status = ENET_DRV_ReadFrame(INST_ENET, ring, & buff, NULL);
    if (status == STATUS_SUCCESS) {
      mac_frame_t * frame;

      frame = (mac_frame_t * ) buff.data;

      /* You can process the payload here */
      (void) frame -> payload;

      /* We successfully received a frame -> turn on LED 2 */
      PINS_DRV_ClearPins(GPIO_PORT,(1<<LED1));
      PINS_DRV_SetPins(GPIO_PORT,(1<<LED2));

      ENET_DRV_ProvideRxBuff(INST_ENET, ring, & buff);
    }
  }
}

int main(void) {

  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
    g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

  ENET_DRV_Init(INST_ENET, & enetState, & enetInitConfig, enet_buffConfigArr, enet_MacAddr);

  status_t error;
  /* Configure clocks for PORT */
  error = CLOCK_DRV_Init( & clockMan1_InitConfig0);
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

  app_eth_sender(); //LPUART and Ether-Type

}
