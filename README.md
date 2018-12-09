LAB4
===
## 本周作業
開機後無窮等待，等按鈕按下後，用上課使用過以外的 LED 燈不斷閃爍。
### 實驗過程:
詳情請詳閱操作手冊

#### Step1:
找出按鈕以及 LED 的腳位  [Discovery kit with STM32F407VG MCU
](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F4DISCOVERY.pdf)目錄中的 **6-3 LEDs**、**6-4 Push buttons**(p.16)，LED 四個燈於 PD12~15， Push button 中 User and Wake-Up buttons 於 PA0。

#### Step2:
知道腳位後，必須知道在記憶體中對應的 address ，在[STM32F405 Reference manual](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf)目錄中找 **Memory map** (p.64)，找出 GPIOA 及 GPIOD 的起始位置分別為 `0x4002 0000` 以及 `0x4002 0C00` 以及其對應的匯流排 AHB1，這意味著以下有關各自 GPIO 的設定都是以此為基準，各自 GPIO 做設定的 register 的 address 在根據各手冊提供的 offset 加上起始位置決定，有了 register 在 memory 的 address 後，在對其設定來達到我們要的功能。

#### Step3:
在使用 GPIO 前，必須啟用 RCC (Reset and clock control) 中對應的 bit，RCC 的起始位置如同 Step2 一樣找 [STM32F405 Reference manual](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf) 的 **Memory map** (p.65) 位於 `0x4002 3800`。
同樣由 Step2 中 **Memory map** 的圖可知 GPIOA 及 GPIOD 走 AHB1 匯流排，因此在 [STM32F405 Reference manual](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf)目錄中找 **Reset and clock control** 的裡面找 GPIOA 及 GPIOD 使用的匯流排 AHB1 於RCC 的 offset，**AHB1 peripheral clock enable register (RCC_AHB1ENR)** (p.180)，其 register 的 offset 為 `0x30`，所以此 register 位置是 `0x4002 3800` + `0x30`，要啟用 GPIOA 及 GPIOD 則是要設定此 register 的 0th 及 3th bit。

#### Step4:
設定各 GPIO 運作的方式，[STM32F405 Reference manual](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf)目錄中找 **GPIO functional description** (p.267)，可以知道 GPIO 功能要如何設置對應的 register，依序設置 MODER、OTYPER、OSPEEDR、PUPDR 四個 register 。
本次使用到 GPIOA 及 GPIOD ，因此兩邊的 register 皆要設置， GPIOA 及 GPIOD 的起始位置分別為 `0x4002 0000` 以及 `0x4002 0C00` 。
四個 register 的內容於 [STM32F405 Reference manual](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf) 目錄中找 **GPIO registers** (p281)。
* MODER (GPIO port mode register)
MODER 的 offset 為 `0x00` ，我們使用到 PD15~12 為 output ， PA0 為 input，根據 GPIO functional description ， output 設置對應的 bit 為 0,1 ，input 設置對應的 bit 為 0,0，
設置 PD15~12 GPIO 的功能，於 `0x4002 0C00` + `0x00` 起始位置加上 offset 的 register 依 PD15~PD12 照 GPIO registers 的圖示，從 31th bit ~ 24th bit 依序設置 `01010101`。
設置 PA0 GPIO 的功能，於 `0x4002 0000` + `0x00` 起始位置加上 offset 的 register 依 PA0 依據 GPIO registers 圖示，從 1th bit ~ 0th bit 設置 01。
* OTYPER (GPIO port output type register)
OTYPER 的 offset 為 `0x04` ， PD15~12 必須使用 PUSH PULL 才不須外接電源，因此照 GPIO registers 皆設定為 0，於 `0x4002 0C00` + `0x04` 起始位置加上 offset 的 register ，從 15h bit ~ 12th bit 依序設置 `0000`。
PA0 是 input 根據 GPIO functional description 不需設置。
* OSPEEDR (GPIO port output speed register)
OSPEEDR 的 offset 為 `0x08`，用來設置 GPIO 輸出的更新速率，根據 GPIO registers 表示有四種速度，本實驗設置哪種都行，於 `0x4002 0C00` + `0x08` 起始位置加上 offset 的 register ，從 31th bit ~ 24th bit 依序設置 `00000000`。
PA0 是 input 根據 GPIO functional description 不需設置。
* PUPDR (GPIO port pull-up/pull-down register)
PUPDR 的 offset 為 `0x0C`，用來決定其基準電位， PD15~12 在本實驗設置成 No pull-up、 pull-down ，因此皆設定為 0，於 `0x4002 0C00` + `0x0C` 起始位置加上 offset 的 register ，從 31th bit ~ 24th bit 依序設置 `00000000`。
PA0 為 input 根據程式需求在決定基準電位，本實驗設置成 No pull-up、 pull-down，於 `0x4002 0000` + `0x0C` 起始位置加上 offset 的 register ，從 1th bit ~ 0th bit 依序設置 `00`。

