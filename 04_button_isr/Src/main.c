#include <stdint.h>
#include <stdio.h>

int __io_putchar(int ch)
{
    volatile uint32_t *ITM_STIM0 = (uint32_t*)0xE0000000;
    volatile uint32_t *ITM_TER   = (uint32_t*)0xE0000E00;
    volatile uint32_t *ITM_TCR   = (uint32_t*)0xE0000E80;

    if (!(*ITM_TCR & 1)) return ch;
    if (!(*ITM_TER & 1)) return ch;

    while (!(*ITM_STIM0 & 1));
    *((volatile uint8_t*)ITM_STIM0) = (uint8_t)ch;

    return ch;
}

// === registers ===
uint32_t volatile *pEXTTIPendReg     = (uint32_t*)(0x40013C00 + 0x14);
uint32_t volatile *pClkCtrlReg       = (uint32_t*)(0x40023800 + 0x30);
uint32_t volatile *pClkCtrlRegApb2   = (uint32_t*)(0x40023800 + 0x44);
uint32_t volatile *pGPIOAModeReg     = (uint32_t*)(0x40020000 + 0x00);
uint32_t volatile *pGPIODModeReg     = (uint32_t*)(0x40020C00 + 0x00);
uint32_t volatile *pGPIODOutReg      = (uint32_t*)(0x40020C00 + 0x14);
uint32_t volatile *pEXTIMaskReg      = (uint32_t*)(0x40013C00 + 0x00);
uint32_t volatile *pEXTTIEdgeCtrlReg = (uint32_t*)(0x40013C00 + 0x08);
uint32_t volatile *pNVICIRQEnReg     = (uint32_t*)0xE000E100;
uint32_t volatile *pSYSCFGEXTICR1   = (uint32_t*)(0x40013800 + 0x08);

// === global val ===
uint8_t  volatile g_button_pressed    = 0;
uint32_t          g_button_press_count = 0;

void button_init(void);
void led_init(void);

int main(void)
{
    led_init();
    button_init();

    while(1)
    {
        *pEXTIMaskReg &= ~(1 << 0);

        if(g_button_pressed)
        {
            // debounce
            for(uint32_t volatile i = 0; i < 500000/2; i++);

            g_button_press_count++;

            // every press is the next LED
            // turn off the LED
            *pGPIODOutReg &= ~(0xF << 12);

            // turn ON by the num pf press
            uint32_t led_pin = 12 + ((g_button_press_count - 1) % 4); // PD12, PD13, PD14, PD15
            *pGPIODOutReg |= (1 << led_pin);

            g_button_pressed = 0;
        }

        *pEXTIMaskReg |= (1 << 0);
    }
}

void led_init(void)
{
    // clock GPIOD
    *pClkCtrlReg |= (1 << 3);

    // PD12, PD13, PD14, PD15 as-output
    *pGPIODModeReg &= ~(0xFF << 24);
    *pGPIODModeReg |=  (0x55 << 24); // 01 01 01 01 = output every 4 pin
}

void button_init(void)
{
    *pClkCtrlReg     |= (1 << 0);
    *pClkCtrlRegApb2 |= (1 << 14);

    *pGPIOAModeReg   &= ~(3 << 0);
    *pSYSCFGEXTICR1  &= ~(0xF << 0);

    *pEXTTIEdgeCtrlReg |= (1 << 0);
    *pEXTIMaskReg      |= (1 << 0);
    *pNVICIRQEnReg     |= (1 << 6);
}

void EXTI0_IRQHandler(void)
{
    g_button_pressed = 1;
    *pEXTTIPendReg |= (1 << 0);
}
