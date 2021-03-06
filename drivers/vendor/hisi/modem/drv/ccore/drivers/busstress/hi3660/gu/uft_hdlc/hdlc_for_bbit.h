//#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "baseaddr.h"
#include "BasicFunc.h"
#include "common.h"
#include "mem_fun.h"
//#include "../sc.h"


#ifndef  __HDLC_H_
#define  __HDLC_H_

//#define	 _UNIT_DEBUG

//#define  _SRAM0_TEST
//#define  _MDDR_TEST

/* #define _ADDR_OFFSET
#ifdef _ADDR_OFFSET
#define EXT_FRM_IN_MAIN_LLI_ADDR_OFFSET 0x4
#define EXT_FRM_IN_SUB_LLI_ADDR_OFFSET 0x4
#define EXT_FRM_OUT_LLI_ADDR_OFFSET 0x4
#define EXT_FRM_OUT_SPC_ADDR_OFFSET 0x1
#define	EXT_FRM_RPT_ADDR_OFFSET 0x4

#define	EXT_DEF_IN_LLI_ADDR_OFFSET 0x4
#define EXT_DEF_OUT_SPC_ADDR_OFFSET 0x4
#define EXT_DEF_RPT_ADDR_OFFSET 0x4

#else
#define EXT_FRM_IN_MAIN_LLI_ADDR_OFFSET 0x0
#define EXT_FRM_IN_SUB_LLI_ADDR_OFFSET 0x0
#define EXT_FRM_OUT_LLI_ADDR_OFFSET 0x0
#define EXT_FRM_OUT_SPC_ADDR_OFFSET 0x0
#define	EXT_FRM_RPT_ADDR_OFFSET 0x0

#define	EXT_DEF_IN_LLI_ADDR_OFFSET 0x0
#define EXT_DEF_OUT_SPC_ADDR_OFFSET 0x0
#define EXT_DEF_RPT_ADDR_OFFSET 0x0

#endif

//#include  "Sft_Module_BaseAddr.h"


#define  HDLC_Test_Num  500

#ifdef _SRAM0_TEST
#define  EXT_FRM_RAW_DATA_ADDR		SRAM0_BASEADDR
#define  EXT_FRM_CHECK_DATA_ADDR		SRAM0_BASEADDR + 0x10000
#define  EXT_FRM_OUT_SPC_ADDR			SRAM0_BASEADDR + 0x20000+EXT_FRM_OUT_SPC_ADDR_OFFSET
#define  EXT_FRM_IN_MAIN_LLI_ADDR		SRAM0_BASEADDR + 0x30000+EXT_FRM_IN_MAIN_LLI_ADDR_OFFSET
#define  EXT_FRM_IN_SUB_LLI_ADDR		SRAM0_BASEADDR + 0x34000+EXT_FRM_IN_SUB_LLI_ADDR_OFFSET
#define  EXT_FRM_OUT_LLI_ADDR			SRAM0_BASEADDR + 0x38000+EXT_FRM_OUT_LLI_ADDR_OFFSET
#define  EXT_FRM_RPT_ADDR				SRAM0_BASEADDR + 0x3C000+EXT_FRM_RPT_ADDR_OFFSET

#define  EXT_DEF_RAW_DATA_ADDR		SRAM0_BASEADDR + 0x40000
#define  EXT_DEF_CHECK_DATA_ADDR	SRAM0_BASEADDR + 0x50000
#define  EXT_DEF_OUT_SPC_ADDR			SRAM0_BASEADDR + 0x60000+EXT_DEF_OUT_SPC_ADDR_OFFSET
#define  EXT_DEF_IN_LLI_ADDR			SRAM0_BASEADDR + 0x70000+EXT_DEF_IN_LLI_ADDR_OFFSET
#define  EXT_DEF_RPT_ADDR				SRAM0_BASEADDR + 0x74000+EXT_DEF_RPT_ADDR_OFFSET
#endif


#ifdef _MDDR_TEST
#define  EXT_FRM_RAW_DATA_ADDR			MDDR_BASEADDR + 0x00000
#define  EXT_FRM_CHECK_DATA_ADDR		MDDR_BASEADDR + 0x08000
#define  EXT_FRM_OUT_SPC_ADDR			MDDR_BASEADDR + 0x10000+EXT_FRM_OUT_SPC_ADDR_OFFSET
#define  EXT_FRM_IN_MAIN_LLI_ADDR		MDDR_BASEADDR + 0x18000+EXT_FRM_IN_MAIN_LLI_ADDR_OFFSET
#define  EXT_FRM_IN_SUB_LLI_ADDR		MDDR_BASEADDR + 0x19000+EXT_FRM_IN_SUB_LLI_ADDR_OFFSET
#define  EXT_FRM_OUT_LLI_ADDR			MDDR_BASEADDR + 0x1a000+EXT_FRM_OUT_LLI_ADDR_OFFSET
#define  EXT_FRM_RPT_ADDR				MDDR_BASEADDR + 0x1b000+EXT_FRM_RPT_ADDR_OFFSET

#define  EXT_DEF_RAW_DATA_ADDR			MDDR_BASEADDR + 0x1c000
#define  EXT_DEF_CHECK_DATA_ADDR		MDDR_BASEADDR + 0x24000
#define  EXT_DEF_OUT_SPC_ADDR			MDDR_BASEADDR + 0x2c000+EXT_DEF_OUT_SPC_ADDR_OFFSET
#define  EXT_DEF_IN_LLI_ADDR			MDDR_BASEADDR + 0x34000+EXT_DEF_IN_LLI_ADDR_OFFSET
#define  EXT_DEF_RPT_ADDR				MDDR_BASEADDR + 0x35000+EXT_DEF_RPT_ADDR_OFFSET
#endif */


