/******************************************************** ***************************************************** ***************************************************** ***************************************************** ***********
* @file touch_800x480.c
* @version V1.0
* @date 2021-7-20
* @author anti-customer technology
* @brief GT911 touch driver
    ***************************************************** ***************************************************** ***************************************************** ***************************************************** ***********
    * @description
*
* Experimental platform: FK STM32H743IIT6 core board (model: FK743M2-IIT6) + FK 800*480 resolution RGB screen
*

>>>>> File description:
*
* 1. Operation functions related to touch screen
* 2. Use analog IIC
* 3. The default communication speed is 100KHz
*
	****************************************************************************************************************************************************************************************************************************************************FANKE*******
***/

#include "touch_800x480.h"

volatile TouchStructure touchInfo; 			//	Touch information structure, which is called in the function Touch_Scan() to store touch data
volatile static uint8_t Modify_Flag = 0;	// Touch coordinate modification flag
	

/****************************************************** ***************************************************** *********************************
* Function name: GT9XX_Reset
* Entry parameters: None
* Return value: None
* Function function: reset GT911
* Description: Reset GT911, and configure the IIC address of the chip as 0xBA/0xBB
**********************************************************************************************************************************/

void GT9XX_Reset(void)
{
	Touch_INT_Out();	//	Configure the INT pin as an output
	
	// Initialize pin state
	HAL_GPIO_WritePin(Touch_INT_PORT,Touch_INT_PIN,GPIO_PIN_RESET);  // INT output low level
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_SET);    // RST output high level
	Touch_IIC_Delay(10000);
	
	// start reset
// Keep the INT pin low and set the device address to 0XBA/0XBB
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_RESET); // Pull down the reset pin, at this time the chip performs a reset
	Touch_IIC_Delay(150000);			// time delay
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_SET);			//Pull the reset pin high, the reset is over
	Touch_IIC_Delay(350000);			// time delay
	Touch_INT_In();						// The INT pin is turned into a floating input
	Touch_IIC_Delay(20000);				// 
									
}

/******************************************************** ***************************************************** *******************************
* Function name: GT9XX_WriteHandle
* Entry parameters: addr - the register to be operated
* Return value: SUCCESS - the operation was successful
* ERROR - operation failed
* Function: GT9XX write operation
* Description: Write to the specified register
***************************************************************************************************************************************/

uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;		// status flag

	Touch_IIC_Start();	// Start IIC communication
	if( Touch_IIC_WriteByte(GT9XX_IIC_WADDR) == ACK_OK ) //write data command
	{
		if( Touch_IIC_WriteByte((uint8_t)(addr >> 8)) == ACK_OK ) //Write 16-bit address
		{
			if( Touch_IIC_WriteByte((uint8_t)(addr)) != ACK_OK )
			{
				status = ERROR;	// operation failed
			}			
		}
	}
	status = SUCCESS;	// Successful operation
	return status;	
}

/******************************************************** ***************************************************** *******************************
* Function name: GT9XX_WriteData
* Entry parameters: addr - the register to be written
* value - the data to write
* Return value: SUCCESS - the operation was successful
* ERROR - operation failed
* Function: GT9XX write one byte data
* Description: Write one byte of data to the specified register
***************************************************************************************************************************************/

uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	Touch_IIC_Start(); //Start IIC communication

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//Write to the register to be manipulated
	{
		if (Touch_IIC_WriteByte(value) != ACK_OK) //write data
		{
			status = ERROR;						
		}
	}	
	Touch_IIC_Stop(); // stop communication
	
	status = SUCCESS;	// write success
	return status;
}

/***************************************************** ***************************************************** *********************************
* Function name: GT9XX_WriteReg
* Entry parameters: addr - the first address of the register area to be written
* cnt - data length
* value - the data area to write
* Return value: SUCCESS - the operation was successful
* ERROR - operation failed
* Function: GT9XX write register
* Description: Write the data of the specified length to the register area of the chip
*************************************************************************************************************************************/

