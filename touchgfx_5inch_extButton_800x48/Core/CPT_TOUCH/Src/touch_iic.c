/***
	***************************************************** ***************************************************** *********************
* @file touch_iic.c
* @version V1.0
* @date 2021-7-20
* @author anti-customer technology
* @brief touch screen IIC interface related functions
    ***************************************************** ***************************************************** *********************
    * @description
*
* Experimental platform: anti-customer STM32H743IIT6 core board (model: FK743M2-IIT6)

>>>>> File description:
*
* 1. IIC functions related to touch screen
* 2. Use analog IIC
* 3. The default communication speed is 100KHz
*
*************************************************************************************************************************
***/

#include "touch_iic.h"  


/****************************************************** *****************************************
* Function name: Touch_IIC_GPIO_Config
* Entry parameters: None
* Return value: None
* Function: Initialize the GPIO port of IIC, push-pull output
* Note: Since the IIC communication speed is not high, the IO port speed here can be configured as 2M
****************************************************************************************/

void Touch_IIC_GPIO_Config (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	Touch_IIC_SCL_CLK_ENABLE;	//Initialize the IO port clock
	Touch_IIC_SDA_CLK_ENABLE;
	Touch_INT_CLK_ENABLE;	
	Touch_RST_CLK_ENABLE;	
	
	GPIO_InitStruct.Pin 			= Touch_IIC_SCL_PIN;				// SCL pin
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_OD;			// open drain output
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;						// without pull-up
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;			// speed class 
	HAL_GPIO_Init(Touch_IIC_SCL_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin 			= Touch_IIC_SDA_PIN;				// SDA pin
	HAL_GPIO_Init(Touch_IIC_SDA_PORT, &GPIO_InitStruct);		

	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      			// push-pull output
	GPIO_InitStruct.Pull  = GPIO_PULLUP;		 					// pull up	
	
	GPIO_InitStruct.Pin = Touch_INT_PIN; 							//	INT
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);				

	GPIO_InitStruct.Pin = Touch_RST_PIN; 							//	RST
	HAL_GPIO_Init(Touch_RST_PORT, &GPIO_InitStruct);					   

	HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_SET);		// SCL output high level
	HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_SET);    // SDA output high level
	HAL_GPIO_WritePin(Touch_INT_PORT, 	  Touch_INT_PIN,     GPIO_PIN_RESET);  // INT output low level
	HAL_GPIO_WritePin(Touch_RST_PORT,     Touch_RST_PIN,     GPIO_PIN_SET);    // RST output high level

}

/***************************************************** *****************************************
* Function name: Touch_IIC_Delay
* Entry parameters: a - delay time
* Return value: None
* Function function: simple delay function
* Note: For the simplicity of transplantation and the low requirement for delay accuracy, there is no need to use a timer for delay
*********************************************************************************************/

void Touch_IIC_Delay(uint32_t a)
{
	volatile uint16_t i;
	while (a --)				
	{
		for (i = 0; i < 8; i++);
	}
}

/****************************************************** *****************************************
* Function name: Touch_IIC_INT_Out
* Entry parameters: None
* Return value: None
* Function function: Configure the INT pin of IIC as output mode
* Description: None
*********************************************************************************************/

void Touch_INT_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      	// output mode
	GPIO_InitStruct.Pull  = GPIO_PULLUP;		 			// pull up	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    	// speed class
	GPIO_InitStruct.Pin   = Touch_INT_PIN ;  				// Initialize INT pin
	
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);		
}
/**************************************************** *****************************************
* Function name: Touch_IIC_INT_In
* Entry parameters: None
* Return value: None
* Function function: Configure the INT pin of IIC as input mode
* Description: None
*********************************************************************************************/

void Touch_INT_In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;      		// input mode
	GPIO_InitStruct.Pull  = GPIO_NOPULL;		 			// Floating	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    	// speed class
	GPIO_InitStruct.Pin   = Touch_INT_PIN ;  				// Initialize INT pin
	
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);		

}
/**************************************************** *****************************************
* Function name: Touch_IIC_Start
* Entry parameters: None
* Return value: None
* Function function: IIC start signal
* Note: When SCL is at high level, SDA transitions from high to low to start signal
***************************************************** *********************************************/
void Touch_IIC_Start(void)
{
	Touch_IIC_SDA(1);		
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
}

