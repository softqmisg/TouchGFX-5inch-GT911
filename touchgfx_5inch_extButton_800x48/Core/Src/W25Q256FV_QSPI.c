
#include "W25Q256FV_QSPI.h"
#include "main.h"

static void W25Q256FV_QSPI_WaitForWriteEnable(QSPI_HandleTypeDef *hqspi);
static void W25Q256FV_QSPI_WaitForBusy(QSPI_HandleTypeDef *hqspi);

/*
  * @brief  Initializes the QSPI interface.
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_Init(QSPI_HandleTypeDef *hqspi)
{

 /* QSPI memory reset */
  if (W25Q256FV_QSPI_ResetMemory(hqspi) != QSPI_OK)
  {
    return QSPI_NOT_SUPPORTED;
  }
  HAL_Delay(1);

  /*if (W25Q256FV_QSPI_WriteEnable(hqspi)!= QSPI_OK) {
	  return QSPI_NOT_SUPPORTED;
  }*/
	
	if(W25Q256FV_QSPI_AutoPollingMemReady(hqspi, W25Q256FV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK){

      return QSPI_NOT_SUPPORTED;
  }
	
	
  /* Set the QSPI memory in 4-bytes address mode */
 if (W25Q256FV_QSPI_EnterFourBytesAddress(hqspi) != QSPI_OK)
  {
    return QSPI_NOT_SUPPORTED;
  }

   /*if (W25Q256FV_QSPI_EnableQspi(hqspi) != QSPI_OK){
  	 return QSPI_NOT_SUPPORTED;
   }*/

  /* Configuration of the dummy cycles on QSPI memory side */
  if (W25Q256FV_QSPI_DummyCyclesCfg(hqspi) != QSPI_OK)
  {
    return QSPI_NOT_SUPPORTED;
  }

 /*if (W25Q256FV_QSPI_WriteDefaultStatusREGs(hqspi) != QSPI_OK){
	 return QSPI_NOT_SUPPORTED;
 }*/

  return QSPI_OK;
}