#### Step5:
對於 output 及 input 的使用，同樣也是對 memory 中對應的 register 做讀寫來操作，完成以上設定後，對 GPIO 的操作由讀寫 IDR、ODR、BSRR 完成。
* IDR (GPIO port input data register)
IDR 的 offset 為 `0x10`，若 GPIO MODER 設置成 input，則 input 的藉由對應的 port 讀取此 register 對應的 bit 來獲得，像本實驗使用到的 PA0，讀取於 `0x4002 0000` + `0x10` 起始位置加上 offset 的 register，利用 bit operation 讀取 0th 的 bit 來獲得 input。
* ODR (GPIO port output data register)
ODR 的 offset 為 `0x14`，若 GPIO MODER 設置成 output，則 output 藉由寫入此 register 對應的 bit 來控制對應的 port 輸出，本實驗使用到的 PD15~12，則可以寫入於 `0x4002 0C00` + `0x14` 起始位置加上 offset 的 register，利用 bit operation 寫入 15th~12th bit 來控制輸出，另外也可以藉由讀取此 register 來獲取 port 輸出的狀態。

* BSRR (GPIO port bit set/reset register)
BSRR 的 offset 為 `0x18`，用來對 output 做寫入動作，共 32 bit，31th~16th bit 設 1 為對 GPIO 15th~0th port 輸出高電位， 15th~0th bit 設 1 為對 GPIO 15th~0th port 設低電位， 對此 register 設 0 沒作用，只可寫入，讀取沒有意義。

