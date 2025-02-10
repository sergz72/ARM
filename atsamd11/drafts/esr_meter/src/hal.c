#include "board.h"
#include <i2c_soft.h>
#include <systick.h>

static unsigned char *txbuf_p;
static volatile int Sys_Tick;

void __attribute__((used)) SERCOM0_Handler(void)
{
    unsigned char flags = SERCOM0_REGS->I2CS.SERCOM_INTFLAG;

    if (flags & SERCOM_I2CS_INTFLAG_AMATCH_Msk)
        // clear amatch flag
        SERCOM0_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_AMATCH_Msk;
    if (flags & SERCOM_I2CS_INTFLAG_DRDY_Msk)
    {
        if (SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) // read
            SERCOM0_REGS->I2CS.SERCOM_DATA = *txbuf_p++;
        else // write
        {
            unsigned int command = SERCOM0_REGS->I2CS.SERCOM_DATA;
            txbuf_p = (unsigned char*)txbufs[command & 1];
        }
        SERCOM0_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(3);
    }
    if (flags & SERCOM_I2CS_INTFLAG_PREC_Msk) // stop
    {
        INTERRUPT_FLAG_CLR;
        interrupt_request = 0;
        SERCOM0_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_PREC_Msk;
    }
}

void __attribute__((used)) SysTick_Handler(void)
{
    Sys_Tick++;
}

static void systick_start(unsigned int us)
{
    systick_set_reload(us * SYSTICK_MULTIPLIER);

    systick_interrupt_enable();
    /* start counting */
    systick_counter_enable();
}

static void systick_stop(void)
{
    /* stop counting */
    systick_counter_disable();
    systick_interrupt_disable();
}

static void systick_wait(void)
{
    Sys_Tick = 0;
    while (!Sys_Tick)
        __WFI();
}

void delay(unsigned int us)
{
    systick_start(us);
    systick_wait();
    systick_stop();
}

void delayms(unsigned int ms)
{
    while (ms--)
        delay(1000);
}

/*
 *PA14(4) -> PAD0 - SDA
 *PA15(5) -> PAD1 - SCL
 */

static void i2c_slave_init(void)
{
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA14 | PORT_PA15;
    PORT_REGS->GROUP[0].PORT_PINCFG[14] = 5; // PMUXEN | PULLEN
    PORT_REGS->GROUP[0].PORT_PINCFG[15] = 5; // PMUXEN | PULLEN
    PORT_REGS->GROUP[0].PORT_PMUX[7] |= 0x22; // SERCOM

    SERCOM0_REGS->I2CS.SERCOM_CTRLB = SERCOM_I2CS_CTRLB_AACKEN_Msk;
    /* Wait for synchronization */
    while((SERCOM0_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }
    /* Set Operation Mode to I2C Slave */
    SERCOM0_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE | SERCOM_I2CS_CTRLA_SDAHOLD_75NS |
        SERCOM_I2CS_CTRLA_SCLSM(1) | SERCOM_I2CS_CTRLA_SPEED_STANDARD_AND_FAST_MODE;
    /* Wait for synchronization */
    while((SERCOM0_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }

    /* Set the slave address */
    SERCOM0_REGS->I2CS.SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(I2C_ADDRESS);

    /* Enable all I2C slave Interrupts */
    SERCOM0_REGS->I2CS.SERCOM_INTENSET = SERCOM_I2CS_INTENSET_Msk;

    /* Enable peripheral */
    SERCOM0_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk ;

    NVIC_SetPriority(SERCOM0_IRQn, 0);
    NVIC_EnableIRQ(SERCOM0_IRQn);
}

/*
 *PA04(14) -> SDA
 *PA02(13) -> SCL
 */

static void i2c_master_init(void)
{
    i2c_soft_init(0);
    PORT_REGS->GROUP[0].PORT_PINCFG[4] = 3; // INEN | PULLEN
    PORT_REGS->GROUP[0].PORT_PINCFG[2] = 3; // INEN | PULLEN
}

/*
 *PA05(1) = TC1/WO[1]
 */

static void pwm_init(void)
{
    PORT_REGS->GROUP[0].PORT_PINCFG[5] = 1; // PMUXEN
    PORT_REGS->GROUP[0].PORT_PMUX[2] = 0x40; // TC/TCC

    /* Configure counter mode & prescaler */
    TC1_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_WAVEGEN_MPWM ;


    TC1_REGS->COUNT16.TC_CC[0] = 480; // 100 khz
    TC1_REGS->COUNT16.TC_CC[1] = 48; //10% duty

    while((TC1_REGS->COUNT16.TC_STATUS & TC_STATUS_SYNCBUSY_Msk)!= 0U)
    {
        /* Wait for Write Synchronization */
    }

    TC1_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
}

/*
 *PA08(2) = button
 */

static void button_init(void)
{
    // in with pullup
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA08;
    PORT_REGS->GROUP[0].PORT_PINCFG[8] = 6; // INEN | PULLEN
}

static void gclk0_init(void)
{
    // source=DFLL48M output
    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_SRC(7) | GCLK_GENCTRL_GENEN_Msk | GCLK_GENCTRL_ID(0);
    while((GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk) == GCLK_STATUS_SYNCBUSY_Msk)
    {
        /* wait for the Generator 0 synchronization */
    }

    /* Selection of the Generator and write Lock for SERCOM0_CORE */
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_SERCOM0_CORE | GCLK_CLKCTRL_GEN(0x0) | GCLK_CLKCTRL_CLKEN_Msk;
    /* Selection of the Generator and write Lock for EIC */
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_GEN(0x0) | GCLK_CLKCTRL_CLKEN_Msk;
    /* Selection of the Generator and write Lock for TC1 */
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_TC1_TC2 | GCLK_CLKCTRL_GEN(0x0) | GCLK_CLKCTRL_CLKEN_Msk;
}

static void clock_init(void)
{
    SYSCTRL_REGS->SYSCTRL_XOSC = 0;
    SYSCTRL_REGS->SYSCTRL_XOSC32K = 0;
    SYSCTRL_REGS->SYSCTRL_OSC32K = 0;

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 2; // Enable DFLL in open loop mode
    while((SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk) != SYSCTRL_PCLKSR_DFLLRDY_Msk)
    {
        /* Waiting for the Ready state */
    }

    gclk0_init();

    // enable tc1, sercom0 clocks
    PM_REGS->PM_APBCMASK = 0x44;

    SYSCTRL_REGS->SYSCTRL_OSC8M = 0;

    systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);
}

/*
 * PA24(9)  = INTERRUPT
 * PA25(10) = LED
 */

static void ports_init(void)
{
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA24;
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA25;
}

void SysInit(void)
{
    // 1 wait state - 2.7V to 3.63V up to 67 MHz clock
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(1);
    clock_init();
    ports_init();
    i2c_slave_init();
    i2c_master_init();
    button_init();
    pwm_init();
}

void _init(void)
{
}