/**
  * @brief  De-Initializes the QSPI interface.
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_DeInit(QSPI_HandleTypeDef *hqspi)
{
  /* Call the DeInit function to reset the driver */
  if (HAL_QSPI_DeInit(hqspi) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_Read(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
QSPI_HandleTypeDef W25Q256FV_Qspi = *QSPIHandle;
  QSPI_CommandTypeDef s_command;

  /* Initialize the read command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_OUT_FAST_READ_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = W25Q256FV_DUMMY_CYCLES_READ_QUAD;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Set S# timing for Read command */

  MODIFY_REG(W25Q256FV_Qspi.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);

  /* Reception of the data */
  if (HAL_QSPI_Receive(QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Restore S# timing for nonRead commands */
  MODIFY_REG(W25Q256FV_Qspi.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);

  return QSPI_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_Write(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;
  uint32_t end_addr, current_size, current_addr;

  /* Calculation of the size between the write address and the end of the page */
  current_size = W25Q256FV_PAGE_SIZE - (WriteAddr % W25Q256FV_PAGE_SIZE);

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the address variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Initialize the program command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_IN_FAST_PROG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Perform the write page by page */
  do
  {
    s_command.Address = current_addr;
    s_command.NbData  = current_size;

    /* Enable write operations */
    if (W25Q256FV_QSPI_WriteEnable(QSPIHandle) != QSPI_OK)
    {
    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      return QSPI_ERROR;
    }

    /* Configure the command */
    if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      return QSPI_ERROR;
    }

    /* Transmission of the data */
    if (HAL_QSPI_Transmit(QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait for end of program */
    if (W25Q256FV_QSPI_AutoPollingMemReady(QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      return QSPI_ERROR;
    }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25Q256FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q256FV_PAGE_SIZE;
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  } while (current_addr < end_addr);

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
  return QSPI_OK;
}


/**
  * @brief  Erases the specified Sector of the QSPI memory.
  * @param  StartAddress: Start address to erase
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_EraseSector(QSPI_HandleTypeDef *QSPIHandle, uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
  QSPI_CommandTypeDef s_command;
  EraseStartAddress = EraseStartAddress - EraseStartAddress % W25Q256FV_SECTOR_SIZE;

  /* Initialize the erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = SECTOR_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  while (EraseEndAddress >= EraseStartAddress) {
	  s_command.Address = (EraseStartAddress & 0x0FFFFFFF);

  /* Enable write operations */
  if (W25Q256FV_QSPI_WriteEnable(QSPIHandle) != QSPI_OK)
  {
		return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
		return QSPI_ERROR;
  }

  EraseStartAddress += W25Q256FV_SECTOR_SIZE;

  /* Configure automatic polling mode to wait for end of erase */

  if (W25Q256FV_QSPI_AutoPollingMemReady(QSPIHandle, W25Q256FV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
  {
     return QSPI_ERROR;
  }

  }


  return QSPI_OK;
}



/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  BlockAddress: Block address to erase
  * @retval QSPI memory status
  */

uint8_t W25Q256FV_QSPI_EraseBlock(QSPI_HandleTypeDef *QSPIHandle, uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
  QSPI_CommandTypeDef s_command;
  EraseStartAddress = EraseStartAddress - EraseStartAddress % W25Q256FV_BLOCK_SIZE;

  /* Initialize the erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = BLOCK_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  while (EraseEndAddress >= EraseStartAddress) {
	  s_command.Address = (EraseStartAddress & 0x0FFFFFFF);

  /* Enable write operations */
  if (W25Q256FV_QSPI_WriteEnable(QSPIHandle) != QSPI_OK)
  {
		return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
		return QSPI_ERROR;
  }

  EraseStartAddress += W25Q256FV_BLOCK_SIZE;

  /* Configure automatic polling mode to wait for end of erase */

  if (W25Q256FV_QSPI_AutoPollingMemReady(QSPIHandle, W25Q256FV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
  {
     return QSPI_ERROR;
  }

  }


  return QSPI_OK;
}



/**
  * @brief  Erases the entire QSPI memory.
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_Erase_Chip(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CHIP_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (W25Q256FV_QSPI_WriteEnable (QSPIHandle) != QSPI_OK)
  {
      return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
      return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait for end of erase */
  if (W25Q256FV_QSPI_AutoPollingMemReady(QSPIHandle, W25Q256FV_BULK_ERASE_MAX_TIME) != QSPI_OK)
  {
      return QSPI_ERROR;
  }


  return QSPI_OK;
}

/**
  * @brief  Reads current status of the QSPI memory.
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_GetStatus(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg;

  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_FLAG_STATUS_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the value of the register */
  if ((reg & (W25Q256FV_FSR_PRERR | W25Q256FV_FSR_VPPERR | W25Q256FV_FSR_PGERR | W25Q256FV_FSR_ERERR)) != 0)
  {
    return QSPI_ERROR;
  }
  else if ((reg & (W25Q256FV_FSR_PGSUS | W25Q256FV_FSR_ERSUS)) != 0)
  {
    return QSPI_SUSPENDED;
  }
  else if ((reg & W25Q256FV_FSR_READY) != 0)
  {
    return QSPI_OK;
  }
  else
  {
    return QSPI_BUSY;
  }
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  pInfo: pointer on the configuration structure
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_GetInfo(QSPI_Info* pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize          = W25Q256FV_FLASH_SIZE;
  pInfo->EraseSectorSize    = W25Q256FV_SECTOR_SIZE;
  pInfo->EraseSectorsNumber = (W25Q256FV_FLASH_SIZE/W25Q256FV_SECTOR_SIZE);
  pInfo->ProgPageSize       = W25Q256FV_PAGE_SIZE;
  pInfo->ProgPagesNumber    = (W25Q256FV_FLASH_SIZE/W25Q256FV_PAGE_SIZE);

  return QSPI_OK;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  * @retval QSPI memory status
  */
uint8_t W25Q256FV_QSPI_EnableMemoryMappedMode(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* Configure the command for the read instruction */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_OUT_FAST_READ_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = W25Q256FV_DUMMY_CYCLES_READ_QUAD;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}




/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi: QSPI handle
  * @retval None
  */
uint8_t W25Q256FV_QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = RESET_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the reset memory command */
  s_command.Instruction = RESET_MEMORY_CMD;
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait the memory is ready */
  if (W25Q256FV_QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function set the QSPI memory in 4-byte address mode
  * @param  hqspi: QSPI handle
  * @retval None
  */
uint8_t W25Q256FV_QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (W25Q256FV_QSPI_WriteEnable (hqspi) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait the memory is ready */
  if (W25Q256FV_QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function configure the dummy cycles on memory side.
  * @param  hqspi: QSPI handle
  * @retval None
  */
uint8_t W25Q256FV_QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg;

  /* Initialize the read volatile configuration register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_VOL_CFG_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Enable write operations */
  if (W25Q256FV_QSPI_WriteEnable (hqspi) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Update volatile configuration register (with new dummy cycles) */
  s_command.Instruction = WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(reg, W25Q256FV_VCR_NB_DUMMY, (W25Q256FV_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(W25Q256FV_VCR_NB_DUMMY)));

  /* Configure the write volatile configuration register command */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */
uint8_t W25Q256FV_QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Enable write operations */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = WRITE_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  s_config.Match           = W25Q256FV_SR_WREN;
  s_config.Mask            = W25Q256FV_SR_WREN;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction    = READ_STATUS_REG1_CMD;
  s_command.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @param  Timeout
  * @retval None
  */
uint8_t W25Q256FV_QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Configure automatic polling mode to wait for memory ready */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_STATUS_REG1_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0;
  s_config.Mask            = W25Q256FV_SR_WIP;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Enable Qspi bit in status reg 2.
  * @retval Operation Resault
  */
uint8_t W25Q256FV_QSPI_EnableQspi(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef s_command;
  uint8_t QSPIEN=0x02;

  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       =	WRITE_STATUS_REG2_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Transmit of the data */

  if (HAL_QSPI_Transmit(QSPIHandle,&QSPIEN, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }


  return QSPI_OK;
}

/**
  * @brief  Check Qspi enable bit is set in status reg 2.
  * @retval QSPI enable status
  */
uint8_t W25Q256FV_QSPI_IsEnabled(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg;
  uint8_t QSPIEN=0x02;

  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_STATUS_REG2_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the value of the register */
  if (reg & QSPIEN){
	  return QSPI_OK;
  }else{
	  return QSPI_NotEnabled;
  }


}


/**
  * @brief  Reads status Registers of the QSPI memory.
  * @retval QSPI selected status Register value
  */
uint8_t W25Q256FV_QSPI_ReadStatusREG(QSPI_HandleTypeDef *QSPIHandle,uint8_t regN)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg;

  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  switch (regN){
  case 1:
	  s_command.Instruction  = READ_STATUS_REG1_CMD;
	  break;
  case 2:
	  s_command.Instruction  = READ_STATUS_REG2_CMD;
	  break;
  case 3:
	  s_command.Instruction  = READ_STATUS_REG3_CMD;
	  break;
  }

  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the value of the register */

  return reg;
}


/**
  * @brief  Write status Registers of the QSPI memory.
  * @retval Operation Resault
  */
uint8_t W25Q256FV_QSPI_WriteStatusREG(QSPI_HandleTypeDef *QSPIHandle,uint8_t regN,uint8_t regVal)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  switch (regN){
  case 1:
	  s_command.Instruction  = WRITE_STATUS_REG1_CMD;
	  break;
  case 2:
	  s_command.Instruction  = WRITE_STATUS_REG2_CMD;
	  break;
  case 3:
	  s_command.Instruction  = WRITE_STATUS_REG3_CMD;
	  break;
  }

  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Transmit of the data */

  if (HAL_QSPI_Transmit(QSPIHandle,&regVal, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }


  return QSPI_OK;
}

/**
  * @brief  Write factory default value of status Registers .
  * @retval Operation Resault
  */
uint8_t W25Q256FV_QSPI_WriteDefaultStatusREGs(QSPI_HandleTypeDef *QSPIHandle){

	uint8_t res = QSPI_OK;

	res = W25Q256FV_QSPI_WriteStatusREG(QSPIHandle, 1, 0x00);
	W25Q256FV_QSPI_WaitForWriteEnable(QSPIHandle);
	//res = W25Q256FV_QSPI_WriteStatusREG(QSPIHandle, 2, 0x00);//By Factory Default QE Bit = 0
	res = W25Q256FV_QSPI_WriteStatusREG(QSPIHandle, 2, 0x02); // Enable Memory QSPI QE Bit = 1
	W25Q256FV_QSPI_WaitForWriteEnable(QSPIHandle);
	//res = W25Q256FV_QSPI_WriteStatusREG(QSPIHandle, 3, 0x60); //By Factory Default Read Driver Strength = 25% (0x60)
	res = W25Q256FV_QSPI_WriteStatusREG(QSPIHandle, 3, 0x00); //Set Read Driver Strength = 100%
	W25Q256FV_QSPI_WaitForWriteEnable(QSPIHandle);

	return res;

}

static void W25Q256FV_QSPI_WaitForWriteEnable(QSPI_HandleTypeDef *hqspi){

	 uint8_t reg = 0;

	while(!(reg & 0x02)){
		W25Q256FV_QSPI_WriteEnable(hqspi);
		W25Q256FV_QSPI_WaitForBusy(hqspi);
		reg = W25Q256FV_QSPI_ReadStatusREG(hqspi, 1);
	}

}

static void W25Q256FV_QSPI_WaitForBusy(QSPI_HandleTypeDef *hqspi){


	uint8_t reg = W25Q256FV_QSPI_ReadStatusREG(hqspi, 1);
	while(reg & 0x01){
		reg = W25Q256FV_QSPI_ReadStatusREG(hqspi, 1);
	}
}
