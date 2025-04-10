/*
 * MyButtonController.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: MehdiBMofidi
 */

#include "MyButtonController.hpp"
#include "main.h"
#include "touchgfx/hal/HAL.hpp"
extern "C" {
	extern  uint8_t User_ButtonState0;
	extern  uint8_t User_ButtonState1;
}
void MyButtonController::init()
{
	//for polling uncomment
//	previousState0=0xff;
//	previousState1=0xff;
}
bool MyButtonController::sample(uint8_t& key)
{
	//for polling mode
//	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_RESET)
//	{
//		if(previousState0==0Xff)
//		{
//			previousState0=0x00;
//			key=0;
//			return true;
//		}
//		return false;
//	}
//	previousState0=0xff;
//
//	if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_3)==GPIO_PIN_RESET)
//	{
//		if(previousState1==0Xff)
//		{
//			previousState1=0x00;
//			key=1;
//			return true;
//		}
//		return false;
//	}
//	previousState1=0xff;

	//for Interrupt Mode
	if(User_ButtonState0)
	{
		User_ButtonState0=0;
		key=1;
		return true;
	}

	if(User_ButtonState1)
	{
		User_ButtonState1=0;
		key=2;
		return true;
	}
	return false;

}