### 程式碼
```c
/**
 * 
 * STM32F4DISCOVERY
 * 
 * blink blue LED (PD15)
 * 
 */

#include <stdint.h>

#define SET_BIT(addr, bit) *((uint32_t *)(addr)) |= ((uint32_t)1 << bit)
#define CLEAR_BIT(addr, bit) *((uint32_t *)(addr)) &= ~((uint32_t)1 << bit)
#define READ_BIT(addr,bit) (((uint32_t)1 == (*(uint32_t *)(addr) & (uint32_t)1 << bit))?1:0)
//#define READ_BIT(addr,bit) *(volatile uint32_t *)(addr) &(uint32_t)1 << bit

//RCC
#define RCC_BASE 0x40023800

#define RCC_AHB1ENR_OFFSET 0x30
#define GPIODEN_BIT 3
#define GPIOAEN_BIT 0

//GPIO
#define GPIOx_MODER_OFFSET 0x00
#define GPIOx_OTYPER_OFFSET 0x04
#define GPIOx_OSPEEDR_OFFSET 0x08
#define GPIOx_PUPDR_OFFSET 0x0C
#define GPIOx_BSRR_OFFSET 0x18

//GPIO D
#define GPIOD_BASE 0x40020C00

#define MODER15_1_BIT 31
#define MODER15_0_BIT 30
#define MODER14_1_BIT 29
#define MODER14_0_BIT 28
#define MODER13_1_BIT 27
#define MODER13_0_BIT 26
#define MODER12_1_BIT 25
#define MODER12_0_BIT 24

#define OT15_BIT 15
#define OT14_BIT 14
#define OT13_BIT 13
#define OT12_BIT 12

#define OSPEEDR15_1_BIT 31
#define OSPEEDR15_0_BIT 30
#define OSPEEDR14_1_BIT 29
#define OSPEEDR14_0_BIT 28
#define OSPEEDR13_1_BIT 27
#define OSPEEDR13_0_BIT 26
#define OSPEEDR12_1_BIT 25
#define OSPEEDR12_0_BIT 24

#define PUPDR15_1_BIT 31
#define PUPDR15_0_BIT 30
#define PUPDR14_1_BIT 29
#define PUPDR14_0_BIT 28
#define PUPDR13_1_BIT 27
#define PUPDR13_0_BIT 26
#define PUPDR12_1_BIT 25
#define PUPDR12_0_BIT 24

#define BR15_BIT 31
#define BS15_BIT 15
#define BR14_BIT 30
#define BS14_BIT 14
#define BR13_BIT 29
#define BS13_BIT 13
#define BR12_BIT 28
#define BS12_BIT 12

//GPIO_A
#define GPIOA_BASE 0x40020000

#define MODER0_1_BIT 0
#define MODER0_0_BIT 1

#define PUPDR0_1_BIT 1
#define PUPDR0_0_BIT 0

#define BR0_BIT 16
#define BS0_BIT 0

#define GPIOx_IDR_OFFSET 0x10
#define IDR_0_BIT 0


void blink(void)
{
	SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIODEN_BIT);
	SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIOAEN_BIT);

	//MODER15 = 01 => General purpose output mode
	CLEAR_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER15_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER15_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER14_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER14_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER13_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER13_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER12_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_MODER_OFFSET, MODER12_0_BIT);
	
	//MODER0 = 00 => General purpose input mode
	CLEAR_BIT(GPIOA_BASE + GPIOx_MODER_OFFSET, MODER0_1_BIT);
	CLEAR_BIT(GPIOA_BASE + GPIOx_MODER_OFFSET, MODER0_0_BIT);


	//OT15 = 0 => Output push-pull
	CLEAR_BIT(GPIOD_BASE + GPIOx_OTYPER_OFFSET, OT15_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OTYPER_OFFSET, OT14_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OTYPER_OFFSET, OT13_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OTYPER_OFFSET, OT12_BIT);


	//OSPEEDR15 = 00 => Low speed
	SET_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR15_1_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR15_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR14_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR14_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR13_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR13_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR12_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_OSPEEDR_OFFSET, OSPEEDR12_0_BIT);


	//PUPDR15 = 00 => No pull-up, pull-down
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR15_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR15_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR14_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR14_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR13_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR13_0_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR12_1_BIT);
	CLEAR_BIT(GPIOD_BASE + GPIOx_PUPDR_OFFSET, PUPDR12_0_BIT);
	//PUPDR0 = 00 => No pull-up, pull-down
	CLEAR_BIT(GPIOA_BASE + GPIOx_PUPDR_OFFSET, PUPDR0_1_BIT);
	CLEAR_BIT(GPIOA_BASE + GPIOx_PUPDR_OFFSET, PUPDR0_0_BIT);
	
	SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR15_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR14_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR13_BIT);
	SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR12_BIT);
	
	unsigned int choose[] = {BS15_BIT, BS14_BIT, BS13_BIT, BS12_BIT};
	unsigned int i, light_count = 0;
	while(!READ_BIT(GPIOA_BASE + GPIOx_IDR_OFFSET, IDR_0_BIT));
	
	while (1)
	{
		if( READ_BIT(GPIOA_BASE + GPIOx_IDR_OFFSET, IDR_0_BIT)){
			while(READ_BIT(GPIOA_BASE + GPIOx_IDR_OFFSET, IDR_0_BIT));
			light_count++;
			if (light_count > 3)
				light_count = 0;
			for (i = 0; i < 10000; i++)
				;
		}

		//reset GPIOD15
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR15_BIT);
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR14_BIT);
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR13_BIT);
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, BR12_BIT);
		for (i = 0; i < 100000; i++)
			;
		SET_BIT(GPIOD_BASE + GPIOx_BSRR_OFFSET, choose[light_count]);
	
		for (i = 0; i < 100000; i++)
			;
	}
}
```

