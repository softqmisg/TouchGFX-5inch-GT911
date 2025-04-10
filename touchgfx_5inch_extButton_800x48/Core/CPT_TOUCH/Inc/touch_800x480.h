#ifndef __TOUCH_H
#define __TOUCH_H

#include "stm32h7xx_hal.h"
#include "touch_iic.h"  


/*------------------------------------ related definition -----------------------------------*/  	

#define TOUCH_MAX   5	//Maximum number of touch points

typedef struct 
{
	uint8_t  flag;			//	Touch flag, when it is 1, it means there is a touch operation
	uint8_t  num;				//	touch points
	uint16_t x[TOUCH_MAX];	//	x-coordinate
	uint16_t y[TOUCH_MAX];	//	y-coordinate
}TouchStructure;

extern volatile TouchStructure touchInfo;	// Touch data structure declaration	

/*------------------------------------ deposit definition -----------------------------------*/  		

#define GT9XX_IIC_RADDR 0xBB			// IIC initialization address
#define GT9XX_IIC_WADDR 0xBA

#define GT9XX_CFG_ADDR 	0x8047		// Firmware configuration information register and configuration start address
#define GT9XX_READ_ADDR 0x814E		// Touch Information Register
#define GT9XX_ID_ADDR 	0x8140		// Touch panel ID register

/*------------------------------------ function declaration -----------------------------------*/  		

uint8_t 	Touch_Init(void);		// touch screen initialization
void 		Touch_Scan(void);		// touch scan
void  	GT9XX_Reset(void);	// Perform a reset operation


#endif

