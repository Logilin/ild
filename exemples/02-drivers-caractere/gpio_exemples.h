#ifndef _GPIO_EXEMPLES_H_
#define _GPIO_EXEMPLES_H_

#define RASPBERRY_PI         1
#define BEAGLEBONE_BLACK     2
#define TERASIC_DE1_SOC      3


// Uncomment one of the next lines depending on you plateform.
//#define GPIO_MODE  RASPBERRY_PI
#define GPIO_MODE    BEAGLEBONE_BLACK
//#define GPIO_MODE  TERASIC_DE1_SOC


#ifndef GPIO_MODE
	#error "Please select your platform in gpio_exemples.h"
#else

	#if GPIO_MODE == RASPBERRY_PI
		// Input on pin 16 (GPIO 23)
		#define GPIO_IN  23
		// Output on pin 18 (GPIO 24)
		#define GPIO_OUT 24
	#elif GPIO_MODE == BEAGLEBONE_BLACK
		// Input on pin P8-08 (GPIO 67)
		#define GPIO_IN  67
		// Output on LED 4 (GPIO 56)
		#define GPIO_OUT 56
	#elif GPIO_MODE == TERASIC_DE1_SOC
		// Input on the push button
		#define GPIO_IN  223
		// Output on the led.
		#define GPIO_OUT 222
	#else
		#error "Unknown platform definition in gpioo_exemples.h"
	#endif
#endif

#endif