/******************************************************* ***************************************
* Function name: Touch_IIC_Stop
* Entry parameters: None
* Return value: None
* Function function: IIC stop signal
* Note: When SCL is at high level, SDA changes from low to high to start signal
*********************************************************************************************/

void Touch_IIC_Stop(void)
{
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
}

/******************************************************* ***************************************
* Function name: Touch_IIC_ACK
* Entry parameters: None
* Return value: None
* Function function: IIC response signal
* Explanation: When SCL is high level, the SDA pin output is low level, and a response signal is generated
*******************************************************************************************/

void Touch_IIC_ACK(void)
{
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);	
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(0);		// When the SCL output is low, SDA should be pulled high immediately, releasing the bus
	Touch_IIC_SDA(1);		
	
	Touch_IIC_Delay(IIC_DelayVaule);

}

/******************************************************* ***************************************
* Function name: Touch_IIC_NoACK
* Entry parameters: None
* Return value: None
* Function function: IIC non-response signal
* Note: When SCL is high level, if the SDA pin is high level, a non-response signal is generated
*********************************************************************************************/

void Touch_IIC_NoACK(void)
{
	Touch_IIC_SCL(0);	
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
}
/**************************************************** *****************************************
* Function name: Touch_IIC_WaitACK
* Entry parameters: None
* Return value: None
* Function: Wait for the receiving device to send a response signal
* Note: When SCL is high level, if the SDA pin is detected as low level, the receiving device responds normally
*****************************************************************************************/

uint8_t Touch_IIC_WaitACK(void)
{
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);	
	
	if( HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT,Touch_IIC_SDA_PIN) != 0) //Determine if the device is responding		
	{
		Touch_IIC_SCL(0);	
		Touch_IIC_Delay( IIC_DelayVaule );		
		return ACK_ERR;	//no response
	}
	else
	{
		Touch_IIC_SCL(0);	
		Touch_IIC_Delay( IIC_DelayVaule );		
		return ACK_OK;	//normal response
	}
}

/****************************************************** *****************************************
* Function name: Touch_IIC_WriteByte
* Entry parameters: IIC_Data - 8-bit data to be written
* Return value: ACK_OK - the device responded normally
* ACK_ERR - Device responded with an error
* Function function: write one byte data
* Explanation: the high bit comes first
*********************************************************************************************/

uint8_t Touch_IIC_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		Touch_IIC_SDA(IIC_Data & 0x80);
		
		Touch_IIC_Delay( IIC_DelayVaule );
		Touch_IIC_SCL(1);
		Touch_IIC_Delay( IIC_DelayVaule );
		Touch_IIC_SCL(0);		
		if(i == 7)
		{
			Touch_IIC_SDA(1);			
		}
		IIC_Data <<= 1;
	}

	return Touch_IIC_WaitACK(); //Wait for the device to respond
}

/****************************************************** *****************************************
* Function name: Touch_IIC_ReadByte
* Entry parameters: ACK_Mode - Response mode, input 1 to send an acknowledgment signal, input 0 to send a non-response signal
* Return value: ACK_OK - the device responded normally
* ACK_ERR - Device responded with an error
* Function function: read one byte of data
* Explanation: 1. The high bit comes first
* 2. A non-response signal should be sent when the host receives the last byte of data
********************************************************************************************/

uint8_t Touch_IIC_ReadByte(uint8_t ACK_Mode)
{
	uint8_t IIC_Data = 0;
	uint8_t i = 0;
	
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		Touch_IIC_SCL(1);
		Touch_IIC_Delay( IIC_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT,Touch_IIC_SDA_PIN) & 0x01);	
		Touch_IIC_SCL(0);
		Touch_IIC_Delay( IIC_DelayVaule );
	}
	
	if ( ACK_Mode == 1 )				//	answer signal
		Touch_IIC_ACK();
	else
		Touch_IIC_NoACK();		 	// non-response signal
	
	return IIC_Data; 
}

/********************************************************************************************/
