# STM32F407 Bare-Metal Programming

Register-level embedded C on the **STM32F407G-DISC1** (ARM Cortex-M4) — no HAL, no libraries.
Every peripheral is configured by writing directly to memory-mapped registers, with addresses
taken from the STM32F4xx Reference Manual (RM0090) and the board schematic.

> Written by **Shalev Turgeman** while working through the FastBit Embedded Brain Academy
> "Microcontroller Embedded C Programming" course. All implementations are my own,
> including extensions beyond the course exercises.

## Projects

| # | Project | What it demonstrates |
|---|---------|----------------------|
| 01 | `01_led_on` | GPIO fundamentals: enabling the GPIOD peripheral clock via `RCC_AHB1ENR`, configuring PD12 as output in `MODER`, driving the LED through `ODR` — three pointer dereferences and the LED is on |
| 02 | `02_led_toggle` | Bit set/clear masking (`|=`, `&= ~`) and a software delay loop; the classic blink, done with raw registers |
| 03 | `03_pin_read` | Reading input state: PA0 (user button) via the `IDR` register with bit masking, driving PD12 accordingly |
| 04 | `04_button_isr` | **Interrupt-driven design**: EXTI0 line configuration (`SYSCFG_EXTICR`, edge trigger, interrupt mask), NVIC enable, a real ISR (`EXTI0_IRQHandler`), a `volatile` flag shared between ISR and main loop, software debounce, and EXTI masking during event handling. Extended beyond the course exercise: each press advances a rotating pattern across all four board LEDs (PD12–PD15), plus a custom `__io_putchar` implementation for `printf` over ITM/SWO |
| 05 | `05_sizeof_semihosting` | Cross-compiler data-type sizes printed from the target — the practical side of why `<stdint.h>` fixed-width types matter in portable embedded code |

## Key concepts covered

- **Memory-mapped I/O** — peripheral registers as absolute addresses (`(uint32_t*)0x40020C00`), calculated as peripheral base + offset from RM0090
- **Clock gating** — nothing works until the peripheral clock is enabled in `RCC_AHB1ENR` / `APB2ENR`
- **Bit manipulation** — masking, setting, clearing and toggling with `&`, `|`, `^`, `~`, `<<`
- **`volatile` correctness** — hardware registers and ISR-shared variables declared `volatile` so the compiler never optimizes away real reads/writes
- **Interrupts on Cortex-M4** — the full chain: GPIO → SYSCFG → EXTI → NVIC → ISR
- **Debugging without a UART** — `printf` routed through the ITM/SWO trace unit over the ST-LINK's SWD interface

## Building & running

1. Open **STM32CubeIDE** → `File → Import → Existing Projects into Workspace` → select a project folder
2. Build (the included `STM32F407VGTX_FLASH.ld` linker script is used as-is)
3. Flash & debug over the on-board ST-LINK/V2; for projects using `printf`, open the SWV ITM Data Console (core clock 16 MHz, port 0)

## Roadmap

- [ ] Matrix keypad interfacing (row scanning, internal pull-ups via `PUPDR`)
- [ ] LED toggle rewritten with peripheral register `struct`s + bit-fields
- [ ] UART driver from scratch

## Hardware

- STM32F407G-DISC1 discovery board (STM32F407VG — Cortex-M4 @ 168 MHz, 1 MB flash, 192 KB SRAM)
- References: RM0090 reference manual, UM1472 board user manual, MB997 schematic
