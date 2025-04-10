#ifndef W25Q256FV_QSPI_H
#define W25Q256FV_QSPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"


 /**
   * @brief  W25Q256FV Configuration
   */
 #define W25Q256FV_FLASH_SIZE                  0x2000000 /* 256 MBits => 32MBytes */
 #define W25Q256FV_BLOCK_SIZE                  0x10000   /* 512 sectors of 64KBytes */
 #define W25Q256FV_SECTOR_SIZE         	       0x1000    /* 8192 subsectors of 4kBytes */
 #define W25Q256FV_PAGE_SIZE                   0x100     /* 131072 pages of 256 bytes */

 #define W25Q256FV_DUMMY_CYCLES_READ           8
 #define W25Q256FV_DUMMY_CYCLES_READ_QUAD      8
 #define W25Q256FV_DUMMY_CYCLES_READ_DTR       8
 #define W25Q256FV_DUMMY_CYCLES_READ_QUAD_DTR  8

 #define W25Q256FV_BULK_ERASE_MAX_TIME         400000
 #define W25Q256FV_SECTOR_ERASE_MAX_TIME       2000
 #define W25Q256FV_SUBSECTOR_ERASE_MAX_TIME    400

 /**
   * @brief  W25Q256FV Commands
   */
 /* Reset Operations */
 #define RESET_ENABLE_CMD                     0x66
 #define RESET_MEMORY_CMD                     0x99

 /* Identification Operations */
 #define READ_ID_CMD                          0x9E
 #define READ_ID_CMD2                         0x9F
 #define MULTIPLE_IO_READ_ID_CMD              0xAF
 #define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

 /* Read Operations */
 #define READ_CMD                             0x03
 #define READ_4_BYTE_ADDR_CMD                 0x13

 #define FAST_READ_CMD                        0x0B
 #define FAST_READ_DTR_CMD                    0x0D
 #define FAST_READ_4_BYTE_ADDR_CMD            0x0C

 #define DUAL_OUT_FAST_READ_CMD               0x3B
 #define DUAL_OUT_FAST_READ_DTR_CMD           0x3D
 #define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x3C

 #define DUAL_INOUT_FAST_READ_CMD             0xBB
 #define DUAL_INOUT_FAST_READ_DTR_CMD         0xBD
 #define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

 #define QUAD_OUT_FAST_READ_CMD               0x6B
 #define QUAD_OUT_FAST_READ_DTR_CMD           0x6D
 #define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x6C

 #define QUAD_INOUT_FAST_READ_CMD             0xEB
 #define QUAD_INOUT_FAST_READ_DTR_CMD         0xED
 #define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC

 /* Write Operations */
 #define WRITE_ENABLE_CMD                     0x06
 #define WRITE_DISABLE_CMD                    0x04

 /* Register Operations */
 #define READ_STATUS_REG1_CMD                  0x05
 #define WRITE_STATUS_REG1_CMD                 0x01

 #define READ_STATUS_REG2_CMD                  0x35
 #define WRITE_STATUS_REG2_CMD                 0x31

 #define READ_STATUS_REG3_CMD                  0x15
 #define WRITE_STATUS_REG3_CMD                 0x11

 #define unlock_blocks                         0x39

 #define READ_LOCK_REG_CMD                    0xE8
 #define WRITE_LOCK_REG_CMD                   0xE5

 #define READ_FLAG_STATUS_REG_CMD             0x70
 #define CLEAR_FLAG_STATUS_REG_CMD            0x50

 #define READ_NONVOL_CFG_REG_CMD              0xB5
 #define WRITE_NONVOL_CFG_REG_CMD             0xB1

 #define READ_VOL_CFG_REG_CMD                 0x85
 #define WRITE_VOL_CFG_REG_CMD                0x81

 #define READ_ENHANCED_VOL_CFG_REG_CMD        0x65
 #define WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61

 #define READ_EXT_ADDR_REG_CMD                0xC8
 #define WRITE_EXT_ADDR_REG_CMD               0xC5

 /* Program Operations */
 #define PAGE_PROG_CMD                        0x02
 #define PAGE_PROG_4_BYTE_ADDR_CMD            0x12

 #define DUAL_IN_FAST_PROG_CMD                0xA2
 #define EXT_DUAL_IN_FAST_PROG_CMD            0xD2

 #define QUAD_IN_FAST_PROG_CMD                0x32
 #define EXT_QUAD_IN_FAST_PROG_CMD            0x12 /*0x38*/
 #define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x34

 /* Erase Operations */
 #define SECTOR_ERASE_CMD                     0x20
 #define BLOCK_ERASE_CMD                      0xD8
 #define CHIP_ERASE_CMD                       0xC7


 #define PROG_ERASE_RESUME_CMD                0x7A
 #define PROG_ERASE_SUSPEND_CMD               0x75

 /* One-Time Programmable Operations */
 #define READ_OTP_ARRAY_CMD                   0x4B
 #define PROG_OTP_ARRAY_CMD                   0x42

 /* 4-byte Address Mode Operations */
 #define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
 #define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

 /* Quad Operations */
 #define ENTER_QUAD_CMD                       0x35
 #define EXIT_QUAD_CMD                        0xF5

 /**
   * @brief  W25Q256FV Registers
   */
 /* Status Register */
 #define W25Q256FV_SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
 #define W25Q256FV_SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */
 #define W25Q256FV_SR_BLOCKPR                  ((uint8_t)0x5C)    /*!< Block protected against program and erase operations */
 #define W25Q256FV_SR_PRBOTTOM                 ((uint8_t)0x20)    /*!< Protected memory area defined by BLOCKPR starts from top or bottom */
 #define W25Q256FV_SR_SRWREN                   ((uint8_t)0x80)    /*!< Status register write enable/disable */

 /* Nonvolatile Configuration Register */
 #define W25Q256FV_NVCR_NBADDR                 ((uint16_t)0x0001) /*!< 3-bytes or 4-bytes addressing */
 #define W25Q256FV_NVCR_SEGMENT                ((uint16_t)0x0002) /*!< Upper or lower 128Mb segment selected by default */
 #define W25Q256FV_NVCR_DUAL                   ((uint16_t)0x0004) /*!< Dual I/O protocol */
 #define W25Q256FV_NVCR_QUAB                   ((uint16_t)0x0008) /*!< Quad I/O protocol */
 #define W25Q256FV_NVCR_RH                     ((uint16_t)0x0010) /*!< Reset/hold */
 #define W25Q256FV_NVCR_ODS                    ((uint16_t)0x01C0) /*!< Output driver strength */
 #define W25Q256FV_NVCR_XIP                    ((uint16_t)0x0E00) /*!< XIP mode at power-on reset */
 #define W25Q256FV_NVCR_NB_DUMMY               ((uint16_t)0xF000) /*!< Number of dummy clock cycles */

 /* Volatile Configuration Register */
 #define W25Q256FV_VCR_WRAP                    ((uint8_t)0x03)    /*!< Wrap */
 #define W25Q256FV_VCR_XIP                     ((uint8_t)0x08)    /*!< XIP */
 #define W25Q256FV_VCR_NB_DUMMY                ((uint8_t)0xF0)    /*!< Number of dummy clock cycles */

 /* Extended Address Register */
 #define W25Q256FV_EAR_A24                     ((uint8_t)0x01)    /*!< Select the lower or upper 128Mb segment */

 /* Enhanced Volatile Configuration Register */
 #define W25Q256FV_EVCR_ODS                    ((uint8_t)0x07)    /*!< Output driver strength */
 #define W25Q256FV_EVCR_VPPA                   ((uint8_t)0x08)    /*!< Vpp accelerator */
 #define W25Q256FV_EVCR_RH                     ((uint8_t)0x10)    /*!< Reset/hold */
 #define W25Q256FV_EVCR_DUAL                   ((uint8_t)0x40)    /*!< Dual I/O protocol */
 #define W25Q256FV_EVCR_QUAD                   ((uint8_t)0x80)    /*!< Quad I/O protocol */

 /* Flag Status Register */
 #define W25Q256FV_FSR_NBADDR                  ((uint8_t)0x01)    /*!< 3-bytes or 4-bytes addressing */
 #define W25Q256FV_FSR_PRERR                   ((uint8_t)0x02)    /*!< Protection error */
 #define W25Q256FV_FSR_PGSUS                   ((uint8_t)0x04)    /*!< Program operation suspended */
 #define W25Q256FV_FSR_VPPERR                  ((uint8_t)0x08)    /*!< Invalid voltage during program or erase */
 #define W25Q256FV_FSR_PGERR                   ((uint8_t)0x10)    /*!< Program error */
 #define W25Q256FV_FSR_ERERR                   ((uint8_t)0x20)    /*!< Erase error */
 #define W25Q256FV_FSR_ERSUS                   ((uint8_t)0x40)    /*!< Erase operation suspended */
 #define W25Q256FV_FSR_READY                   ((uint8_t)0x80)    /*!< Ready or command in progress */


