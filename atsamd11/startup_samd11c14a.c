/**
 * \file
 *
 * \brief GCC startup file for ATSAMD11C14A
 *
 * Copyright (c) 2022 Microchip Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

#include "samd11c14a.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/* Optional application-provided functions */
extern void __attribute__((weak,long_call)) _on_reset(void);
extern void __attribute__((weak,long_call)) _on_bootstrap(void);

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Reset handler */
void Reset_Handler(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M0PLUS core handlers */
void NonMaskableInt_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVCall_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void PM_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SYSCTRL_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USB_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EVSYS_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM0_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM1_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AC_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DAC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PTC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack = (void*) (&_estack),

        .pfnReset_Handler              = (void*) Reset_Handler,
        .pfnNonMaskableInt_Handler     = (void*) NonMaskableInt_Handler,
        .pfnHardFault_Handler          = (void*) HardFault_Handler,
        .pvReservedC12                 = (void*) (0UL), /* Reserved */
        .pvReservedC11                 = (void*) (0UL), /* Reserved */
        .pvReservedC10                 = (void*) (0UL), /* Reserved */
        .pvReservedC9                  = (void*) (0UL), /* Reserved */
        .pvReservedC8                  = (void*) (0UL), /* Reserved */
        .pvReservedC7                  = (void*) (0UL), /* Reserved */
        .pvReservedC6                  = (void*) (0UL), /* Reserved */
        .pfnSVCall_Handler             = (void*) SVCall_Handler,
        .pvReservedC4                  = (void*) (0UL), /* Reserved */
        .pvReservedC3                  = (void*) (0UL), /* Reserved */
        .pfnPendSV_Handler             = (void*) PendSV_Handler,
        .pfnSysTick_Handler            = (void*) SysTick_Handler,

        /* Configurable interrupts */
        .pfnPM_Handler                 = (void*) PM_Handler,     /* 0  Power Manager */
        .pfnSYSCTRL_Handler            = (void*) SYSCTRL_Handler, /* 1  System Control */
        .pfnWDT_Handler                = (void*) WDT_Handler,    /* 2  Watchdog Timer */
        .pfnRTC_Handler                = (void*) RTC_Handler,    /* 3  Real-Time Counter */
        .pfnEIC_Handler                = (void*) EIC_Handler,    /* 4  External Interrupt Controller */
        .pfnNVMCTRL_Handler            = (void*) NVMCTRL_Handler, /* 5  Non-Volatile Memory Controller */
        .pfnDMAC_Handler               = (void*) DMAC_Handler,   /* 6  Direct Memory Access Controller */
        .pfnUSB_Handler                = (void*) USB_Handler,    /* 7  Universal Serial Bus */
        .pfnEVSYS_Handler              = (void*) EVSYS_Handler,  /* 8  Event System Interface */
        .pfnSERCOM0_Handler            = (void*) SERCOM0_Handler, /* 9  Serial Communication Interface */
        .pfnSERCOM1_Handler            = (void*) SERCOM1_Handler, /* 10 Serial Communication Interface */
        .pvReserved11                  = (void*) (0UL),          /* 11 Reserved */
        .pfnTCC0_Handler               = (void*) TCC0_Handler,   /* 12 Timer Counter Control */
        .pfnTC1_Handler                = (void*) TC1_Handler,    /* 13 Basic Timer Counter */
        .pfnTC2_Handler                = (void*) TC2_Handler,    /* 14 Basic Timer Counter */
        .pfnADC_Handler                = (void*) ADC_Handler,    /* 15 Analog Digital Converter */
        .pfnAC_Handler                 = (void*) AC_Handler,     /* 16 Analog Comparators */
        .pfnDAC_Handler                = (void*) DAC_Handler,    /* 17 Digital Analog Converter */
        .pfnPTC_Handler                = (void*) PTC_Handler     /* 18 Peripheral Touch Controller */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Call the optional application-provided _on_reset() function. */
        if (_on_reset) {
                _on_reset();
        }

        /* Initialize the C library */
        __libc_init_array();

        /* Call the optional application-provided _on_bootstrap() function. */
        if (_on_bootstrap) {
                _on_bootstrap();
        }

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
