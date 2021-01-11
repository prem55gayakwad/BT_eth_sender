#include "app.h"


extern void app_eth_sender(void) { //function to transmit Ether-Type

  LPUART1_init(); /* Initialize LPUART @ 9600*/
  LPUART1_transmit_string("  \n\r");
  LPUART1_transmit_string("Running LPUART\n\r"); /* Transmit char string */
  LPUART1_transmit_string("Tx_Rx_Bluetooth\n\r"); /* Transmit char string */
  LPUART1_transmit_string("  \n\r");

  for (;;) {

    //to select packet
    uint8_t packet;
    LPUART1_transmit_string("Enter number of packets to transmit \n\r "
      "\n\r Press 1: Send 50 packets \n\r"
      "\n\r Press 2: Send 100 packets \n\r"
      "\n\r Press 3: Send 200 packets \n\r");

    packet = (uint8_t)(LPUART1_receive_char());
    uint8_t a;

    if (packet == '1') {
      a = 49;
      LPUART1_transmit_string("50 packets will be transmitted\n\r");
      LPUART1_transmit_string("  \n\r");
    } else if (packet == '2') {
      a = 99;
      LPUART1_transmit_string("100 packets will be transmitted\n\r");
      LPUART1_transmit_string("  \n\r");
    } else if (packet == '3') {
      a = 199;
      LPUART1_transmit_string("200 packets will be transmitted\n\r");
      LPUART1_transmit_string("  \n\r");
    } else {
      LPUART1_transmit_string("Invalid Input\n\r");
    }

    //to select delay
    LPUART1_transmit_string("Select the transmission delay: Slow or Fast\n\r "
      "\n\r Press s: To send Slow: 7000ms  \n\r"
      "\n\r Press f: To send Fast: 70ms  \n\r");
    LPUART1_transmit_string("  \n\r");
    uint8_t delay_select = (uint8_t)(LPUART1_receive_char());

    LPUART1_transmit_string("Select the Ether Types from below option\n\r");
    LPUART1_transmit_string("Press 4 for 0x0800- Internet Protocol version 4 (IPv4)\n\r");
    LPUART1_transmit_string("  \n\r");
    LPUART1_transmit_string("Press 5 for 0x22F0- EtherType Audio Video Transport Protocol (AVTP)\n\r ");
    LPUART1_transmit_string("  \n\r");
    LPUART1_transmit_string("Press 6 for  0x0806	Address Resolution Protocol (ARP) \n\r ");
    LPUART1_transmit_string("  \n\r");
    //	for(;;) {

    /* Turn off LED 1 and LED 2 */
    PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
    PINS_DRV_ClearPins(GPIO_PORT, (1 << LED2));

    /* Disable MPU */
    MPU -> CESR = 0x00815200;

    /* Initialize ENET instance */
    enet_buffer_t buff, rxBuff;
    enet_tx_enh_info_t txInfo;
    enet_rx_enh_info_t rxInfo;
    // mac_frame_t frame;
    uint8_t i;
    uint16_t j = 0;

    char send = LPUART1_receive_char(); /* Receive Char */

    if (send == '4') {

      LPUART1_transmit_string("IPv4 running... \n");
      PINS_DRV_SetPins(LED0_PORT, 1 << LED0_PIN);
      PINS_DRV_ClearPins(LED1_PORT, 1 << LED1_PIN);

      frame.etherType = 0x0008;   //0800-->0008
      for (i = 0; i < 64U; i++) {
        frame.payload[i] = i;
        //LPUART1_transmit_string("0800");
      }
    } else if (send == '5') {
      LPUART1_transmit_string("\nAVTP running...\n");
      PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
      PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

      frame.etherType = 0xF022;  //
      for (i = 0; i < 64U; i++) {
        frame.payload[i] = i;
        //  LPUART1_transmit_string("22F0");
      }
    } else if (send == '6') {
      LPUART1_transmit_string("\n ARP running...\n");
      PINS_DRV_SetPins(LED0_PORT, 1 << LED1_PIN);
      PINS_DRV_ClearPins(LED1_PORT, 1 << LED0_PIN);

      frame.etherType = 0x0608;  //
      for (i = 0; i < 64U; i++) {
        frame.payload[i] = i;
        // LPUART1_transmit_string("0806");
      }
    } else {
      LPUART1_transmit_string("Invalid input\n");

    }

    copyBuff(frame.destAddr, enet_MacAddr, 6U);
    copyBuff(frame.srcAddr, enet_MacAddr, 6U);
    //   frame.length = 50U;

    buff.data = (uint8_t * ) & frame;
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

    while (j <= a)
    //	            	 while (1)

    {

      state = PHY_GetLinkStatus(0, & linkStatus);
      if (linkStatus) {
        PINS_DRV_ClearPins(GPIO_PORT, (1 << LED1));
        PINS_DRV_SetPins(GPIO_PORT, (1 << LED2));
      } else {
        PINS_DRV_ClearPins(GPIO_PORT, (1 << LED2));
        PINS_DRV_SetPins(GPIO_PORT, (1 << LED1));
      }

      if (linkStatus) {
        state = ENET_DRV_SendFrame(INST_ENET, 0U, & buff, NULL);
        if (state != STATUS_SUCCESS) {
          PINS_DRV_SetPins(GPIO_PORT, (1 << LED1));
          PINS_DRV_SetPins(GPIO_PORT, (1 << LED2));
        }
        state = ENET_DRV_GetTransmitStatus(INST_ENET, 0U, & buff, & txInfo);

        state = ENET_DRV_ReadFrame(INST_ENET, 0U, & rxBuff, & rxInfo);

        if (state == STATUS_SUCCESS) {
          /*
           * do something with received data here
           */

          RxFrame = (mac_frame_t * ) rxBuff.data;

          // pull internal data-buffer back to driver pool
          ENET_DRV_ProvideRxBuff(INST_ENET, 0U, & rxBuff);
        }
        j++;

      }
      if (delay_select == 's') {
        delay(7000000);

      } else if (delay_select == 'f') {
        delay(70000);

      } else {
        LPUART1_transmit_string("Invalid Input\n\r");
      }

    }

  }
}
