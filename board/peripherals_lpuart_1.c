/***********************************************************************************************************************
 * This file was generated by the S32 Configuration Tools. Any manual edits made to this file
 * will be overwritten if the respective S32 Configuration Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v7.0
processor: S32K148
package_id: S32K148_LQFP144
mcu_data: s32sdk_s32k1xx_rtm_400
processor_version: 0.0.0
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 23631c9c-96af-4ef1-a29b-83139b23b73e
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Included files 
 ******************************************************************************/
#include "peripherals_lpuart_1.h"

/*******************************************************************************
 * lpuart_1 initialization code
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'lpuart_1'
- type: 'lpuart_config'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'lpuart'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'LPUART_1'
- config_sets:
  - lpuart_driver:
    - lpuart_state_name: 'lpUartState0'
    - lpuart_configuration:
      - 0:
        - name: 'lpuart_0_InitConfig0'
        - readonly: 'true'
        - transferType: 'LPUART_USING_INTERRUPTS'
        - baudRate: '9600'
        - parityMode: 'LPUART_PARITY_DISABLED'
        - stopBitCount: 'LPUART_ONE_STOP_BIT'
        - bitCountPerChar: 'LPUART_8_BITS_PER_CHAR'
        - rxDMAChannel: '0'
        - txDMAChannel: '0'
    - quick_selection: 'dv_lpuart0'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files in application code.
 *
 */
lpuart_state_t lpUartState0;

const lpuart_user_config_t lpuart_0_InitConfig0 = {
  .transferType = LPUART_USING_INTERRUPTS,
  .baudRate = 9600UL,
  .parityMode = LPUART_PARITY_DISABLED,
  .stopBitCount = LPUART_ONE_STOP_BIT,
  .bitCountPerChar = LPUART_8_BITS_PER_CHAR,
  .rxDMAChannel = 0UL,
  .txDMAChannel = 0UL
};

