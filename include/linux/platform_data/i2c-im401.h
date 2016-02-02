/*
 * i2c-im401.h  --  IM401 Smart DMIC bridge driver
 *
 * Copyright 2014 Fortemedia Inc.
 * Author: Henry Zhang <henryhzhang@fortemedia.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _IM401_H_
#define _IM401_H_
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>

//#include <mach/board_lge.h>

/*
 * 	TrainDataHeader.
 * 	The header packs training data generated by Sensory which are gram.bin
 * 	and net.bin. It provides info for driver to identify the ID of training
 * 	data and for firmware to load the training data.
 */

#define TD_SYNC 0x2379

typedef struct {
	short sync;			// The value should be TD_SYNC. Otherwise, it is
					// not legal.
	short ID;			// ID of the training data which should have a
					// matching in SMicTDInfo.
	long NetStartOffset;		// The offset (bytes) of start address of net.bin
					// from the start address of gram.bin in Smart Mic.
	long NetEndOffset;		// The offset (bytes) of end address of net.bin
					// from the start address of gram.bin in Smart Mic.
} TrainDataHeader;



/*
 * 	SMicFWHeader & SMicFWSubHeader.
 * 	This header packs FW binaries and provide information for driver to
 * 	load firmware to Smart Mic. The FW package is able to pack a variety
 * 	number of binary, ex: IRAM binary, DRAM binary...etc.
 * 	a SMicFWHeader should be followed by a SMicFWSubHeader which provides
 * 	loading information of training data.
 */

typedef struct {
	unsigned int Offset;		// The location of the binary in this file.
	unsigned int Size;		// The size of the binary.
	unsigned int Addr;		// The address of the binary in Smart Mic.
} SMicBinInfo;


typedef struct {
	unsigned int ID;		// ID of the Train Data. This ID allows driver to
					// find the corresponding Train Data by matching
					// this ID with the ID in TrainDataHeader.
	unsigned int Addr;		// The address of the training data in Smart Mic.
} SMicTDInfo;


#define	SMICFW_SYNC			0x23795888

typedef struct {
	unsigned int Sync;		// The value should be SMICFW_SYNC. Otherwise, it
					// is not legal.
	unsigned int Version;		// Version number of the FW.
	unsigned int NumBin;		// Number of binary file.
	SMicBinInfo *BinArray;		// Information of each binary file.
} SMicFWHeader;

typedef struct {
	unsigned int NumTD;		// Number of training data (trigger).
	SMicTDInfo *TDArray;		// Information of each train data.
} SMicFWSubHeader;

struct IM401_gpio {
    struct device *dev;
    struct kobject *kobj;
    struct input_dev *input_dev;

    struct delayed_work     irq_work;
    struct delayed_work     det_work;
    struct mutex        read_mutex;
    unsigned int        irq_gpio;
    bool            irq_enabled;
    spinlock_t      irq_enabled_lock;
};


/*
 * SMIC_REG_FW_STATUS
 * It is the address for driver to check the state of SmartMic.
 */
#define	SMIC_REG_FW_STATUS_mask		0x00ff
#define SMIC_REG_FW_STATUS_shift	0x0

/*
 * Value definition of SMIC_REG_FW_STATUS.
 */
#define SMIC_ERR_INIT_SUCCESS		0x1
#define SMIC_ERR_TD_OVERSIZE		0x2
#define SMIC_ERR_TRAP				0x3

#define IM401_FIRMWARE		"/etc/firmware/"
//Henry temporarily change the custom firmware path to default.
#if 0
#define IM401_CUSTOM_FIRMWARE	"/sdcard/im401/"
#else
#define IM401_CUSTOM_FIRMWARE	"/etc/firmware/"
#endif
#define IM401_PRIVATE_DATA1		"/etc/firmware/0x0fff0000.dat"
#define IM401_PRIVATE_DATA2		"/etc/firmware/0x0ffe0000.dat"
#define IM401_PRIVATE_GRAMMAR	"/etc/firmware/grammar.dat"