uint8_t GT9XX_WriteReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	Touch_IIC_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) 	// Write to the register to be manipulated
	{
		for(i = 0 ; i < cnt; i++)			 	// count
		{
			Touch_IIC_WriteByte(value[i]);	// data input
		}					
		Touch_IIC_Stop();		// stop IIC communication
		status = SUCCESS;		// write success
	}
	else
	{
		Touch_IIC_Stop();		// stop IIC communication
		status = ERROR;		// write failed
	}
	return status;	
}

/****************************************************** ***************************************************** *********************************
* Function name: GT9XX_ReadReg
* Entry parameters: addr - the first address of the register area to be read
* cnt - data length
* value - the data area to read
* Return value: SUCCESS - the operation was successful
* ERROR - operation failed
* Function: GT9XX read register
* Description: Read the data of the specified length from the register area of the chip
***************************************************************************************************************************************/

uint8_t GT9XX_ReadReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = ERROR;
	Touch_IIC_Start();		// Start IIC communication

	if( GT9XX_WriteHandle(addr) == SUCCESS) //Write to the register to be manipulated
	{
		Touch_IIC_Start(); //Restart IIC communication

		if (Touch_IIC_WriteByte(GT9XX_IIC_RADDR) == ACK_OK)	// send read command
		{	
			for(i = 0 ; i < cnt; i++)	// count
			{
				if (i == (cnt - 1))
				{
					value[i] = Touch_IIC_ReadByte(0);	//Send a non-response signal when the last data is read
				}
				else
				{
					value[i] = Touch_IIC_ReadByte(1);	// send acknowledgment signal
				}
			}					
			Touch_IIC_Stop();	// stop IIC communication
			status = SUCCESS;
		}
	}
	Touch_IIC_Stop();	// stop IIC communication
	return (status);	
}
/**************************************************** ***************************************************** ***********************************
* Function name: PanelRecognition
* Entry parameters: None
* Return value: None
* Function function: identify the version of the screen
* Note: For hardware versions before RGB070M1-800*480 V1.1, the resolution of the touch screen is 1024*600,
* For the compatibility of the program, the judgment process is performed here. The RST and INT pins of the old version are not connected to the core board.
* Therefore, it can be identified according to the level status of these two pins. This code is only useful for the old version of the 7-inch screen.
* Other size screens don't care
*************************************************************************************************************************************/

void	PanelRecognition (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	Touch_INT_CLK_ENABLE;	//Initialize the IO port clock
	Touch_RST_CLK_ENABLE;		

		
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;      		//	input mode
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;		 			//	pull down input
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    	// speed class
	GPIO_InitStruct.Pin   = Touch_INT_PIN ;  				// Initialize INT pin
	
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);			

	GPIO_InitStruct.Pin  = Touch_RST_PIN; 					//	RST
	HAL_GPIO_Init(Touch_RST_PORT, &GPIO_InitStruct);					   

	Touch_IIC_Delay(4000);		// time delay
	
	// The RST and INT pins of the old version are not connected to the core board, so they can be identified according to the level status of these two pins,
// The new version of the hardware has done pull-up processing on these two pins
	if( (HAL_GPIO_ReadPin(Touch_RST_PORT,Touch_RST_PIN) != 1) && (HAL_GPIO_ReadPin(Touch_INT_PORT,Touch_INT_PIN) != 1)  )	
	{// In the hardware version before V1.1, the resolution of the touch screen is 1024*600, for program compatibility, here is the judgment processing
// This variable flag is mainly used to determine whether the software needs to modify the collected touch coordinates
// This code is only valid for the old version of the 7-inch screen, and other sizes of screen do not need to be ignored
		Modify_Flag	= 1;			
	}
}
/**************************************************** ***************************************************** ***********************************
* Function name: Touch_Init
* Entry parameters: None
* Return value: SUCCESS - initialization succeeded
* ERROR - Error, no touchscreen detected
* Function: Initialize the touch IC, read the corresponding information and send it to the serial port
* Description: Initialize the touch panel
**********************************************************************************************************************************/

