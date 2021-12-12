/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @attention
 * All the peripheral type definition are taken from the peripheral library
 * files provided by ST.
 * 
 * Copyright:
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 */

#ifndef CLOCK_STM32_FAKE_H_
#define CLOCK_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"

// *** Class defines for fake peripherals

/** 
  * @brief Reset and Clock Control
  */
class RCC_TypeDef: public Fake::Peripheral
{
private:

public:
  // Constructor which initializes peripheral with default values
  RCC_TypeDef();

  // Methods for unit testing
  void registers_to_default (void) override;

  // Fake Peripheral registers
  Fake::Register_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  Fake::Register_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  Fake::Register_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  Fake::Register_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  Fake::Register_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  Fake::Register_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  Fake::Register_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  Fake::Register_t RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  Fake::Register_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  Fake::Register_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  Fake::Register_t RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  Fake::Register_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  Fake::Register_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  Fake::Register_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  Fake::Register_t RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  Fake::Register_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  Fake::Register_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  Fake::Register_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  Fake::Register_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  Fake::Register_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  Fake::Register_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  Fake::Register_t RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  Fake::Register_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  Fake::Register_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  Fake::Register_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  Fake::Register_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  Fake::Register_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  Fake::Register_t RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  Fake::Register_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  Fake::Register_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
  Fake::Register_t PLLSAICFGR;    /*!< RCC PLLSAI configuration register,                           Address offset: 0x88 */
  Fake::Register_t DCKCFGR;       /*!< RCC Dedicated Clocks configuration register,                 Address offset: 0x8C */
};