#define IM401_RESET						0x00
#define IM401_ANA_CIRCUIT_CTRL_LDO1		0x02
#define IM401_ANA_CIRCUIT_CTRL_LDO2		0x03
#define IM401_ANA_CIRCUIT_CTRL_LDO3		0x04
#define IM401_ANA_CIRCUIT_CTRL_ADC1_1	0x05
#define IM401_ANA_CIRCUIT_CTRL_ADC1_2	0x06
#define IM401_ANA_CIRCUIT_CTRL_ADC2_1	0x07
#define IM401_ANA_CIRCUIT_CTRL_ADC2_2	0x08
#define IM401_ANA_CIRCUIT_CTRL_ADC2_3	0x09
#define IM401_ANA_CIRCUIT_CTRL_MICBST	0x0a
#define IM401_ANA_CIRCUIT_CTRL_ADCFED	0x0b
#define IM401_ANA_CIRCUIT_CTRL_INPUTBUF	0x0c
#define IM401_ANA_CIRCUIT_CTRL_VREF		0x0d
#define IM401_ANA_CIRCUIT_CTRL_MBIAS		0x0e
#define IM401_AD_DIG_FILTER_CTRL1		0x2a
#define IM401_AD_DIG_FILTER_CTRL2		0x2b
#define IM401_DFT_BIST_SCAN				0x2c
#define IM401_UPFILTER_CTRL1				0x2d
#define IM401_UPFILTER_CTRL2				0x2e
#define IM401_GPIO_CTRL1					0x40
#define IM401_GPIO_CTRL2					0x41
#define IM401_GPIO_CTRL3					0x42
#define IM401_GPIO_STATUS				0x43
#define IM401_DIG_PAD_CTRL1				0x44
#define IM401_DIG_PAD_CTRL2				0x45
#define IM401_DMIC_DATA_CTRL				0x46
#define IM401_TEST_MODE_CTRL1			0x4c
#define IM401_TEST_MODE_CTRL2			0x4d
#define IM401_TEST_MODE_CTRL3			0x4e
#define IM401_VAD_CTRL1					0x50
#define IM401_VAD_CTRL2					0x51
#define IM401_VAD_CTRL3					0x52
#define IM401_VAD_CTRL4					0x53
#define IM401_VAD_STATUS1				0x54
#define IM401_VAD_STATUS2				0x55
#define IM401_BUF_SRAM_CTRL1				0x57
#define IM401_BUF_SRAM_CTRL2				0x58
#define IM401_BUF_SRAM_CTRL3				0x59
#define IM401_BUF_SRAM_CTRL4				0x5a
#define IM401_BUF_SRAM_CTRL5				0x5b
#define IM401_BUF_SRAM_CTRL6				0x5c
#define IM401_BUF_SRAM_CTRL7				0x5d
#define IM401_AUTO_MODE_CTRL				0x60
#define IM401_PWR_ANLG1					0x61
#define IM401_PWR_ANLG2					0x62
#define IM401_PWR_DIG					0x64
#define IM401_PWR_DSP					0x65
#define IM401_PRIV_INDEX					0x6a
#define IM401_PRIV_DATA					0x6c
#define IM401_BUF_MODE_CTRL_PLL_CAL1		0x73
#define IM401_BUF_MODE_CTRL_PLL_CAL2		0x74
#define IM401_BUF_MODE_CTRL_PLL_CAL3		0x75
#define IM401_BUF_MODE_CTRL_PLL_CAL4		0x76
#define IM401_BUF_MODE_CTRL_PLL_CAL5		0x78
#define IM401_BUF_MODE_CTRL_PLL_CAL6		0x79
#define IM401_KEY_FHRASE_CTRL_AVD		0x7b
#define IM401_AUTO_CLK_SEL_STATUS1		0x7c
#define IM401_AUTO_CLK_SEL_STATUS2		0x7d
#define IM401_AUTO_CLK_SEL_STATUS3		0x7e
#define IM401_AUTO_CLK_SEL_STATUS4		0x7f
#define IM401_PLL_CLOCK_CTRL1			0x80
#define IM401_PLL_CLOCK_CTRL2			0x81
#define IM401_PLL_CLOCK_CTRL3			0x82
#define IM401_PLL_CAL_CTRL1				0x83
#define IM401_PLL_CAL_CTRL2				0x84
#define IM401_PLL_CAL_CTRL3				0x85
#define IM401_PLL_CAL_CTRL4				0x86
#define IM401_PLL_CAL_CTRL5				0x87
#define IM401_PLL_CAL_CTRL6				0x88
#define IM401_PLL_CAL_CTRL7				0x89
#define IM401_PLL_CAL_CTRL8				0x8a
#define IM401_PLL_CAL_CTRL9				0x8b
#define IM401_PLL_CAL_STATUS1			0x8c
#define IM401_PLL_CAL_STATUS2			0x8d
#define IM401_PLL_CAL_STATUS3			0x8e
#define IM401_DSP_CTRL1					0x90
#define IM401_DSP_CTRL2					0x91
#define IM401_DSP_CTRL3					0x92
#define IM401_DSP_CTRL4					0x93
#define IM401_DSP_CTRL5					0x94
#define IM401_DSP_CTRL6					0x95
#define IM401_DSP_CTRL7					0x96
#define IM401_DSP_CTRL8					0x97
#define IM401_DSP_CTRL9					0x98
#define IM401_DSP_CTRL10					0x99
#define IM401_DSP_CTRL11					0x9a
#define IM401_DSP_CTRL12					0x9b
#define IM401_DSP_CTRL13					0x9c
#define IM401_DSP_CTRL14					0x9d
#define IM401_DSP_CTRL15					0x9e
#define IM401_PLL_CLK_EXT_CTRL1			0xc0
#define IM401_PLL_CLK_EXT_CTRL2			0xc1
#define IM401_ADC_EXT_CTRL1				0xc2
#define IM401_DUMMY_RTK1			0xd0
#define IM401_DUMMY_RTK2			0xd1
#define IM401_DUMMY_RTK3			0xd2
#define IM401_DUMMY_RTK4			0xd3
#define IM401_DUMMY_RTK5			0xd4
#define IM401_DUMMY_RTK6			0xd5
#define IM401_DUMMY_RTK7			0xd6
#define IM401_DUMMY_RTK8			0xd7
#define IM401_DUMMY_RTK9			0xd8
#define IM401_DUMMY_RTK10			0xd9
#define IM401_DUMMY_RTK11			0xda
#define IM401_DUMMY_RTK12			0xdb
#define IM401_DUMMY_RTK13			0xdc
#define IM401_DUMMY_RTK14			0xdd
#define IM401_DUMMY_RTK15			0xde
#define IM401_DUMMY_RTK16			0xdf
#define IM401_DUMMY_CUSTOMER1			0xe0
#define IM401_DUMMY_CUSTOMER2			0xe1
#define IM401_DUMMY_CUSTOMER3			0xe2
#define IM401_DUMMY_CUSTOMER4			0xe3
#define IM401_DUMMY_CUSTOMER5			0xe4
#define IM401_DUMMY_CUSTOMER6			0xe5
#define IM401_DUMMY_CUSTOMER7			0xe6
#define IM401_DUMMY_CUSTOMER8			0xe7
#define IM401_DUMMY_CUSTOMER9			0xe8
#define IM401_DUMMY_CUSTOMER10			0xe9
#define IM401_DUMMY_CUSTOMER11			0xea
#define IM401_DUMMY_CUSTOMER12			0xeb
#define IM401_DUMMY_CUSTOMER13			0xec
#define IM401_DUMMY_CUSTOMER14			0xed
#define IM401_DUMMY_CUSTOMER15			0xee
#define IM401_DUMMY_CUSTOMER16			0xef
#define IM401_DSP_MEM_CTRL1			0xf0
#define IM401_DSP_MEM_CTRL2			0xf1
#define IM401_DSP_MEM_CTRL3			0xf2
#define IM401_DSP_MEM_CTRL4			0xf3
#define IM401_DSP_MEM_CTRL5			0xf4
#define IM401_DSP_MEM_CTRL6			0xf7
#define IM401_DSP_MEM_CTRL7			0xf8
#define IM401_DUMMY1				0xfa
#define IM401_DUMMY2				0xfb
#define IM401_DUMMY3				0xfc
#define IM401_VENDOR_ID			0xfd
#define IM401_VENDOR_ID1			0xfe
#define IM401_VENDOR_ID2			0xff