uint8_t Touch_Init(void)
{
	uint8_t GT9XX_Info[11];	// Touch screen IC information
	uint8_t cfgVersion = 0;	// Touch configuration version

	
	// Identify the version of the screen, in RGB070M1-800*480 V1.1 and later hardware versions, do not need to ignore this code
// This code is only valid for the old version of the 7-inch screen, and other sizes of screen do not need to be ignored
	PanelRecognition();			
	
	Touch_IIC_GPIO_Config(); 	// Initialize IIC pins
	GT9XX_Reset();					// Reset IC
		
	GT9XX_ReadReg (GT9XX_ID_ADDR,11,GT9XX_Info);		// Read touch screen IC information
	GT9XX_ReadReg (GT9XX_CFG_ADDR,1,&cfgVersion);	// Read touch configuration version

	
	if( GT9XX_Info[0] == '9' )	//Check if the first character is 9
	{
//		printf("Touch ID: GT%.4s \r\n",GT9XX_Info); //print the ID of the touch chip
// printf("Firmware version: 0X%.4x\r\n",(GT9XX_Info[5]<<8) + GT9XX_Info[4]); // Chip firmware version
// printf("Touch resolution: %d * %d\r\n",(GT9XX_Info[7]<<8) + GT9XX_Info[6],(GT9XX_Info[9]<<8) +GT9XX_Info[8]) ; // current touch resolution
// printf("Touch parameter configuration version: 0X%.2x \r\n",cfgVersion); // Touch configuration version

/*---------Version identification, RGB070M1-800*480 V1.1 and later hardware versions or screens of other sizes, do not need to ignore this code -----*/
		if( ( (GT9XX_Info[7]<<8) + GT9XX_Info[6] ) == 1024 )		// Determine whether the X-axis resolution of the touch screen is 1024
		{
			// In the hardware version before RGB070M1-800*480 V1.1, the resolution of the touch screen is 1024*600. For the compatibility of the program, the judgment process is performed here
// This variable flag is mainly used to determine whether the software needs to modify the collected touch coordinates
			Modify_Flag	= 1;	
		}
		else if( ( (GT9XX_Info[7]<<8) + GT9XX_Info[6] ) == 800 )	// The X-axis resolution of the touch screen is 800
		{
			Modify_Flag	= 0;	// Set the flag to 0, no need to do processing
		}
		
/*-------------------------------------------------------------------------------------------------*/			
		return SUCCESS;
	}
	else
	{
//		printf("Touch Error\r\n");	//error, no touchscreen detected
		return ERROR;
	}

}
/**************************************************** ***************************************************** ***********************************
* Function name: Touch_Scan
* Entry parameters: None
* Return value: None
* Function function: touch scan
* Description: Call this function periodically in the program to detect touch operations, and the touch information is stored in the touchInfo structure
***************************************************************************************************************************************/

void Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; 		// Used to store touch data
	uint8_t  i = 0;	
	
	GT9XX_ReadReg (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	// read data
	GT9XX_WriteData (GT9XX_READ_ADDR,0);									//	Clear the register flag bit of the touch chip
	touchInfo.num = touchData[0] & 0x0f;									// Get the current touch points
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) 	//	When the number of touches is between 1-5
	{
		for(i=0;i<touchInfo.num;i++)		// Get the corresponding touch coordinates
		{
			touchInfo.y[i] = (touchData[5+8*i]<<8) | touchData[4+8*i];	// Get the Y coordinate
			touchInfo.x[i] = (touchData[3+8*i]<<8) | touchData[2+8*i];	//	Get the X coordinate	

/*---------Version identification, RGB070M1-800*480 V1.1 and later hardware versions or screens of other sizes, do not need to ignore this code -----*/

// In the hardware version before V1.1, the resolution of the touch screen is 1024*600, for program compatibility, here is the judgment processing
// This variable flag is mainly used to determine whether the software needs to modify the collected touch coordinates
			if( Modify_Flag == 1)
			{
				touchInfo.y[i] *= 0.8;		// Convert touch coordinates of 1024*600 resolution to 800*480 resolution
				touchInfo.x[i] *= 0.78;		// Convert touch coordinates of 1024*600 resolution to 800*480 resolution
			}		
/*-------------------------------------------------------------------------------------------------*/			
			
		}
		touchInfo.flag = 1;	// The position of the touch flag is 1, which means that there is a touch action
	}
	else                       
	{
		touchInfo.flag = 0;	// Touch flag position 0, no touch action
	}
}

/*************************************************************************************************************************************************************************************************FANKE****/

