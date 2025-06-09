#ifndef AT32F403A_407_CONF_H
#define AT32F403A_407_CONF_H

#define HEXT_STARTUP_TIMEOUT             ((uint16_t)0x3000)  /*!< time out for hext start up */
#define HICK_VALUE                       ((uint32_t)8000000) /*!< value of the high speed internal clock in hz */
#define HEXT_VALUE                       ((uint32_t)8000000) /*!< value of the high speed external crystal in hz */

#define CRM_MODULE_ENABLED
#define GPIO_MODULE_ENABLED
#define I2C_MODULE_ENABLED
#define SPI_MODULE_ENABLED
#define DMA_MODULE_ENABLED
#define MISC_MODULE_ENABLED
#define USB_MODULE_ENABLED
#include "at32f403a_407_gpio.h"
#include "at32f403a_407_crm.h"
#include "at32f403a_407_i2c.h"
#include "at32f403a_407_spi.h"
#include "at32f403a_407_dma.h"
#include "at32f403a_407_misc.h"
#include "at32f403a_407_usb.h"

#endif