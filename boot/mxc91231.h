/*
 *  Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 *    - Platform specific register memory map
 *
 *  Copyright (C) 2006-2007 Motorola, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Date     Author    Comment
 * 10/2006  Motorola  Added phys to virt address mappings for Motorola specific
 *                    products.  Also added additional ethernet, LCD and GPIO
 *                    #defines as well as several WFN bug fixes.
 * 01/2007  Motorola  Added FIQ_START for watchdog debug support.
 * 04/2007  Motorola  Added the definition for 32KHz clock source.
 * 10/2007  Motorola  FIQ related added.
 */
#ifndef __ASM_ARM_ARCH_MXC91231_H_
#define __ASM_ARM_ARCH_MXC91231_H_


#ifdef CONFIG_MOT_FEAT_32KHZ_GPT
#define MXC_TIMER_CLK           32768
#else
#define MXC_TIMER_CLK		13000000
#endif /* CONFIG_MOT_FEAT_32KHZ_GPT */
#define MXC_TIMER_DIVIDER       1

/*!
 * Register an interrupt handler for the SMN as well as the SCC.  In some
 * implementations, the SMN is not connected at all, and in others, it is
 * on the same interrupt line as the SCM. Comment this line out accordingly
 */
#define USE_SMN_INTERRUPT

/*
 * This define specifies whether the muxed general purpose interrupt
 * lines from MU is integrated with the ARM core
 */
#define MU_MUX_GN_INTS          1

/*
 * UART Chip level Configuration that a user may not have to edit. These
 * configuration vary depending on how the UART module is integrated with
 * the ARM core
 */
#define MXC_UART_NR 3
/*!
 * This option is used to set or clear the RXDMUXSEL bit in control reg 3.
 * Certain platforms need this bit to be set in order to receive Irda data.
 */
#define MXC_UART_IR_RXDMUX      0x0004

/*!
 * This option is used to set or clear the RXDMUXSEL bit in control reg 3.
 * Certain platforms need this bit to be set in order to receive UART data.
 */
#define MXC_UART_RXDMUX         0

// end FIXME

/*
 * L2CC
 */
#define     L2CC_BASE_ADDR          0x30000000
#define     L2CC_BASE_ADDR_VIRT     0xD1000000
#define     L2CC_SIZE               SZ_64K

/*
 * AIPS 1
 */
#define     AIPS1_BASE_ADDR         0x43F00000
#define     AIPS1_BASE_ADDR_VIRT    0xD4000000
#define     AIPS1_SIZE              SZ_1M

#define     AIPS1_CTRL_BASE_ADDR    AIPS1_BASE_ADDR
#define     MAX_BASE_ADDR           (AIPS1_BASE_ADDR + 0x04000)
#define     EVTMON_BASE_ADDR        (AIPS1_BASE_ADDR + 0x08000)
#define     CLKCTL_BASE_ADDR        (AIPS1_BASE_ADDR + 0x0C000)
#define     ETB_SLOT4_BASE_ADDR     (AIPS1_BASE_ADDR + 0x10000)
#define     ETB_SLOT5_BASE_ADDR     (AIPS1_BASE_ADDR + 0x14000)
#define     ECT_CTIO_BASE_ADDR      (AIPS1_BASE_ADDR + 0x18000)
#define     I2C_BASE_ADDR           (AIPS1_BASE_ADDR + 0x80000)
#define     MU_BASE_ADDR            (AIPS1_BASE_ADDR + 0x88000)
#define     UART1_BASE_ADDR         (AIPS1_BASE_ADDR + 0x90000)
#define     UART2_BASE_ADDR         (AIPS1_BASE_ADDR + 0x94000)
#define     DSM_BASE_ADDR           (AIPS1_BASE_ADDR + 0x98000)
#define     OWIRE_BASE_ADDR         (AIPS1_BASE_ADDR + 0x9C000)
#define     SSI1_BASE_ADDR          (AIPS1_BASE_ADDR + 0xA0000)
#define     KPP_BASE_ADDR           (AIPS1_BASE_ADDR + 0xA8000)
#define     IOMUX_AP_BASE_ADDR      (AIPS1_BASE_ADDR + 0xAC000)
#define     CTI_AP_BASE_ADDR        (AIPS1_BASE_ADDR + 0xB8000)

/*
 * SPBA global module enabled #0
 */
#define     SPBA0_BASE_ADDR         0x50000000
#define     SPBA0_BASE_ADDR_VIRT    0xD4100000
#define     SPBA0_SIZE              SZ_1M