enum {
	IM401_NORMAL,
	IM401_DSP,
	IM401_DSP_ONESHOT,
	IM401_DSP_RELOAD,
	IM401_DSP_IRQRESET,
	IM401_DSP_IRQQUERY,
	IM401_DSP_QUERYFWSTATE,
	IM401_HOST_IRQRESET,
	IM401_HOST_IRQQUERY,
	IM401_QUERY_DUMMY_CUSTOMER14,
	
	IM401_DSP_LOADGRAMMAR,
	IM401_REG_DUMP,
	IM401_DSP_STARTONESHOT_FLAG,
	IM401_DSP_STOPONESHOT_FLAG,
	IM401_DSP_BOOST_RESET,
	IM401_DSP_FACTORY_ENTER,
	IM401_DSP_FACTORY_QUIT,
	IM401_DSP_SVTHD,
};

// Mar.20, 2015 - Henry Zhang
// Defined for im401 register dump function
#define MAX_REG_NUM	151

unsigned short im401_regs[MAX_REG_NUM] = {
	0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x2a, 0x2b,
	0x2c, 0x2d, 0x2e, 0x40, 0x41, 0x42, 0x43, 0x44,
	0x45, 0x46, 0x4c, 0x4d, 0x4e, 0x50, 0x51, 0x52,
	0x53, 0x54, 0x55, 0x57, 0x58, 0x59, 0x5a, 0x5b,
	0x5c, 0x5d, 0x60, 0x61, 0x62, 0x64, 0x65, 0x6a,
	0x6c, 0x73, 0x74, 0x75, 0x76, 0x78, 0x79, 0x7b,
	0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
	0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
	0x8c, 0x8d, 0x8e, 0x90, 0x91, 0x92, 0x93, 0x94,
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c,
	0x9d, 0x9e, 0xc0, 0xc1, 0xc2, 0xd0, 0xd1, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2,
	0xf3, 0xf4, 0xf7, 0xf8, 0xfa, 0xfb, 0xfc, 0xfd,
	0xfe, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

short im401_reg_num = 138;
//short im401_reg_idx = 0;
// End of definition of register dump function

void im401_dsp_stop(void);
void im401_dsp_start(void);
void im401_dsp_one_shot(void);

#endif /*_IM401_H_*/
