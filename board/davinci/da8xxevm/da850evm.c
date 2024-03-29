/*
 * Copyright (C) 2009 Nick Thompson, GE Fanuc, Ltd. <nick.thompson@gefanuc.com>
 *
 * Base on code from TI. Original Notices follow:
 *
 * (C) Copyright 2008, Texas Instruments, Inc. http://www.ti.com/
 *
 * Modified for DA8xx EVM.
 *
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
 *
 * Parts are shamelessly stolen from various TI sources, original copyright
 * follows:
 * -----------------------------------------------------------------
 *
 * Copyright (C) 2004 Texas Instruments.
 *
 * ----------------------------------------------------------------------------
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 */

#include <common.h>
#include <i2c.h>
#include <spi.h>
#include <net.h>
#include <spi_flash.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <nand.h>
#include <asm/arch/nand_defs.h>
#include "../common/misc.h"
#ifdef CONFIG_DAVINCI_MMC
#include <mmc.h>
#include <asm/arch/sdmmc_defs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define pinmux	&davinci_syscfg_regs->pinmux

#ifdef CONFIG_SPI_FLASH
/* SPI0 pin muxer settings */
const struct pinmux_config spi1_pins[] = {
	{ pinmux[5], 1, 1 },
	{ pinmux[5], 1, 2 },
	{ pinmux[5], 1, 4 },
	{ pinmux[5], 1, 5 },
};
#endif

#ifdef CONFIG_DAVINCI_MMC
/* SPI0 pin muxer settings */
const struct pinmux_config mmc0_pins[] = {
	/* GP0[11] is required for SD to work on Rev 3 EVMs */
	{ pinmux[0],  8, 4 },   /* GP0[11] */
	{ pinmux[10], 2, 0 },	/* MMCSD0_CLK */
	{ pinmux[10], 2, 1 },	/* MMCSD0_CMD */
	{ pinmux[10], 2, 2 },	/* MMCSD0_DAT_0 */
	{ pinmux[10], 2, 3 },	/* MMCSD0_DAT_1 */
	{ pinmux[10], 2, 4 },	/* MMCSD0_DAT_2 */
	{ pinmux[10], 2, 5 },	/* MMCSD0_DAT_3 */
	/* DA850 supports only 4-bit mode, remaining pins are not configured */
};
#endif

/* UART pin muxer settings */
const struct pinmux_config uart_pins[] = {
	{ pinmux[0], 4, 6 },
	{ pinmux[0], 4, 7 },
	{ pinmux[4], 2, 4 },
	{ pinmux[4], 2, 5 }
};

#ifdef CONFIG_DRIVER_TI_EMAC
const struct pinmux_config emac_pins[] = {
#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
	{ pinmux[14], 8, 2 },
	{ pinmux[14], 8, 3 },
	{ pinmux[14], 8, 4 },
	{ pinmux[14], 8, 5 },
	{ pinmux[14], 8, 6 },
	{ pinmux[14], 8, 7 },
	{ pinmux[15], 8, 1 },
#else
	{ pinmux[2], 8, 1 },
	{ pinmux[2], 8, 2 },
	{ pinmux[2], 8, 3 },
	{ pinmux[2], 8, 4 },
	{ pinmux[2], 8, 5 },
	{ pinmux[2], 8, 6 },
	{ pinmux[2], 8, 7 },
	{ pinmux[3], 8, 0 },
	{ pinmux[3], 8, 1 },
	{ pinmux[3], 8, 2 },
	{ pinmux[3], 8, 3 },
	{ pinmux[3], 8, 4 },
	{ pinmux[3], 8, 5 },
	{ pinmux[3], 8, 6 },
	{ pinmux[3], 8, 7 },
#endif /* CONFIG_DRIVER_TI_EMAC_USE_RMII */
	{ pinmux[4], 8, 0 },
	{ pinmux[4], 8, 1 }
};
#endif /* CONFIG_DRIVER_TI_EMAC */

/* I2C pin muxer settings */
const struct pinmux_config i2c_pins[] = {
	{ pinmux[4], 2, 2 },
	{ pinmux[4], 2, 3 }
};

#ifdef CONFIG_USE_NAND
const struct pinmux_config aemif_pins[] = {
	{ pinmux[7], 1, 1 },
	{ pinmux[7], 1, 2 },
	{ pinmux[7], 1, 4 },
	{ pinmux[7], 1, 5 },
	{ pinmux[9], 1, 0 },
	{ pinmux[9], 1, 1 },
	{ pinmux[9], 1, 2 },
	{ pinmux[9], 1, 3 },
	{ pinmux[9], 1, 4 },
	{ pinmux[9], 1, 5 },
	{ pinmux[9], 1, 6 },
	{ pinmux[9], 1, 7 },
	{ pinmux[12], 1, 5 },
	{ pinmux[12], 1, 6 }
};
#elif defined(CONFIG_SYS_USE_NOR)
const struct pinmux_config nor_pins[] = {
	/* GP0[11] is required for SD to work on Rev 3 EVMs */
	{ pinmux[0], 8, 4 },   /* GP0[11] */
	{ pinmux[5], 1, 6 },
	{ pinmux[6], 1, 6 },
	{ pinmux[7], 1, 0 },
	{ pinmux[7], 1, 4 },
	{ pinmux[7], 1, 5 },
	{ pinmux[8], 1, 0 },
	{ pinmux[8], 1, 1 },
	{ pinmux[8], 1, 2 },
	{ pinmux[8], 1, 3 },
	{ pinmux[8], 1, 4 },
	{ pinmux[8], 1, 5 },
	{ pinmux[8], 1, 6 },
	{ pinmux[8], 1, 7 },
	{ pinmux[9], 1, 0 },
	{ pinmux[9], 1, 1 },
	{ pinmux[9], 1, 2 },
	{ pinmux[9], 1, 3 },
	{ pinmux[9], 1, 4 },
	{ pinmux[9], 1, 5 },
	{ pinmux[9], 1, 6 },
	{ pinmux[9], 1, 7 },
	{ pinmux[10], 1, 0 },
	{ pinmux[10], 1, 1 },
	{ pinmux[10], 1, 2 },
	{ pinmux[10], 1, 3 },
	{ pinmux[10], 1, 4 },
	{ pinmux[10], 1, 5 },
	{ pinmux[10], 1, 6 },
	{ pinmux[10], 1, 7 },
	{ pinmux[11], 1, 0 },
	{ pinmux[11], 1, 1 },
	{ pinmux[11], 1, 2 },
	{ pinmux[11], 1, 3 },
	{ pinmux[11], 1, 4 },
	{ pinmux[11], 1, 5 },
	{ pinmux[11], 1, 6 },
	{ pinmux[11], 1, 7 },
	{ pinmux[12], 1, 0 },
	{ pinmux[12], 1, 1 },
	{ pinmux[12], 1, 2 },
	{ pinmux[12], 1, 3 },
	{ pinmux[12], 1, 4 },
	{ pinmux[12], 1, 5 },
	{ pinmux[12], 1, 6 },
	{ pinmux[12], 1, 7 }
};
#endif

int board_init(void)
{
	unsigned int temp;
#ifndef CONFIG_USE_IRQ
	/*
	 * Mask all IRQs by clearing the global enable and setting
	 * the enable clear for all the 90 interrupts.
	 */

	writel(0, &davinci_aintc_regs->ger);

	writel(0, &davinci_aintc_regs->hier);

	writel(0xffffffff, &davinci_aintc_regs->ecr1);
	writel(0xffffffff, &davinci_aintc_regs->ecr2);
	writel(0xffffffff, &davinci_aintc_regs->ecr3);
#endif

	/* arch number of the board */
	gd->bd->bi_arch_number = MACH_TYPE_DAVINCI_DA850_EVM;

	/* address of boot parameters */
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;

	/*
	 * Power on required peripherals
	 * ARM does not have access by default to PSC0 and PSC1
	 * assuming here that the DSP bootloader has set the IOPU
	 * such that PSC access is available to ARM
	 */
	lpsc_on(DAVINCI_LPSC_AEMIF);    /* NAND, NOR */
	lpsc_on(DAVINCI_LPSC_SPI1);     /* Serial Flash */
	lpsc_on(DAVINCI_LPSC_EMAC);     /* image download */
	lpsc_on(DAVINCI_LPSC_UART2);    /* console */
	lpsc_on(DAVINCI_LPSC_GPIO);
#ifdef CONFIG_DAVINCI_MMC
	lpsc_on(DAVINCI_LPSC_MMC_SD);
#endif

	/* setup the SUSPSRC for ARM to control emulation suspend */
	writel(readl(&davinci_syscfg_regs->suspsrc) &
	       ~(DAVINCI_SYSCFG_SUSPSRC_EMAC | DAVINCI_SYSCFG_SUSPSRC_I2C |
		 DAVINCI_SYSCFG_SUSPSRC_SPI1 | DAVINCI_SYSCFG_SUSPSRC_TIMER0 |
		 DAVINCI_SYSCFG_SUSPSRC_UART2),
	       &davinci_syscfg_regs->suspsrc);

#ifdef CONFIG_SPI_FLASH
	if (davinci_configure_pin_mux(spi1_pins, ARRAY_SIZE(spi1_pins)) != 0)
		return 1;
#endif

#ifdef CONFIG_DAVINCI_MMC
	if (davinci_configure_pin_mux(mmc0_pins, ARRAY_SIZE(mmc0_pins)) != 0)
		return 1;

	/* Set the GPIO direction as output */
	temp = REG(GPIO_BANK0_REG_DIR_ADDR);
	temp &= ~(0x01 << 11);
	REG(GPIO_BANK0_REG_DIR_ADDR) = temp;

	/* Set the output as high */
	temp = REG(GPIO_BANK0_REG_SET_ADDR);
	temp |= (0x01 << 11);
	REG(GPIO_BANK0_REG_SET_ADDR) = temp;
#endif

	if (davinci_configure_pin_mux(uart_pins, ARRAY_SIZE(uart_pins)) != 0)
		return 1;

	if (davinci_configure_pin_mux(i2c_pins, ARRAY_SIZE(i2c_pins)) != 0)
		return 1;

#ifdef CONFIG_DRIVER_TI_EMAC
	if (davinci_configure_pin_mux(emac_pins, ARRAY_SIZE(emac_pins)) != 0)
		return 1;
#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
	REG(CFGCHIP3) |= (1 << 8);
#else
	/* set cfgchip3 to selct MII */
	REG(CFGCHIP3) &= ~(1 << 8);
#endif /* CONFIG_DRIVER_TI_EMAC_USE_RMII */

#endif /* CONFIG_DRIVER_TI_EMAC */

#ifdef CONFIG_USE_NAND
	if (davinci_configure_pin_mux(aemif_pins, ARRAY_SIZE(aemif_pins)) != 0)
		return 1;
#elif defined(CONFIG_SYS_USE_NOR)
	if (davinci_configure_pin_mux(nor_pins, ARRAY_SIZE(nor_pins)) != 0)
		return 1;

	/* Set the GPIO direction as output */
	temp = REG(GPIO_BANK0_REG_DIR_ADDR);
	temp &= ~(0x01 << 11);
	REG(GPIO_BANK0_REG_DIR_ADDR) = temp;

	/* Set the output as low */
	temp = REG(GPIO_BANK0_REG_SET_ADDR);
	temp |= (0x01 << 11);
	REG(GPIO_BANK0_REG_CLR_ADDR) = temp;
#endif

	/* enable the console UART */
	writel((DAVINCI_UART_PWREMU_MGMT_FREE | DAVINCI_UART_PWREMU_MGMT_URRST |
		DAVINCI_UART_PWREMU_MGMT_UTRST),
	       &davinci_uart2_ctrl_regs->pwremu_mgmt);

	return(0);
}

#define CFG_MAC_ADDR_SPI_BUS	0
#define CFG_MAC_ADDR_SPI_CS	0
#define CFG_MAC_ADDR_SPI_MAX_HZ	CONFIG_SF_DEFAULT_SPEED
#define CFG_MAC_ADDR_SPI_MODE	SPI_MODE_3

#define CFG_MAC_ADDR_OFFSET	(flash->size - SZ_64K)

static int  get_mac_addr(u8 *addr)
{
	int ret;
	struct spi_flash *flash;

	flash = spi_flash_probe(CFG_MAC_ADDR_SPI_BUS, CFG_MAC_ADDR_SPI_CS,
			CFG_MAC_ADDR_SPI_MAX_HZ, CFG_MAC_ADDR_SPI_MODE);
	if (!flash) {
		printf(" Error - unable to probe SPI flash.\n");
		goto err_probe;
	}

	ret = spi_flash_read(flash, CFG_MAC_ADDR_OFFSET, 6, addr);
	if (ret) {
		printf("Error - unable to read MAC address from SPI flash.\n");
		goto err_read;
	}

err_read:
	/* cannot call free currently since the free function calls free() for
	 * spi_flash structure though it is not directly allocated through
	 * malloc()
	 */
	/* spi_flash_free(flash); */
err_probe:
	return ret;
}

#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
/**
 * rmii_hw_init
 *
 * DA850/OMAP-L138 EVM can interface to a daughter card for
 * additional features. This card has an I2C GPIO Expander TCA6416
 * to select the required functions like camera, RMII Ethernet,
 * character LCD, video.
 *
 * Initialization of the expander involves configuring the
 * polarity and direction of the ports. P07-P05 are used here.
 * These ports are connected to a Mux chip which enables only one
 * functionality at a time.
 *
 * For RMII phy to respond, the MII MDIO clock has to be  disabled
 * since both the PHY devices have address as zero. The MII MDIO
 * clock is controlled via GPIO2[6].
 *
 * This code is valid for Beta version of the hardware
 */
int rmii_hw_init(void)
{
	const struct pinmux_config gpio_pins[] = {
		{ pinmux[6], 8, 1 }
	};
	u_int8_t buf[2];
	unsigned int temp;
	int ret;

	/* PinMux for GPIO */
	if (davinci_configure_pin_mux(gpio_pins, ARRAY_SIZE(gpio_pins)) != 0)
		return 1;

	/* I2C Exapnder configuration */
	/* Set polarity to non-inverted */
	buf[0] = 0x0;
	buf[1] = 0x0;
	ret = i2c_write(CONFIG_SYS_I2C_EXPANDER_ADDR, 4, 1, buf, 2);
	if (ret) {
		printf("\nExpander @ 0x%02x write FAILED!!!\n",
				CONFIG_SYS_I2C_EXPANDER_ADDR);
		return ret;
	}

	/* Configure P07-P05 as outputs */
	buf[0] = 0x1f;
	buf[1] = 0xff;
	ret = i2c_write(CONFIG_SYS_I2C_EXPANDER_ADDR, 6, 1, buf, 2);
	if (ret) {
		printf("\nExpander @ 0x%02x write FAILED!!!\n",
				CONFIG_SYS_I2C_EXPANDER_ADDR);
	}

	/* For Ethernet RMII selection
	 * P07(SelA)=0
	 * P06(SelB)=1
	 * P05(SelC)=1
	 */
	if (i2c_read(CONFIG_SYS_I2C_EXPANDER_ADDR, 2, 1, buf, 1)) {
		printf("\nExpander @ 0x%02x read FAILED!!!\n",
				CONFIG_SYS_I2C_EXPANDER_ADDR);
	}

	buf[0] &= 0x1f;
	buf[0] |= (0 << 7) | (1 << 6) | (1 << 5);
	if (i2c_write(CONFIG_SYS_I2C_EXPANDER_ADDR, 2, 1, buf, 1)) {
		printf("\nExpander @ 0x%02x write FAILED!!!\n",
				CONFIG_SYS_I2C_EXPANDER_ADDR);
	}

	/* Set the output as high */
	temp = REG(GPIO_BANK2_REG_SET_ADDR);
	temp |= (0x01 << 6);
	REG(GPIO_BANK2_REG_SET_ADDR) = temp;

	/* Set the GPIO direction as output */
	temp = REG(GPIO_BANK2_REG_DIR_ADDR);
	temp &= ~(0x01 << 6);
	REG(GPIO_BANK2_REG_DIR_ADDR) = temp;

	return 0;
}
#endif

void dsp_lpsc_on(unsigned domain, unsigned int id)
{
	dv_reg_p mdstat, mdctl, ptstat, ptcmd;
	struct davinci_psc_regs *psc_regs;

	psc_regs = davinci_psc0_regs;
	mdstat = &psc_regs->psc0.mdstat[id];
	mdctl = &psc_regs->psc0.mdctl[id];
	ptstat = &psc_regs->ptstat;
	ptcmd = &psc_regs->ptcmd;

	while (*ptstat & (0x1 << domain)) {;}

	if ((*mdstat & 0x1f) == 0x03)
		return;                 /* Already on and enabled */

	*mdctl |= 0x03;

	*ptcmd = 0x1 << domain;

	while (*ptstat & (0x1 << domain)) {;}
	while ((*mdstat & 0x1f) != 0x03) {;}    /* Probably an overkill... */
}

static void dspwake(void)
{
	unsigned *resetvect = (unsigned *)DAVINCI_L3CBARAM_BASE;

	/* if the device is ARM only, return */
	if ((REG(CHIP_REV_ID_REG) & 0x3f) == 0x10)
		return;

	if (!strcmp(getenv("dspwake"), "no"))
		return;

	*resetvect++ = 0x1E000; /* DSP Idle */
	/* clear out the next 10 words as NOP */
	memset(resetvect, 0, sizeof(unsigned) * 10);

	/* setup the DSP reset vector */
	REG(HOST1CFG) = DAVINCI_L3CBARAM_BASE;

	dsp_lpsc_on(1, DAVINCI_LPSC_GEM);
	REG(PSC0_MDCTL + (15 * 4)) |= 0x100;
}

int misc_init_r(void)
{
	uint8_t tmp[20], addr[10];

	printf ("ARM Clock : %d Hz\n", clk_get(DAVINCI_ARM_CLKID));
	printf ("DDR Clock : %d Hz\n", clk_get(DAVINCI_DDR_CLKID)/2);

	if (getenv("ethaddr") == NULL) {
		/* Set Ethernet MAC address from EEPROM */
		get_mac_addr(addr);

		if(is_multicast_ether_addr(addr) || is_zero_ether_addr(addr)) {
			printf("Invalid MAC address read.\n");
			return -EINVAL;
		}
		sprintf((char *)tmp, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],
				addr[1], addr[2], addr[3], addr[4], addr[5]);

		setenv("ethaddr", (char *)tmp);
	}

#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
	/* Select RMII fucntion through the expander */
	if (rmii_hw_init())
		printf("RMII hardware init failed!!!\n");
#endif

	dspwake();	

	return (0);
}

#ifdef CONFIG_NAND_DAVINCI
int board_nand_init(struct nand_chip *nand)
{
       davinci_nand_init(nand);

       return 0;
}
#endif

#ifdef CONFIG_DAVINCI_MMC
static struct davinci_mmc mmc_sd0 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD0_BASE,
	.host_caps = MMC_MODE_4BIT,	/* DA850 supports only 4-bit SD/MMC */
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};

int board_mmc_init(bd_t *bis)
{
	mmc_sd0.input_clk = clk_get(DAVINCI_MMCSD_CLKID);

	/* Add slot-0 to mmc subsystem */
	return davinci_mmc_init(bis, &mmc_sd0);
}
#endif