// *** Public references to fake peripherals
// Public pointer to RCC peripheral which is faked
extern RCC_TypeDef* RCC;
extern unsigned long RCC_BASE;

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define RCC_APB1ENR_TIM2EN_Pos             (0U)                                
#define RCC_APB1ENR_TIM2EN_Msk             (0x1UL << RCC_APB1ENR_TIM2EN_Pos)    /*!< 0x00000001 */
#define RCC_APB1ENR_TIM2EN                 RCC_APB1ENR_TIM2EN_Msk              
#define RCC_APB1ENR_TIM3EN_Pos             (1U)                                
#define RCC_APB1ENR_TIM3EN_Msk             (0x1UL << RCC_APB1ENR_TIM3EN_Pos)    /*!< 0x00000002 */
#define RCC_APB1ENR_TIM3EN                 RCC_APB1ENR_TIM3EN_Msk              
#define RCC_APB1ENR_TIM4EN_Pos             (2U)                                
#define RCC_APB1ENR_TIM4EN_Msk             (0x1UL << RCC_APB1ENR_TIM4EN_Pos)    /*!< 0x00000004 */
#define RCC_APB1ENR_TIM4EN                 RCC_APB1ENR_TIM4EN_Msk              
#define RCC_APB1ENR_TIM5EN_Pos             (3U)                                
#define RCC_APB1ENR_TIM5EN_Msk             (0x1UL << RCC_APB1ENR_TIM5EN_Pos)    /*!< 0x00000008 */
#define RCC_APB1ENR_TIM5EN                 RCC_APB1ENR_TIM5EN_Msk              
#define RCC_APB1ENR_TIM6EN_Pos             (4U)                                
#define RCC_APB1ENR_TIM6EN_Msk             (0x1UL << RCC_APB1ENR_TIM6EN_Pos)    /*!< 0x00000010 */
#define RCC_APB1ENR_TIM6EN                 RCC_APB1ENR_TIM6EN_Msk              
#define RCC_APB1ENR_TIM7EN_Pos             (5U)                                
#define RCC_APB1ENR_TIM7EN_Msk             (0x1UL << RCC_APB1ENR_TIM7EN_Pos)    /*!< 0x00000020 */
#define RCC_APB1ENR_TIM7EN                 RCC_APB1ENR_TIM7EN_Msk              
#define RCC_APB1ENR_TIM12EN_Pos            (6U)                                
#define RCC_APB1ENR_TIM12EN_Msk            (0x1UL << RCC_APB1ENR_TIM12EN_Pos)   /*!< 0x00000040 */
#define RCC_APB1ENR_TIM12EN                RCC_APB1ENR_TIM12EN_Msk             
#define RCC_APB1ENR_TIM13EN_Pos            (7U)                                
#define RCC_APB1ENR_TIM13EN_Msk            (0x1UL << RCC_APB1ENR_TIM13EN_Pos)   /*!< 0x00000080 */
#define RCC_APB1ENR_TIM13EN                RCC_APB1ENR_TIM13EN_Msk             
#define RCC_APB1ENR_TIM14EN_Pos            (8U)                                
#define RCC_APB1ENR_TIM14EN_Msk            (0x1UL << RCC_APB1ENR_TIM14EN_Pos)   /*!< 0x00000100 */
#define RCC_APB1ENR_TIM14EN                RCC_APB1ENR_TIM14EN_Msk             
#define RCC_APB1ENR_WWDGEN_Pos             (11U)                               
#define RCC_APB1ENR_WWDGEN_Msk             (0x1UL << RCC_APB1ENR_WWDGEN_Pos)    /*!< 0x00000800 */
#define RCC_APB1ENR_WWDGEN                 RCC_APB1ENR_WWDGEN_Msk              
#define RCC_APB1ENR_SPI2EN_Pos             (14U)                               
#define RCC_APB1ENR_SPI2EN_Msk             (0x1UL << RCC_APB1ENR_SPI2EN_Pos)    /*!< 0x00004000 */
#define RCC_APB1ENR_SPI2EN                 RCC_APB1ENR_SPI2EN_Msk              
#define RCC_APB1ENR_SPI3EN_Pos             (15U)                               
#define RCC_APB1ENR_SPI3EN_Msk             (0x1UL << RCC_APB1ENR_SPI3EN_Pos)    /*!< 0x00008000 */
#define RCC_APB1ENR_SPI3EN                 RCC_APB1ENR_SPI3EN_Msk              
#define RCC_APB1ENR_USART2EN_Pos           (17U)                               
#define RCC_APB1ENR_USART2EN_Msk           (0x1UL << RCC_APB1ENR_USART2EN_Pos)  /*!< 0x00020000 */
#define RCC_APB1ENR_USART2EN               RCC_APB1ENR_USART2EN_Msk            
#define RCC_APB1ENR_USART3EN_Pos           (18U)                               
#define RCC_APB1ENR_USART3EN_Msk           (0x1UL << RCC_APB1ENR_USART3EN_Pos)  /*!< 0x00040000 */
#define RCC_APB1ENR_USART3EN               RCC_APB1ENR_USART3EN_Msk            
#define RCC_APB1ENR_UART4EN_Pos            (19U)                               
#define RCC_APB1ENR_UART4EN_Msk            (0x1UL << RCC_APB1ENR_UART4EN_Pos)   /*!< 0x00080000 */
#define RCC_APB1ENR_UART4EN                RCC_APB1ENR_UART4EN_Msk             
#define RCC_APB1ENR_UART5EN_Pos            (20U)                               
#define RCC_APB1ENR_UART5EN_Msk            (0x1UL << RCC_APB1ENR_UART5EN_Pos)   /*!< 0x00100000 */
#define RCC_APB1ENR_UART5EN                RCC_APB1ENR_UART5EN_Msk             
#define RCC_APB1ENR_I2C1EN_Pos             (21U)                               
#define RCC_APB1ENR_I2C1EN_Msk             (0x1UL << RCC_APB1ENR_I2C1EN_Pos)    /*!< 0x00200000 */
#define RCC_APB1ENR_I2C1EN                 RCC_APB1ENR_I2C1EN_Msk              
#define RCC_APB1ENR_I2C2EN_Pos             (22U)                               
#define RCC_APB1ENR_I2C2EN_Msk             (0x1UL << RCC_APB1ENR_I2C2EN_Pos)    /*!< 0x00400000 */
#define RCC_APB1ENR_I2C2EN                 RCC_APB1ENR_I2C2EN_Msk              
#define RCC_APB1ENR_I2C3EN_Pos             (23U)                               
#define RCC_APB1ENR_I2C3EN_Msk             (0x1UL << RCC_APB1ENR_I2C3EN_Pos)    /*!< 0x00800000 */
#define RCC_APB1ENR_I2C3EN                 RCC_APB1ENR_I2C3EN_Msk              
#define RCC_APB1ENR_CAN1EN_Pos             (25U)                               
#define RCC_APB1ENR_CAN1EN_Msk             (0x1UL << RCC_APB1ENR_CAN1EN_Pos)    /*!< 0x02000000 */
#define RCC_APB1ENR_CAN1EN                 RCC_APB1ENR_CAN1EN_Msk              
#define RCC_APB1ENR_CAN2EN_Pos             (26U)                               
#define RCC_APB1ENR_CAN2EN_Msk             (0x1UL << RCC_APB1ENR_CAN2EN_Pos)    /*!< 0x04000000 */
#define RCC_APB1ENR_CAN2EN                 RCC_APB1ENR_CAN2EN_Msk              
#define RCC_APB1ENR_PWREN_Pos              (28U)                               
#define RCC_APB1ENR_PWREN_Msk              (0x1UL << RCC_APB1ENR_PWREN_Pos)     /*!< 0x10000000 */
#define RCC_APB1ENR_PWREN                  RCC_APB1ENR_PWREN_Msk               
#define RCC_APB1ENR_DACEN_Pos              (29U)                               
#define RCC_APB1ENR_DACEN_Msk              (0x1UL << RCC_APB1ENR_DACEN_Pos)     /*!< 0x20000000 */
#define RCC_APB1ENR_DACEN                  RCC_APB1ENR_DACEN_Msk               
#define RCC_APB1ENR_UART7EN_Pos            (30U)                               
#define RCC_APB1ENR_UART7EN_Msk            (0x1UL << RCC_APB1ENR_UART7EN_Pos)   /*!< 0x40000000 */
#define RCC_APB1ENR_UART7EN                RCC_APB1ENR_UART7EN_Msk             
#define RCC_APB1ENR_UART8EN_Pos            (31U)                               
#define RCC_APB1ENR_UART8EN_Msk            (0x1UL << RCC_APB1ENR_UART8EN_Pos)   /*!< 0x80000000 */
#define RCC_APB1ENR_UART8EN                RCC_APB1ENR_UART8EN_Msk             

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define RCC_APB2ENR_TIM1EN_Pos             (0U)                                
#define RCC_APB2ENR_TIM1EN_Msk             (0x1UL << RCC_APB2ENR_TIM1EN_Pos)    /*!< 0x00000001 */
#define RCC_APB2ENR_TIM1EN                 RCC_APB2ENR_TIM1EN_Msk              
#define RCC_APB2ENR_TIM8EN_Pos             (1U)                                
#define RCC_APB2ENR_TIM8EN_Msk             (0x1UL << RCC_APB2ENR_TIM8EN_Pos)    /*!< 0x00000002 */
#define RCC_APB2ENR_TIM8EN                 RCC_APB2ENR_TIM8EN_Msk              
#define RCC_APB2ENR_USART1EN_Pos           (4U)                                
#define RCC_APB2ENR_USART1EN_Msk           (0x1UL << RCC_APB2ENR_USART1EN_Pos)  /*!< 0x00000010 */
#define RCC_APB2ENR_USART1EN               RCC_APB2ENR_USART1EN_Msk            
#define RCC_APB2ENR_USART6EN_Pos           (5U)                                
#define RCC_APB2ENR_USART6EN_Msk           (0x1UL << RCC_APB2ENR_USART6EN_Pos)  /*!< 0x00000020 */
#define RCC_APB2ENR_USART6EN               RCC_APB2ENR_USART6EN_Msk            
#define RCC_APB2ENR_ADC1EN_Pos             (8U)                                
#define RCC_APB2ENR_ADC1EN_Msk             (0x1UL << RCC_APB2ENR_ADC1EN_Pos)    /*!< 0x00000100 */
#define RCC_APB2ENR_ADC1EN                 RCC_APB2ENR_ADC1EN_Msk              
#define RCC_APB2ENR_ADC2EN_Pos             (9U)                                
#define RCC_APB2ENR_ADC2EN_Msk             (0x1UL << RCC_APB2ENR_ADC2EN_Pos)    /*!< 0x00000200 */
#define RCC_APB2ENR_ADC2EN                 RCC_APB2ENR_ADC2EN_Msk              
#define RCC_APB2ENR_ADC3EN_Pos             (10U)                               
#define RCC_APB2ENR_ADC3EN_Msk             (0x1UL << RCC_APB2ENR_ADC3EN_Pos)    /*!< 0x00000400 */
#define RCC_APB2ENR_ADC3EN                 RCC_APB2ENR_ADC3EN_Msk              
#define RCC_APB2ENR_SDIOEN_Pos             (11U)                               
#define RCC_APB2ENR_SDIOEN_Msk             (0x1UL << RCC_APB2ENR_SDIOEN_Pos)    /*!< 0x00000800 */
#define RCC_APB2ENR_SDIOEN                 RCC_APB2ENR_SDIOEN_Msk              
#define RCC_APB2ENR_SPI1EN_Pos             (12U)                               
#define RCC_APB2ENR_SPI1EN_Msk             (0x1UL << RCC_APB2ENR_SPI1EN_Pos)    /*!< 0x00001000 */
#define RCC_APB2ENR_SPI1EN                 RCC_APB2ENR_SPI1EN_Msk              
#define RCC_APB2ENR_SPI4EN_Pos             (13U)                               
#define RCC_APB2ENR_SPI4EN_Msk             (0x1UL << RCC_APB2ENR_SPI4EN_Pos)    /*!< 0x00002000 */
#define RCC_APB2ENR_SPI4EN                 RCC_APB2ENR_SPI4EN_Msk              
#define RCC_APB2ENR_SYSCFGEN_Pos           (14U)                               
#define RCC_APB2ENR_SYSCFGEN_Msk           (0x1UL << RCC_APB2ENR_SYSCFGEN_Pos)  /*!< 0x00004000 */
#define RCC_APB2ENR_SYSCFGEN               RCC_APB2ENR_SYSCFGEN_Msk            
#define RCC_APB2ENR_TIM9EN_Pos             (16U)                               
#define RCC_APB2ENR_TIM9EN_Msk             (0x1UL << RCC_APB2ENR_TIM9EN_Pos)    /*!< 0x00010000 */
#define RCC_APB2ENR_TIM9EN                 RCC_APB2ENR_TIM9EN_Msk              
#define RCC_APB2ENR_TIM10EN_Pos            (17U)                               
#define RCC_APB2ENR_TIM10EN_Msk            (0x1UL << RCC_APB2ENR_TIM10EN_Pos)   /*!< 0x00020000 */
#define RCC_APB2ENR_TIM10EN                RCC_APB2ENR_TIM10EN_Msk             
#define RCC_APB2ENR_TIM11EN_Pos            (18U)                               
#define RCC_APB2ENR_TIM11EN_Msk            (0x1UL << RCC_APB2ENR_TIM11EN_Pos)   /*!< 0x00040000 */
#define RCC_APB2ENR_TIM11EN                RCC_APB2ENR_TIM11EN_Msk             
#define RCC_APB2ENR_SPI5EN_Pos             (20U)                               
#define RCC_APB2ENR_SPI5EN_Msk             (0x1UL << RCC_APB2ENR_SPI5EN_Pos)    /*!< 0x00100000 */
#define RCC_APB2ENR_SPI5EN                 RCC_APB2ENR_SPI5EN_Msk              
#define RCC_APB2ENR_SPI6EN_Pos             (21U)                               
#define RCC_APB2ENR_SPI6EN_Msk             (0x1UL << RCC_APB2ENR_SPI6EN_Pos)    /*!< 0x00200000 */
#define RCC_APB2ENR_SPI6EN                 RCC_APB2ENR_SPI6EN_Msk              
#define RCC_APB2ENR_SAI1EN_Pos             (22U)                               
#define RCC_APB2ENR_SAI1EN_Msk             (0x1UL << RCC_APB2ENR_SAI1EN_Pos)    /*!< 0x00400000 */
#define RCC_APB2ENR_SAI1EN                 RCC_APB2ENR_SAI1EN_Msk              
#define RCC_APB2ENR_LTDCEN_Pos             (26U)                               
#define RCC_APB2ENR_LTDCEN_Msk             (0x1UL << RCC_APB2ENR_LTDCEN_Pos)    /*!< 0x04000000 */
#define RCC_APB2ENR_LTDCEN                 RCC_APB2ENR_LTDCEN_Msk              

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define RCC_APB2LPENR_TIM1LPEN_Pos         (0U)                                
#define RCC_APB2LPENR_TIM1LPEN_Msk         (0x1UL << RCC_APB2LPENR_TIM1LPEN_Pos) /*!< 0x00000001 */
#define RCC_APB2LPENR_TIM1LPEN             RCC_APB2LPENR_TIM1LPEN_Msk          
#define RCC_APB2LPENR_TIM8LPEN_Pos         (1U)                                
#define RCC_APB2LPENR_TIM8LPEN_Msk         (0x1UL << RCC_APB2LPENR_TIM8LPEN_Pos) /*!< 0x00000002 */
#define RCC_APB2LPENR_TIM8LPEN             RCC_APB2LPENR_TIM8LPEN_Msk          
#define RCC_APB2LPENR_USART1LPEN_Pos       (4U)                                
#define RCC_APB2LPENR_USART1LPEN_Msk       (0x1UL << RCC_APB2LPENR_USART1LPEN_Pos) /*!< 0x00000010 */
#define RCC_APB2LPENR_USART1LPEN           RCC_APB2LPENR_USART1LPEN_Msk        
#define RCC_APB2LPENR_USART6LPEN_Pos       (5U)                                
#define RCC_APB2LPENR_USART6LPEN_Msk       (0x1UL << RCC_APB2LPENR_USART6LPEN_Pos) /*!< 0x00000020 */
#define RCC_APB2LPENR_USART6LPEN           RCC_APB2LPENR_USART6LPEN_Msk        
#define RCC_APB2LPENR_ADC1LPEN_Pos         (8U)                                
#define RCC_APB2LPENR_ADC1LPEN_Msk         (0x1UL << RCC_APB2LPENR_ADC1LPEN_Pos) /*!< 0x00000100 */
#define RCC_APB2LPENR_ADC1LPEN             RCC_APB2LPENR_ADC1LPEN_Msk          
#define RCC_APB2LPENR_ADC2LPEN_Pos         (9U)                                
#define RCC_APB2LPENR_ADC2LPEN_Msk         (0x1UL << RCC_APB2LPENR_ADC2LPEN_Pos) /*!< 0x00000200 */
#define RCC_APB2LPENR_ADC2LPEN             RCC_APB2LPENR_ADC2LPEN_Msk          
#define RCC_APB2LPENR_ADC3LPEN_Pos         (10U)                               
#define RCC_APB2LPENR_ADC3LPEN_Msk         (0x1UL << RCC_APB2LPENR_ADC3LPEN_Pos) /*!< 0x00000400 */
#define RCC_APB2LPENR_ADC3LPEN             RCC_APB2LPENR_ADC3LPEN_Msk          
#define RCC_APB2LPENR_SDIOLPEN_Pos         (11U)                               
#define RCC_APB2LPENR_SDIOLPEN_Msk         (0x1UL << RCC_APB2LPENR_SDIOLPEN_Pos) /*!< 0x00000800 */
#define RCC_APB2LPENR_SDIOLPEN             RCC_APB2LPENR_SDIOLPEN_Msk          
#define RCC_APB2LPENR_SPI1LPEN_Pos         (12U)                               
#define RCC_APB2LPENR_SPI1LPEN_Msk         (0x1UL << RCC_APB2LPENR_SPI1LPEN_Pos) /*!< 0x00001000 */
#define RCC_APB2LPENR_SPI1LPEN             RCC_APB2LPENR_SPI1LPEN_Msk          
#define RCC_APB2LPENR_SPI4LPEN_Pos         (13U)                               
#define RCC_APB2LPENR_SPI4LPEN_Msk         (0x1UL << RCC_APB2LPENR_SPI4LPEN_Pos) /*!< 0x00002000 */
#define RCC_APB2LPENR_SPI4LPEN             RCC_APB2LPENR_SPI4LPEN_Msk          
#define RCC_APB2LPENR_SYSCFGLPEN_Pos       (14U)                               
#define RCC_APB2LPENR_SYSCFGLPEN_Msk       (0x1UL << RCC_APB2LPENR_SYSCFGLPEN_Pos) /*!< 0x00004000 */
#define RCC_APB2LPENR_SYSCFGLPEN           RCC_APB2LPENR_SYSCFGLPEN_Msk        
#define RCC_APB2LPENR_TIM9LPEN_Pos         (16U)                               
#define RCC_APB2LPENR_TIM9LPEN_Msk         (0x1UL << RCC_APB2LPENR_TIM9LPEN_Pos) /*!< 0x00010000 */
#define RCC_APB2LPENR_TIM9LPEN             RCC_APB2LPENR_TIM9LPEN_Msk          
#define RCC_APB2LPENR_TIM10LPEN_Pos        (17U)                               
#define RCC_APB2LPENR_TIM10LPEN_Msk        (0x1UL << RCC_APB2LPENR_TIM10LPEN_Pos) /*!< 0x00020000 */
#define RCC_APB2LPENR_TIM10LPEN            RCC_APB2LPENR_TIM10LPEN_Msk         
#define RCC_APB2LPENR_TIM11LPEN_Pos        (18U)                               
#define RCC_APB2LPENR_TIM11LPEN_Msk        (0x1UL << RCC_APB2LPENR_TIM11LPEN_Pos) /*!< 0x00040000 */
#define RCC_APB2LPENR_TIM11LPEN            RCC_APB2LPENR_TIM11LPEN_Msk         
#define RCC_APB2LPENR_SPI5LPEN_Pos         (20U)                               
#define RCC_APB2LPENR_SPI5LPEN_Msk         (0x1UL << RCC_APB2LPENR_SPI5LPEN_Pos) /*!< 0x00100000 */
#define RCC_APB2LPENR_SPI5LPEN             RCC_APB2LPENR_SPI5LPEN_Msk          
#define RCC_APB2LPENR_SPI6LPEN_Pos         (21U)                               
#define RCC_APB2LPENR_SPI6LPEN_Msk         (0x1UL << RCC_APB2LPENR_SPI6LPEN_Pos) /*!< 0x00200000 */
#define RCC_APB2LPENR_SPI6LPEN             RCC_APB2LPENR_SPI6LPEN_Msk          
#define RCC_APB2LPENR_SAI1LPEN_Pos         (22U)                               
#define RCC_APB2LPENR_SAI1LPEN_Msk         (0x1UL << RCC_APB2LPENR_SAI1LPEN_Pos) /*!< 0x00400000 */
#define RCC_APB2LPENR_SAI1LPEN             RCC_APB2LPENR_SAI1LPEN_Msk          
#define RCC_APB2LPENR_LTDCLPEN_Pos         (26U)                               
#define RCC_APB2LPENR_LTDCLPEN_Msk         (0x1UL << RCC_APB2LPENR_LTDCLPEN_Pos) /*!< 0x04000000 */
#define RCC_APB2LPENR_LTDCLPEN             RCC_APB2LPENR_LTDCLPEN_Msk          
#endif