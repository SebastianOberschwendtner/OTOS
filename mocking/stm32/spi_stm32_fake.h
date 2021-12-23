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

#ifndef SPI_STM32_FAKE_H_
#define SPI_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface                         */
/*                                                                            */
/******************************************************************************/
#define SPI_I2S_FULLDUPLEX_SUPPORT /*!< I2S Full-Duplex support */

/*******************  Bit definition for SPI_CR1 register  ********************/
#define SPI_CR1_CPHA_Pos (0U)
#define SPI_CR1_CPHA_Msk (0x1UL << SPI_CR1_CPHA_Pos) /*!< 0x00000001 */
#define SPI_CR1_CPHA SPI_CR1_CPHA_Msk                /*!<Clock Phase      */
#define SPI_CR1_CPOL_Pos (1U)
#define SPI_CR1_CPOL_Msk (0x1UL << SPI_CR1_CPOL_Pos) /*!< 0x00000002 */
#define SPI_CR1_CPOL SPI_CR1_CPOL_Msk                /*!<Clock Polarity   */
#define SPI_CR1_MSTR_Pos (2U)
#define SPI_CR1_MSTR_Msk (0x1UL << SPI_CR1_MSTR_Pos) /*!< 0x00000004 */
#define SPI_CR1_MSTR SPI_CR1_MSTR_Msk                /*!<Master Selection */

#define SPI_CR1_BR_Pos (3U)
#define SPI_CR1_BR_Msk (0x7UL << SPI_CR1_BR_Pos) /*!< 0x00000038 */
#define SPI_CR1_BR SPI_CR1_BR_Msk                /*!<BR[2:0] bits (Baud Rate Control) */
#define SPI_CR1_BR_0 (0x1UL << SPI_CR1_BR_Pos)   /*!< 0x00000008 */
#define SPI_CR1_BR_1 (0x2UL << SPI_CR1_BR_Pos)   /*!< 0x00000010 */
#define SPI_CR1_BR_2 (0x4UL << SPI_CR1_BR_Pos)   /*!< 0x00000020 */

#define SPI_CR1_SPE_Pos (6U)
#define SPI_CR1_SPE_Msk (0x1UL << SPI_CR1_SPE_Pos) /*!< 0x00000040 */
#define SPI_CR1_SPE SPI_CR1_SPE_Msk                /*!<SPI Enable                          */
#define SPI_CR1_LSBFIRST_Pos (7U)
#define SPI_CR1_LSBFIRST_Msk (0x1UL << SPI_CR1_LSBFIRST_Pos) /*!< 0x00000080 */
#define SPI_CR1_LSBFIRST SPI_CR1_LSBFIRST_Msk                /*!<Frame Format                        */
#define SPI_CR1_SSI_Pos (8U)
#define SPI_CR1_SSI_Msk (0x1UL << SPI_CR1_SSI_Pos) /*!< 0x00000100 */
#define SPI_CR1_SSI SPI_CR1_SSI_Msk                /*!<Internal slave select               */
#define SPI_CR1_SSM_Pos (9U)
#define SPI_CR1_SSM_Msk (0x1UL << SPI_CR1_SSM_Pos) /*!< 0x00000200 */
#define SPI_CR1_SSM SPI_CR1_SSM_Msk                /*!<Software slave management           */
#define SPI_CR1_RXONLY_Pos (10U)
#define SPI_CR1_RXONLY_Msk (0x1UL << SPI_CR1_RXONLY_Pos) /*!< 0x00000400 */
#define SPI_CR1_RXONLY SPI_CR1_RXONLY_Msk                /*!<Receive only                        */
#define SPI_CR1_DFF_Pos (11U)
#define SPI_CR1_DFF_Msk (0x1UL << SPI_CR1_DFF_Pos) /*!< 0x00000800 */
#define SPI_CR1_DFF SPI_CR1_DFF_Msk                /*!<Data Frame Format                   */
#define SPI_CR1_CRCNEXT_Pos (12U)
#define SPI_CR1_CRCNEXT_Msk (0x1UL << SPI_CR1_CRCNEXT_Pos) /*!< 0x00001000 */
#define SPI_CR1_CRCNEXT SPI_CR1_CRCNEXT_Msk                /*!<Transmit CRC next                   */
#define SPI_CR1_CRCEN_Pos (13U)
#define SPI_CR1_CRCEN_Msk (0x1UL << SPI_CR1_CRCEN_Pos) /*!< 0x00002000 */
#define SPI_CR1_CRCEN SPI_CR1_CRCEN_Msk                /*!<Hardware CRC calculation enable     */
#define SPI_CR1_BIDIOE_Pos (14U)
#define SPI_CR1_BIDIOE_Msk (0x1UL << SPI_CR1_BIDIOE_Pos) /*!< 0x00004000 */
#define SPI_CR1_BIDIOE SPI_CR1_BIDIOE_Msk                /*!<Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE_Pos (15U)
#define SPI_CR1_BIDIMODE_Msk (0x1UL << SPI_CR1_BIDIMODE_Pos) /*!< 0x00008000 */
#define SPI_CR1_BIDIMODE SPI_CR1_BIDIMODE_Msk                /*!<Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define SPI_CR2_RXDMAEN_Pos (0U)
#define SPI_CR2_RXDMAEN_Msk (0x1UL << SPI_CR2_RXDMAEN_Pos) /*!< 0x00000001 */
#define SPI_CR2_RXDMAEN SPI_CR2_RXDMAEN_Msk                /*!<Rx Buffer DMA Enable                 */
#define SPI_CR2_TXDMAEN_Pos (1U)
#define SPI_CR2_TXDMAEN_Msk (0x1UL << SPI_CR2_TXDMAEN_Pos) /*!< 0x00000002 */
#define SPI_CR2_TXDMAEN SPI_CR2_TXDMAEN_Msk                /*!<Tx Buffer DMA Enable                 */
#define SPI_CR2_SSOE_Pos (2U)
#define SPI_CR2_SSOE_Msk (0x1UL << SPI_CR2_SSOE_Pos) /*!< 0x00000004 */
#define SPI_CR2_SSOE SPI_CR2_SSOE_Msk                /*!<SS Output Enable                     */
#define SPI_CR2_FRF_Pos (4U)
#define SPI_CR2_FRF_Msk (0x1UL << SPI_CR2_FRF_Pos) /*!< 0x00000010 */
#define SPI_CR2_FRF SPI_CR2_FRF_Msk                /*!<Frame Format                         */
#define SPI_CR2_ERRIE_Pos (5U)
#define SPI_CR2_ERRIE_Msk (0x1UL << SPI_CR2_ERRIE_Pos) /*!< 0x00000020 */
#define SPI_CR2_ERRIE SPI_CR2_ERRIE_Msk                /*!<Error Interrupt Enable               */
#define SPI_CR2_RXNEIE_Pos (6U)
#define SPI_CR2_RXNEIE_Msk (0x1UL << SPI_CR2_RXNEIE_Pos) /*!< 0x00000040 */
#define SPI_CR2_RXNEIE SPI_CR2_RXNEIE_Msk                /*!<RX buffer Not Empty Interrupt Enable */
#define SPI_CR2_TXEIE_Pos (7U)
#define SPI_CR2_TXEIE_Msk (0x1UL << SPI_CR2_TXEIE_Pos) /*!< 0x00000080 */
#define SPI_CR2_TXEIE SPI_CR2_TXEIE_Msk                /*!<Tx buffer Empty Interrupt Enable     */

/********************  Bit definition for SPI_SR register  ********************/
#define SPI_SR_RXNE_Pos (0U)
#define SPI_SR_RXNE_Msk (0x1UL << SPI_SR_RXNE_Pos) /*!< 0x00000001 */
#define SPI_SR_RXNE SPI_SR_RXNE_Msk                /*!<Receive buffer Not Empty */
#define SPI_SR_TXE_Pos (1U)
#define SPI_SR_TXE_Msk (0x1UL << SPI_SR_TXE_Pos) /*!< 0x00000002 */
#define SPI_SR_TXE SPI_SR_TXE_Msk                /*!<Transmit buffer Empty    */
#define SPI_SR_CHSIDE_Pos (2U)
#define SPI_SR_CHSIDE_Msk (0x1UL << SPI_SR_CHSIDE_Pos) /*!< 0x00000004 */
#define SPI_SR_CHSIDE SPI_SR_CHSIDE_Msk                /*!<Channel side             */
#define SPI_SR_UDR_Pos (3U)
#define SPI_SR_UDR_Msk (0x1UL << SPI_SR_UDR_Pos) /*!< 0x00000008 */
#define SPI_SR_UDR SPI_SR_UDR_Msk                /*!<Underrun flag            */
#define SPI_SR_CRCERR_Pos (4U)
#define SPI_SR_CRCERR_Msk (0x1UL << SPI_SR_CRCERR_Pos) /*!< 0x00000010 */
#define SPI_SR_CRCERR SPI_SR_CRCERR_Msk                /*!<CRC Error flag           */
#define SPI_SR_MODF_Pos (5U)
#define SPI_SR_MODF_Msk (0x1UL << SPI_SR_MODF_Pos) /*!< 0x00000020 */
#define SPI_SR_MODF SPI_SR_MODF_Msk                /*!<Mode fault               */
#define SPI_SR_OVR_Pos (6U)
#define SPI_SR_OVR_Msk (0x1UL << SPI_SR_OVR_Pos) /*!< 0x00000040 */
#define SPI_SR_OVR SPI_SR_OVR_Msk                /*!<Overrun flag             */
#define SPI_SR_BSY_Pos (7U)
#define SPI_SR_BSY_Msk (0x1UL << SPI_SR_BSY_Pos) /*!< 0x00000080 */
#define SPI_SR_BSY SPI_SR_BSY_Msk                /*!<Busy flag                */
#define SPI_SR_FRE_Pos (8U)
#define SPI_SR_FRE_Msk (0x1UL << SPI_SR_FRE_Pos) /*!< 0x00000100 */
#define SPI_SR_FRE SPI_SR_FRE_Msk                /*!<Frame format error flag  */

/********************  Bit definition for SPI_DR register  ********************/
#define SPI_DR_DR_Pos (0U)
#define SPI_DR_DR_Msk (0xFFFFUL << SPI_DR_DR_Pos) /*!< 0x0000FFFF */
#define SPI_DR_DR SPI_DR_DR_Msk                   /*!<Data Register           */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define SPI_CRCPR_CRCPOLY_Pos (0U)
#define SPI_CRCPR_CRCPOLY_Msk (0xFFFFUL << SPI_CRCPR_CRCPOLY_Pos) /*!< 0x0000FFFF */
#define SPI_CRCPR_CRCPOLY SPI_CRCPR_CRCPOLY_Msk                   /*!<CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define SPI_RXCRCR_RXCRC_Pos (0U)
#define SPI_RXCRCR_RXCRC_Msk (0xFFFFUL << SPI_RXCRCR_RXCRC_Pos) /*!< 0x0000FFFF */
#define SPI_RXCRCR_RXCRC SPI_RXCRCR_RXCRC_Msk                   /*!<Rx CRC Register         */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define SPI_TXCRCR_TXCRC_Pos (0U)
#define SPI_TXCRCR_TXCRC_Msk (0xFFFFUL << SPI_TXCRCR_TXCRC_Pos) /*!< 0x0000FFFF */
#define SPI_TXCRCR_TXCRC SPI_TXCRCR_TXCRC_Msk                   /*!<Tx CRC Register         */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define SPI_I2SCFGR_CHLEN_Pos (0U)
#define SPI_I2SCFGR_CHLEN_Msk (0x1UL << SPI_I2SCFGR_CHLEN_Pos) /*!< 0x00000001 */
#define SPI_I2SCFGR_CHLEN SPI_I2SCFGR_CHLEN_Msk                /*!<Channel length (number of bits per audio channel) */

#define SPI_I2SCFGR_DATLEN_Pos (1U)
#define SPI_I2SCFGR_DATLEN_Msk (0x3UL << SPI_I2SCFGR_DATLEN_Pos) /*!< 0x00000006 */
#define SPI_I2SCFGR_DATLEN SPI_I2SCFGR_DATLEN_Msk                /*!<DATLEN[1:0] bits (Data length to be transferred)  */
#define SPI_I2SCFGR_DATLEN_0 (0x1UL << SPI_I2SCFGR_DATLEN_Pos)   /*!< 0x00000002 */
#define SPI_I2SCFGR_DATLEN_1 (0x2UL << SPI_I2SCFGR_DATLEN_Pos)   /*!< 0x00000004 */

#define SPI_I2SCFGR_CKPOL_Pos (3U)
#define SPI_I2SCFGR_CKPOL_Msk (0x1UL << SPI_I2SCFGR_CKPOL_Pos) /*!< 0x00000008 */
#define SPI_I2SCFGR_CKPOL SPI_I2SCFGR_CKPOL_Msk                /*!<steady state clock polarity               */

#define SPI_I2SCFGR_I2SSTD_Pos (4U)
#define SPI_I2SCFGR_I2SSTD_Msk (0x3UL << SPI_I2SCFGR_I2SSTD_Pos) /*!< 0x00000030 */
#define SPI_I2SCFGR_I2SSTD SPI_I2SCFGR_I2SSTD_Msk                /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define SPI_I2SCFGR_I2SSTD_0 (0x1UL << SPI_I2SCFGR_I2SSTD_Pos)   /*!< 0x00000010 */
#define SPI_I2SCFGR_I2SSTD_1 (0x2UL << SPI_I2SCFGR_I2SSTD_Pos)   /*!< 0x00000020 */

#define SPI_I2SCFGR_PCMSYNC_Pos (7U)
#define SPI_I2SCFGR_PCMSYNC_Msk (0x1UL << SPI_I2SCFGR_PCMSYNC_Pos) /*!< 0x00000080 */
#define SPI_I2SCFGR_PCMSYNC SPI_I2SCFGR_PCMSYNC_Msk                /*!<PCM frame synchronization                 */

#define SPI_I2SCFGR_I2SCFG_Pos (8U)
#define SPI_I2SCFGR_I2SCFG_Msk (0x3UL << SPI_I2SCFGR_I2SCFG_Pos) /*!< 0x00000300 */
#define SPI_I2SCFGR_I2SCFG SPI_I2SCFGR_I2SCFG_Msk                /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define SPI_I2SCFGR_I2SCFG_0 (0x1UL << SPI_I2SCFGR_I2SCFG_Pos)   /*!< 0x00000100 */
#define SPI_I2SCFGR_I2SCFG_1 (0x2UL << SPI_I2SCFGR_I2SCFG_Pos)   /*!< 0x00000200 */

#define SPI_I2SCFGR_I2SE_Pos (10U)
#define SPI_I2SCFGR_I2SE_Msk (0x1UL << SPI_I2SCFGR_I2SE_Pos) /*!< 0x00000400 */
#define SPI_I2SCFGR_I2SE SPI_I2SCFGR_I2SE_Msk                /*!<I2S Enable         */
#define SPI_I2SCFGR_I2SMOD_Pos (11U)
#define SPI_I2SCFGR_I2SMOD_Msk (0x1UL << SPI_I2SCFGR_I2SMOD_Pos) /*!< 0x00000800 */
#define SPI_I2SCFGR_I2SMOD SPI_I2SCFGR_I2SMOD_Msk                /*!<I2S mode selection */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define SPI_I2SPR_I2SDIV_Pos (0U)
#define SPI_I2SPR_I2SDIV_Msk (0xFFUL << SPI_I2SPR_I2SDIV_Pos) /*!< 0x000000FF */
#define SPI_I2SPR_I2SDIV SPI_I2SPR_I2SDIV_Msk                 /*!<I2S Linear prescaler         */
#define SPI_I2SPR_ODD_Pos (8U)
#define SPI_I2SPR_ODD_Msk (0x1UL << SPI_I2SPR_ODD_Pos) /*!< 0x00000100 */
#define SPI_I2SPR_ODD SPI_I2SPR_ODD_Msk                /*!<Odd factor for the prescaler */
#define SPI_I2SPR_MCKOE_Pos (9U)
#define SPI_I2SPR_MCKOE_Msk (0x1UL << SPI_I2SPR_MCKOE_Pos) /*!< 0x00000200 */
#define SPI_I2SPR_MCKOE SPI_I2SPR_MCKOE_Msk                /*!<Master Clock Output Enable   */

// *** Class defines for fake peripherals
/** 
  * @brief Serial Peripheral Interface
  */

struct SPI_TypeDef : public Fake::Peripheral
{
    // Constructor
    SPI_TypeDef();

    // Methods for unit testing
    void registers_to_default(void) override;

    // Fake Peripheral registers
    Fake::Register_t CR1;     /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
    Fake::Register_t CR2;     /*!< SPI control register 2,                             Address offset: 0x04 */
    Fake::Register_t SR;      /*!< SPI status register,                                Address offset: 0x08 */
    Fake::Register_t DR;      /*!< SPI data register,                                  Address offset: 0x0C */
    Fake::Register_t CRCPR;   /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
    Fake::Register_t RXCRCR;  /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
    Fake::Register_t TXCRCR;  /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
    Fake::Register_t I2SCFGR; /*!< SPI_I2S configuration register,                     Address offset: 0x1C */
    Fake::Register_t I2SPR;   /*!< SPI_I2S prescaler register,                         Address offset: 0x20 */
};

// *** Public references to fake peripherals
// Public pointer to SPI peripherals which are faked
extern SPI_TypeDef *SPI1;
extern SPI_TypeDef *SPI2;
extern SPI_TypeDef *SPI3;
extern SPI_TypeDef *SPI5;
extern SPI_TypeDef *SPI5;
extern SPI_TypeDef *SPI6;
extern unsigned long SPI1_BASE;
extern unsigned long SPI2_BASE;
extern unsigned long SPI3_BASE;
extern unsigned long SPI4_BASE;
extern unsigned long SPI5_BASE;
extern unsigned long SPI6_BASE;

#endif