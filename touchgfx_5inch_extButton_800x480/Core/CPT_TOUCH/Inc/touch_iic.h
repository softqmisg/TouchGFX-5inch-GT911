#ifndef __IIC_H
#define __IIC_H

#include "stm32h7xx_hal.h"

/*----------------------------------------- IIIC Pin Configuration Macros -----------------------------------------------*/
 

#define Touch_IIC_SCL_CLK_ENABLE     __HAL_RCC_GPIOH_CLK_ENABLE()		// SCL pin clock
#define Touch_IIC_SCL_PORT   			 GPIOH                 				// SCL pin port
#define Touch_IIC_SCL_PIN     		 GPIO_PIN_6  							// SCL pin
         
#define Touch_IIC_SDA_CLK_ENABLE     __HAL_RCC_GPIOI_CLK_ENABLE() 	// SDA pin clock
#define Touch_IIC_SDA_PORT   			 GPIOI                   			// SDA pin port
#define Touch_IIC_SDA_PIN    			 GPIO_PIN_3             			// SDA pin

#define Touch_INT_CLK_ENABLE    		 __HAL_RCC_GPIOH_CLK_ENABLE()		// INT pin clock
#define Touch_INT_PORT   				 GPIOH                   			// INT pin port
#define Touch_INT_PIN    				 GPIO_PIN_7             			// INT pin

#define Touch_RST_CLK_ENABLE   		 __HAL_RCC_GPIOI_CLK_ENABLE()		// RST pin clock
#define Touch_RST_PORT   				 GPIOI                   			// RST pin port
#define Touch_RST_PIN    				 GPIO_PIN_8           				// RST pin

/*------------------------------------------ IIC related definitions -------------------------------------------------*/

#define ACK_OK  	1  			// normal response
#define ACK_ERR 	0				// response error

// IIC communication delay, the Touch_IIC_Delay() function is used,
// The communication speed is around 100KHz
#define IIC_DelayVaule  20  	

/*-------------------------------------------- IO port operation ---------------------------------------------------*/   

#define Touch_IIC_SCL(a)	if (a)	\
										HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_RESET)	

#define Touch_IIC_SDA(a)	if (a)	\
										HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_RESET)		

/*--------------------------------------------- function declaration --------------------------------------------------*/  		
					
void 		Touch_IIC_GPIO_Config (void);					// IIC pin initialization
void 		Touch_IIC_Delay(uint32_t a);					// IIC delay function
void		Touch_INT_Out(void);								// INT data pin configured as output mode
void		Touch_INT_In(void);								// The INT data pin is configured as input mode								
void 		Touch_IIC_Start(void);							// Start IIC communication
void 		Touch_IIC_Stop(void);							// IIC stop signal
void 		Touch_IIC_ACK(void);								//	send response signal
void 		Touch_IIC_NoACK(void);							// Send a no-acknowledgement signal
uint8_t 	Touch_IIC_WaitACK(void);						//	wait for acknowledgment signal
uint8_t	Touch_IIC_WriteByte(uint8_t IIC_Data); 	// write byte function
uint8_t 	Touch_IIC_ReadByte(uint8_t ACK_Mode);		// read byte function
		
#endif //__IIC_H