#define     MMC_SDHC1_BASE_ADDR     (SPBA0_BASE_ADDR + 0x04000)
#define     MMC_SDHC2_BASE_ADDR     (SPBA0_BASE_ADDR + 0x08000)
#define     UART3_BASE_ADDR         (SPBA0_BASE_ADDR + 0x0C000)
#define     CSPI2_BASE_ADDR         (SPBA0_BASE_ADDR + 0x10000)
#define     SSI2_BASE_ADDR          (SPBA0_BASE_ADDR + 0x14000)
#define     SIM_BASE_ADDR           (SPBA0_BASE_ADDR + 0x18000)
#if defined(CONFIG_MOT_WFN435)
#define     SIM1_BASE_ADDR           SIM_BASE_ADDR
#endif
#define     IIM_BASE_ADDR           (SPBA0_BASE_ADDR + 0x1C000)
#define     CTI_SDMA_BASE_ADDR      (SPBA0_BASE_ADDR + 0x20000)
#define     USBOTG_CTRL_BASE_ADDR   (SPBA0_BASE_ADDR + 0x24000)
#define     USBOTG_DATA_BASE_ADDR   (SPBA0_BASE_ADDR + 0x28000)
#define     CSPI1_BASE_ADDR         (SPBA0_BASE_ADDR + 0x30000)
#define     SPBA_CTRL_BASE_ADDR     (SPBA0_BASE_ADDR + 0x3C000)
#define     IOMUX_COM_BASE_ADDR     (SPBA0_BASE_ADDR + 0x40000)
#define     CRM_COM_BASE_ADDR       (SPBA0_BASE_ADDR + 0x44000)
#define     CRM_AP_BASE_ADDR        (SPBA0_BASE_ADDR + 0x48000)
#define     PLL0_BASE_ADDR          (SPBA0_BASE_ADDR + 0x4C000)
#define     PLL1_BASE_ADDR          (SPBA0_BASE_ADDR + 0x50000)
#define     PLL2_BASE_ADDR          (SPBA0_BASE_ADDR + 0x54000)
#define     GPIO4_SH_BASE_ADDR      (SPBA0_BASE_ADDR + 0x58000)
#define     HAC_BASE_ADDR           (SPBA0_BASE_ADDR + 0x5C000)
#define     SAHARA_BASE_ADDR        (SPBA0_BASE_ADDR + 0x5C000)
#define     PLL3_BASE_ADDR          (SPBA0_BASE_ADDR + 0x60000)

/*
 * SPBA global module enabled #1
 */
#define     SPBA1_BASE_ADDR         0x52000000
#define     SPBA1_BASE_ADDR_VIRT    0xD4400000
#define     SPBA1_SIZE              SZ_1M

#define     MQSPI_BASE_ADDR         (SPBA1_BASE_ADDR + 0x34000)
#define     EL1T_BASE_ADDR          (SPBA1_BASE_ADDR + 0x38000)

/*!
 * Defines for SPBA modules
 */
#define SPBA_SDHC1		0x04
#define SPBA_SDHC2		0x08
#define SPBA_UART3		0x0C
#define SPBA_CSPI2		0x10
#define SPBA_SSI2		0x14
#define SPBA_SIM		0x18
#define SPBA_IIM		0x1C
#define SPBA_CTI_SDMA		0x20
#define SPBA_USBOTG_CTRL_REGS	0x24
#define SPBA_USBOTG_DATA_REGS	0x28
#define SPBA_CSPI1		0x30
#define SPBA_MQSPI		0x34
#define SPBA_EL1T		0x38
#define SPBA_IOMUX		0x40
#define SPBA_CRM_COM		0x44
#define SPBA_CRM_AP		0x48
#define SPBA_PLL0		0x4C
#define SPBA_PLL1		0x50
#define SPBA_PLL2		0x54
#define SPBA_GPIO4		0x58
#define SPBA_SAHARA		0x5C

/*
 * AIPS 2
 */
#define     AIPS2_BASE_ADDR         0x53F00000
#define     AIPS2_BASE_ADDR_VIRT    0xD4200000
#define     AIPS2_SIZE              SZ_1M