/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)
#define QSPI_NotEnabled     ((uint8_t)0x10)



#define QSPI_FORCE_RESET()         __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()       __HAL_RCC_QSPI_RELEASE_RESET()



/* W25Q256FV */
/* Size of the flash */
#define QSPI_FLASH_SIZE            24     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/**
  * @}
  */


/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} QSPI_Info;

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup QSPI_Exported_Functions
  * @{
  */
uint8_t W25Q256FV_QSPI_Init       (QSPI_HandleTypeDef *hqspi);
uint8_t W25Q256FV_QSPI_DeInit     (QSPI_HandleTypeDef *hqspi);

uint8_t W25Q256FV_QSPI_ResetMemory          (QSPI_HandleTypeDef *hqspi);
uint8_t W25Q256FV_QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);
uint8_t W25Q256FV_QSPI_DummyCyclesCfg       (QSPI_HandleTypeDef *hqspi);
uint8_t W25Q256FV_QSPI_WriteEnable          (QSPI_HandleTypeDef *hqspi);
uint8_t W25Q256FV_QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);

uint8_t W25Q256FV_QSPI_Read       (QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t W25Q256FV_QSPI_Write      (QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t W25Q256FV_QSPI_EraseSector(QSPI_HandleTypeDef *QSPIHandle, uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t W25Q256FV_QSPI_EraseBlock(QSPI_HandleTypeDef *QSPIHandle, uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t W25Q256FV_QSPI_Erase_Chip (QSPI_HandleTypeDef *QSPIHandle);
uint8_t W25Q256FV_QSPI_GetStatus  (QSPI_HandleTypeDef *QSPIHandle);
uint8_t W25Q256FV_QSPI_GetInfo    (QSPI_Info* pInfo);
uint8_t W25Q256FV_QSPI_EnableMemoryMappedMode(QSPI_HandleTypeDef *QSPIHandle);
uint8_t W25Q256FV_QSPI_EnableQspi(QSPI_HandleTypeDef *QSPIHandle);
uint8_t W25Q256FV_QSPI_IsEnabled(QSPI_HandleTypeDef *QSPIHandle);
uint8_t W25Q256FV_QSPI_ReadStatusREG(QSPI_HandleTypeDef *QSPIHandle,uint8_t regN);
uint8_t W25Q256FV_QSPI_WriteStatusREG(QSPI_HandleTypeDef *QSPIHandle,uint8_t regN,uint8_t regVal);
uint8_t W25Q256FV_QSPI_WriteDefaultStatusREGs(QSPI_HandleTypeDef *QSPIHandle);
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif


