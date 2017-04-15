/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#ifndef _GPIO_EXEMPLES_H_
#define _GPIO_EXEMPLES_H_

#define EXEMPLE_RASPBERRY_PI       1
#define EXEMPLE_BEAGLEBONE_BLACK   2
#define EXEMPLE_TERASIC_DE1_SOC    3


// Uncomment one of the following lines depending on you plateform:
  // #define EXEMPLE_GPIO_MODE  EXEMPLE_RASPBERRY_PI
     #define EXEMPLE_GPIO_MODE        EXEMPLE_BEAGLEBONE_BLACK
  // #define EXEMPLE_GPIO_MODE        EXEMPLE_TERASIC_DE1_SOC


#ifndef EXEMPLE_GPIO_MODE
	#error "Please select your platform in gpio_exemples.h"
#else

	#if EXEMPLE_GPIO_MODE == EXEMPLE_RASPBERRY_PI
		// Input on pin 16 (GPIO 23)
		#define EXEMPLE_GPIO_IN  23
		// Output on pin 18 (GPIO 24)
		#define EXEMPLE_GPIO_OUT 24
	#elif EXEMPLE_GPIO_MODE == EXEMPLE_BEAGLEBONE_BLACK
		// Input on pin P8-07 (GPIO 66)
		#define EXEMPLE_GPIO_IN  66
		// Output on LED 4 (GPIO 56)
		#define EXEMPLE_GPIO_OUT 56
	#elif EXEMPLE_GPIO_MODE == EXEMPLE_TERASIC_DE1_SOC
		// Input on the push button
		#define EXEMPLE_GPIO_IN  223
		// Output on the led.
		#define EXEMPLE_GPIO_OUT 222
	#else
		#error "Unknown platform definition in gpioo_exemples.h"
	#endif
#endif

#endif