#define     GEMK_BASE_ADDR          (AIPS2_BASE_ADDR + 0x8C000)
#define     GPT1_BASE_ADDR          (AIPS2_BASE_ADDR + 0x90000)
#define     EPIT1_AP_BASE_ADDR      (AIPS2_BASE_ADDR + 0x94000)
#define     SCC_BASE_ADDR           (AIPS2_BASE_ADDR + 0xAC000)
#define     RNGA_BASE_ADDR          (AIPS2_BASE_ADDR + 0xB0000)
#define     IPU_CTRL_BASE_ADDR      (AIPS2_BASE_ADDR + 0xC0000)
#define     AUDMUX_BASE_ADDR        (AIPS2_BASE_ADDR + 0xC4000)
#define     EDIO_BASE_ADDR          (AIPS2_BASE_ADDR + 0xC8000)
#define     GPIO1_AP_BASE_ADDR      (AIPS2_BASE_ADDR + 0xCC000)
#define     GPIO2_AP_BASE_ADDR      (AIPS2_BASE_ADDR + 0xD0000)
#define     SDMA_BASE_ADDR          (AIPS2_BASE_ADDR + 0xD4000)
#define     RTC_BASE_ADDR           (AIPS2_BASE_ADDR + 0xD8000)
#define     WDOG1_BASE_ADDR         (AIPS2_BASE_ADDR + 0xDC000)
#define     PWM_BASE_ADDR           (AIPS2_BASE_ADDR + 0xE0000)
#define     GPIO3_AP_BASE_ADDR      (AIPS2_BASE_ADDR + 0xE4000)
#define     WDOG2_BASE_ADDR         (AIPS2_BASE_ADDR + 0xE8000)
#define     RTIC_BASE_ADDR          (AIPS2_BASE_ADDR + 0xEC000)
#define     LPMC_BASE_ADDR          (AIPS2_BASE_ADDR + 0xF0000)

/*
 * ROMP and AVIC
 */
#define     ROMP_BASE_ADDR          0x60000000
#define     ROMP_BASE_ADDR_VIRT     0xD4300000
#define     ROMP_SIZE               SZ_64K

#define     AVIC_BASE_ADDR          0x68000000
#define     AVIC_BASE_ADDR_VIRT     0xD4310000
#define     AVIC_SIZE               SZ_64K

/*
 * NAND, SDRAM, WEIM, M3IF, EMI controllers
 */
#define     X_MEMC_BASE_ADDR        0xB8000000
#define     X_MEMC_BASE_ADDR_VIRT   0xD4320000
#define     X_MEMC_SIZE             SZ_64K

#define     NFC_BASE_ADDR           (X_MEMC_BASE_ADDR + 0x0000)
#define     ESDCTL_BASE_ADDR        (X_MEMC_BASE_ADDR + 0x1000)
#define     WEIM_BASE_ADDR          (X_MEMC_BASE_ADDR + 0x2000)
#define     M3IF_BASE_ADDR          (X_MEMC_BASE_ADDR + 0x3000)
#define     EMI_CTL_BASE_ADDR       (X_MEMC_BASE_ADDR + 0x4000)

/*
 * Memory regions and CS
 * CPLD is connected on CS4
 * CS5 is TP1021 or it is not connected
 * */
#if defined(CONFIG_MOT_WFN430)
#define     FB_RAM_BASE_ADDR        0x78000000
#define     FB_RAM_SIZE             SZ_256K
#endif
#if defined(CONFIG_MACH_MXC27530EVB)
#if !defined(CONFIG_MOT_WFN430)
#define     IPU_MEM_BASE_ADDR       0x70000000
#endif
#define     CSD0_BASE_ADDR          0x80000000
#define     CSD1_BASE_ADDR          0x90000000
#define     CS0_BASE_ADDR           0xA0000000
#define     CS1_BASE_ADDR           0xA8000000
#define     CS2_BASE_ADDR           0xB0000000
#define     CS3_BASE_ADDR           0xB2000000

#define     CS4_BASE_ADDR           0xB4000000
#define     CS4_BASE_ADDR_VIRT      0xEB000000
#define     CS4_SIZE                SZ_16M

#define     CS5_BASE_ADDR           0xB6000000
#define     CS5_BASE_ADDR_VIRT      0xEC000000
#define     CS5_SIZE                SZ_16M

#if !defined(CONFIG_MOT_WFN430)
#define     MAX_S0_BASE_ADDR        0xC0000000
#endif

#elif defined(CONFIG_MACH_SCMA11REF)
#define     CS4_BASE_ADDR           0xB4000000
#define     CS4_BASE_ADDR_VIRT      0xEB000000
#define     CS4_SIZE                SZ_16M
#endif /* defined(CONFIG_MACH_MXC27530EVB) */

/*!
 * This macro defines the physical to virtual address mapping for all the
 * peripheral modules. It is used by passing in the physical address as x
 * and returning the virtual address. If the physical address is not mapped,
 * it returns 0xDEADBEEF
 * Add new elif's for any new boards based off scma11 platform
 */
#if defined(CONFIG_MACH_MXC27530EVB)
#define IO_ADDRESS(x)   \
        (((x >= L2CC_BASE_ADDR) && (x < (L2CC_BASE_ADDR + L2CC_SIZE))) ? L2CC_IO_ADDRESS(x):\
        ((x >= AIPS1_BASE_ADDR) && (x < (AIPS1_BASE_ADDR + AIPS1_SIZE))) ? AIPS1_IO_ADDRESS(x):\
        ((x >= SPBA0_BASE_ADDR) && (x < (SPBA0_BASE_ADDR + SPBA0_SIZE))) ? SPBA0_IO_ADDRESS(x):\
        ((x >= SPBA1_BASE_ADDR) && (x < (SPBA1_BASE_ADDR + SPBA1_SIZE))) ? SPBA1_IO_ADDRESS(x):\
        ((x >= AIPS2_BASE_ADDR) && (x < (AIPS2_BASE_ADDR + AIPS2_SIZE))) ? AIPS2_IO_ADDRESS(x):\
        ((x >= ROMP_BASE_ADDR) && (x < (ROMP_BASE_ADDR + ROMP_SIZE))) ? ROMP_IO_ADDRESS(x):\
        ((x >= AVIC_BASE_ADDR) && (x < (AVIC_BASE_ADDR + AVIC_SIZE))) ? AVIC_IO_ADDRESS(x):\
        ((x >= CS4_BASE_ADDR) && (x < (CS4_BASE_ADDR + CS4_SIZE))) ? CS4_IO_ADDRESS(x):\
        ((x >= CS5_BASE_ADDR) && (x < (CS5_BASE_ADDR + CS5_SIZE))) ? CS5_IO_ADDRESS(x):\
        ((x >= X_MEMC_BASE_ADDR) && (x < (X_MEMC_BASE_ADDR + X_MEMC_SIZE))) ? X_MEMC_IO_ADDRESS(x):\
        0xDEADBEEF)
#elif defined(CONFIG_MACH_SCMA11REF)
#define IO_ADDRESS(x)   \
        (((x >= L2CC_BASE_ADDR) && (x < (L2CC_BASE_ADDR + L2CC_SIZE))) ? L2CC_IO_ADDRESS(x):\
        ((x >= AIPS1_BASE_ADDR) && (x < (AIPS1_BASE_ADDR + AIPS1_SIZE))) ? AIPS1_IO_ADDRESS(x):\
        ((x >= SPBA0_BASE_ADDR) && (x < (SPBA0_BASE_ADDR + SPBA0_SIZE))) ? SPBA0_IO_ADDRESS(x):\
        ((x >= SPBA1_BASE_ADDR) && (x < (SPBA1_BASE_ADDR + SPBA1_SIZE))) ? SPBA1_IO_ADDRESS(x):\
        ((x >= AIPS2_BASE_ADDR) && (x < (AIPS2_BASE_ADDR + AIPS2_SIZE))) ? AIPS2_IO_ADDRESS(x):\
        ((x >= ROMP_BASE_ADDR) && (x < (ROMP_BASE_ADDR + ROMP_SIZE))) ? ROMP_IO_ADDRESS(x):\
        ((x >= AVIC_BASE_ADDR) && (x < (AVIC_BASE_ADDR + AVIC_SIZE))) ? AVIC_IO_ADDRESS(x):\
        ((x >= CS4_BASE_ADDR) && (x < (CS4_BASE_ADDR + CS4_SIZE))) ? CS4_IO_ADDRESS(x):\
        ((x >= X_MEMC_BASE_ADDR) && (x < (X_MEMC_BASE_ADDR + X_MEMC_SIZE))) ? X_MEMC_IO_ADDRESS(x):\
        0xDEADBEEF)