//*******************************
// Define registers' offset
//*******************************
#define BASE_ADDR             	HDLC_BASEADDR

//#ifdef HIHDLCV200
//#define BASE_ADDR          	0x5f0c0000
//#endif

//registers' offset
//general
#define  STATE_SW_RST			BASE_ADDR + 0x00
#define  PRIOR_TIMEOUT_CTRL		BASE_ADDR + 0x04
#define  RD_ERR_CURR_ADDR		BASE_ADDR + 0x08
#define  WR_ERR_CURR_ADDR		BASE_ADDR + 0x0c

//framer
#define  HDLC_FRM_EN			BASE_ADDR + 0x10
#define  HDLC_FRM_RAW_INT		BASE_ADDR + 0x14
#define  HDLC_FRM_INT_STATUS	BASE_ADDR + 0x18
#define  HDLC_FRM_INT_CLR		BASE_ADDR + 0x1c
#define  HDLC_FRM_CFG			BASE_ADDR + 0x20
#define  HDLC_FRM_ACCM			BASE_ADDR + 0x24
#define  HDLC_FRM_STATUS		BASE_ADDR + 0x28
#define  FRM_IN_LLI_ADDR		BASE_ADDR + 0x30
#define  FRM_IN_SUBLLI_ADDR		BASE_ADDR + 0x34
#define  FRM_IN_PKT_LEN			BASE_ADDR + 0x38
#define  FRM_IN_BLK_ADDR		BASE_ADDR + 0x3c
#define  FRM_IN_BLK_LEN			BASE_ADDR + 0x40
#define  FRM_OUT_LLI_ADDR		BASE_ADDR + 0x44
#define  FRM_OUT_SPC_ADDR		BASE_ADDR + 0x48
#define  FRM_OUT_SPC_DEP		BASE_ADDR + 0x4c
#define  FRM_RPT_ADDR			BASE_ADDR + 0x50
#define  FRM_RPT_DEP			BASE_ADDR + 0x54
//deframer
#define  HDLC_DEF_EN			BASE_ADDR + 0x60
#define  HDLC_DEF_RAW_INT		BASE_ADDR + 0x64
#define  HDLC_DEF_INT_STATUS	BASE_ADDR + 0x68
#define  HDLC_DEF_INT_CLR		BASE_ADDR + 0x6c
#define  HDLC_DEF_CFG			BASE_ADDR + 0x70
#define  DEF_UNCOMPLETED_LEN	BASE_ADDR + 0x74
#define  DEF_UNCOMPLETED_PRO	BASE_ADDR + 0x78
#define  DEF_UNCOMPLETED_ADDR	BASE_ADDR + 0x7c
#define  DEF_UNCOMPLETED_ST_AGO	BASE_ADDR + 0x80
#define  HDLC_DEF_GO_ON			BASE_ADDR + 0x84
#define  HDLC_DEF_STATUS		BASE_ADDR + 0x88
#define  DEF_UNCOMPLETED_ST_NOW	BASE_ADDR + 0x8c
#define  DEF_IN_LLI_ADDR		BASE_ADDR + 0x90
#define  DEF_IN_PKT_ADDR		BASE_ADDR + 0x94
#define  DEF_IN_PKT_LEN			BASE_ADDR + 0x98
#define  DEF_IN_PKT_LEN_MAX		BASE_ADDR + 0x9c
#define  DEF_OUT_SPC_ADDR		BASE_ADDR + 0xa0
#define  DEF_OUT_SPC_DEP		BASE_ADDR + 0xa4
#define  DEF_RPT_ADDR			BASE_ADDR + 0xa8
#define  DEF_RPT_DEP			BASE_ADDR + 0xac
#define  HDLC_DEF_ERR_INFO_0	BASE_ADDR + 0xb0
#define  HDLC_DEF_ERR_INFO_1	BASE_ADDR + 0xb4
#define  HDLC_DEF_ERR_INFO_2	BASE_ADDR + 0xb8
#define  HDLC_DEF_ERR_INFO_3	BASE_ADDR + 0xbc
#define  DEF_INFO_FRL_CNT_AGO	BASE_ADDR + 0xc0
#define  DEF_INFO_FRL_CNT_NOW	BASE_ADDR + 0xc4

int hdlcv200_frm_def_test_bbit(UINT32 addr_src_hdlc,UINT32 addr_des_hdlc);
#endif
