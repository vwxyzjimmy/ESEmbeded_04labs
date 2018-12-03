/**
 * 
 * STM32F4DISCOVERY
 * 
 * blink blue LED (PD15)
 * 
 */

#include <stdint.h>

#define SET_BIT(addr, bit) *((volatile uint32_t *)(addr)) |= (uint32_t)1 << bit
#define CLEAR_BIT(addr, bit) *((volatile uint32_t *)(addr)) &= ~((uint32_t)1 << bit)

//RCC
#define RCC_BASE 0x40023800

#define RCC_AHB1ENR_OFFSET 0x30
#define GPIODEN_BIT 3

//GPIO
#define GPIOD_BASE 0x40020C00

#define GPIOx_MODER_OFFSET 0x00
#define MODER15_1_BIT 31
#define MODER15_0_BIT 30

#define GPIOx_OTYPER_OFFSET 0x04
#define OT15_BIT 15

#define GPIOx_OSPEEDR_OFFSET 0x08
#define OSPEEDR15_1_BIT 31
#define OSPEEDR15_0_BIT 30

#define GPIOx_PUPDR_OFFSET 0x0C
#define PUPDR15_1_BIT 31
#define PUPDR15_0_BIT 30

#define GPIOx_BSRR_OFFSET 0x18
#define BR15_BIT 31
#define BS15_BIT 15

void blink(void)
{
	SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIODEN_BIT);

	//MODER15 = 01 => General purpose output mode
	CLEAR_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER15_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER15_0_BIT);

	//OT15 = 0 => Output push-pull
	CLEAR_BIT(GPIOD_BASE + GPIOx_OTYPER_OFFSET, OT15_BIT);

	//OSPEEDR15 = 00 => Low speed
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR15_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR15_0_BIT);

	//PUPDR15 = 00 => No pull-up, pull-down
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR15_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR15_0_BIT);

	unsigned int i;
	while (1)
	{
		//reset GPIOD15
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR15_BIT);

		for (i = 0; i < 100000; i++)
			;

		//set GPIOD15
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BS15_BIT);

		for (i = 0; i < 100000; i++)
			;
	}
}