#else
#define IO_ADDRESS(x)   \
        (((x >= L2CC_BASE_ADDR) && (x < (L2CC_BASE_ADDR + L2CC_SIZE))) ? L2CC_IO_ADDRESS(x):\
        ((x >= AIPS1_BASE_ADDR) && (x < (AIPS1_BASE_ADDR + AIPS1_SIZE))) ? AIPS1_IO_ADDRESS(x):\
        ((x >= SPBA0_BASE_ADDR) && (x < (SPBA0_BASE_ADDR + SPBA0_SIZE))) ? SPBA0_IO_ADDRESS(x):\
        ((x >= SPBA1_BASE_ADDR) && (x < (SPBA1_BASE_ADDR + SPBA1_SIZE))) ? SPBA1_IO_ADDRESS(x):\
        ((x >= AIPS2_BASE_ADDR) && (x < (AIPS2_BASE_ADDR + AIPS2_SIZE))) ? AIPS2_IO_ADDRESS(x):\
        ((x >= ROMP_BASE_ADDR) && (x < (ROMP_BASE_ADDR + ROMP_SIZE))) ? ROMP_IO_ADDRESS(x):\
        ((x >= AVIC_BASE_ADDR) && (x < (AVIC_BASE_ADDR + AVIC_SIZE))) ? AVIC_IO_ADDRESS(x):\
        ((x >= X_MEMC_BASE_ADDR) && (x < (X_MEMC_BASE_ADDR + X_MEMC_SIZE))) ? X_MEMC_IO_ADDRESS(x):\
        0xDEADBEEF)
#endif /* !defined(CONFIG_MACH_MXC27530EVB) */

/*
 * define the address mapping macros: in physical address order
 */

#define L2CC_IO_ADDRESS(x)  \
        (((x) - L2CC_BASE_ADDR) + L2CC_BASE_ADDR_VIRT)

#define AIPS1_IO_ADDRESS(x)  \
        (((x) - AIPS1_BASE_ADDR) + AIPS1_BASE_ADDR_VIRT)

#define SPBA0_IO_ADDRESS(x)  \
        (((x) - SPBA0_BASE_ADDR) + SPBA0_BASE_ADDR_VIRT)

#define SPBA1_IO_ADDRESS(x)  \
        (((x) - SPBA1_BASE_ADDR) + SPBA1_BASE_ADDR_VIRT)

#define AIPS2_IO_ADDRESS(x)  \
        (((x) - AIPS2_BASE_ADDR) + AIPS2_BASE_ADDR_VIRT)

#define ROMP_IO_ADDRESS(x)  \
        (((x) - ROMP_BASE_ADDR) + ROMP_BASE_ADDR_VIRT)

#define AVIC_IO_ADDRESS(x)  \
        (((x) - AVIC_BASE_ADDR) + AVIC_BASE_ADDR_VIRT)

#if defined(CONFIG_MACH_MXC27530EVB) || defined(CONFIG_MACH_SCMA11REF)
#define CS4_IO_ADDRESS(x)  \
        (((x) - CS4_BASE_ADDR) + CS4_BASE_ADDR_VIRT)

#if defined(CONFIG_MACH_MXC27530EVB)
#define CS5_IO_ADDRESS(x)  \
        (((x) - CS5_BASE_ADDR) + CS5_BASE_ADDR_VIRT)
#endif /* defined(CONFIG_MACH_MXC27530EVB) */
#endif /* defined(CONFIG_MACH_MXC27530EVB) || defined(CONFIG_MACH_SCMA11REF) */

#define X_MEMC_IO_ADDRESS(x)  \
        (((x) - X_MEMC_BASE_ADDR) + X_MEMC_BASE_ADDR_VIRT)

/*
 * DMA request assignments
 */
#define DMA_REQ_reserved31 31
#define DMA_REQ_NFC        30
#define DMA_REQ_SSI1_TX1   29
#define DMA_REQ_SSI1_RX1   28
#define DMA_REQ_SSI1_TX2   27
#define DMA_REQ_SSI1_RX2   26
#define DMA_REQ_SSI2_TX1   25
#define DMA_REQ_SSI2_RX1   24
#define DMA_REQ_SSI2_TX2   23
#define DMA_REQ_SSI2_RX2   22
#define DMA_REQ_SDHC2      21
#define DMA_REQ_SDHC1      20
#define DMA_REQ_UART1_TX   19
#define DMA_REQ_UART1_RX   18
#define DMA_REQ_UART2_TX   17
#define DMA_REQ_UART2_RX   16
#define DMA_REQ_UART3_TX   15
#define DMA_REQ_UART3_RX   14
#define DMA_REQ_EXT13      13
#define DMA_REQ_EXT12      12
#define DMA_REQ_reserved11 11
#define DMA_REQ_reserved10 10
#define DMA_REQ_CSPI1_TX   9
#define DMA_REQ_CSPI1_RX   8
#define DMA_REQ_CSPI2_TX   7
#define DMA_REQ_CSPI2_RX   6
#define DMA_REQ_reserved5  5
#define DMA_REQ_reserved4  4
#define DMA_REQ_reserved3  3
#define DMA_REQ_reserved2  2
#define DMA_REQ_reserved1  1
#define DMA_REQ_reserved0  0

/*
 * Interrupt numbers
 *
 * TODO: Interrupt numbers for first virtio release.
 * To be finalized once board schematic/Virtio ready.
 */
#define MXC_INT_BASE            0
#define INT_GPIO3               0
#define INT_EL1T_CI             1
#define INT_EL1T_RFCI           2
#define INT_EL1T_RFI            3
#define INT_EL1T_MCU            4
#define INT_EL1T_IPI            5
#define INT_MU_GEN              6
#define INT_GPIO4               7
#define INT_MMC_SDHC2           8
#define INT_MMC_SDHC1           9
#define INT_I2C                 10
#define INT_SSI2                11
#define INT_SSI1                12
#define INT_CSPI2               13
#define INT_CSPI1               14
#define INT_RTIC                15
#define INT_SAHARA              15
#define INT_HAC                 15
#define INT_UART3_RX            16
#define INT_UART3_TX            17
#define INT_UART3_MINT          18
#define INT_ECT                 19
#define INT_SIM_IPB             20
#define INT_SIM_DATA            21
#define INT_RNGA                22
#define INT_DSM_AP              23
#define INT_KPP                 24
#define INT_RTC                 25
#define INT_PWM                 26
#define INT_GEMK_AP             27
#define INT_EPIT                28
#define INT_GPT                 29
#define INT_UART2_RX            30
#define INT_UART2_TX            31
#define INT_UART2_MINT          32
#define INT_NANDFC              33
#define INT_SDMA                34
#define INT_USB_WAKEUP          35
#define INT_USB_SOF             36
#define INT_PMU_EVTMON          37
#define INT_USB_FUNC            38
#define INT_USB_DMA             39
#define INT_USB_CTRL            40
#define INT_IPU_ERR             41
#define INT_IPU_SYN             42
#define INT_UART1_RX            43
#define INT_UART1_TX            44
#define INT_UART1_MINT          45
#define INT_IIM                 46
#define INT_MU_RX_OR            47
#define INT_MU_TX_OR            48
#define INT_SCC_SCM             49
#define INT_SCC_SMN             50
#define INT_GPIO2               51
#define INT_GPIO1               52
#define INT_MQSPI1              53
#define INT_MQSPI2              54
#define INT_WDOG2               55
#define INT_EXT_INT7            56
#define INT_EXT_INT6            57
#define INT_EXT_INT5            58
#define INT_EXT_INT4            59
#define INT_EXT_INT3            60
#define INT_EXT_INT2            61
#define INT_EXT_INT1            62
#define INT_EXT_INT0            63

#define MXC_MAX_INT_LINES       63
#define MXC_MAX_EXT_LINES       8

#if defined(CONFIG_MOT_FEAT_DEBUG_WDOG) || defined(CONFIG_FIQ)
#define FIQ_START		0
#endif
/*!
 * Interrupt Number for ARM11 PMU
 */
#define ARM11_PMU_IRQ		INT_PMU_EVTMON

#define	MXC_GPIO_BASE		(MXC_MAX_INT_LINES + 1)

/*!
 * Number of GPIO port as defined in the IC Spec
 */
#define GPIO_PORT_NUM           4
/*!
 * Number of GPIO pins per port
 */
#define GPIO_NUM_PIN            32

#ifdef CONFIG_MOT_FEAT_GPIO_API
#define GPIO_AP_A_PORT          0
#define GPIO_AP_B_PORT          1
#define GPIO_AP_C_PORT          2
#define GPIO_SP_A_PORT          3
#endif

#define WEIM_CTRL_CS0           WEIM_BASE_ADDR
#define CSCRU                   0x00
#define CSCRL                   0x04
#define CSCRA                   0x08

#define PROD_SIGNATURE          0x8	/* For MXC91231 */
#define CHIP_REV_1_0            0x10
#define CHIP_REV_2_0            0x20
#define CHIP_REV_2_1            0x21

#define SYSTEM_REV_MIN          CHIP_REV_1_0
#define SYSTEM_REV_NUM          3

#endif				/*  __ASM_ARM_ARCH_MXC91231_H_ */
