/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2007
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/


#include <mach/mt3351_typedefs.h>
#include <mach/mt3351_reg_base.h>
#include <mach/mt3351_pdn_hw.h>
#include <mach/mt3351_pdn_sw.h>

void set_DVFS_UPDATE(kal_bool enable)
{
	kal_uint32 tmp = DRV_Reg32(CLKCON);
	if (enable)
	{
		tmp |= DVFS_UPD;
		DRV_WriteReg32(CLKCON, tmp);
	}
	else
	{
		tmp &= ~DVFS_UPD;
		DRV_WriteReg32(CLKCON, tmp);
	}		

}

void Set_CLK_DLY(UINT8 delay)
{
	kal_uint32 tmp = DRV_Reg32(CLKCON);
	tmp &= ~ (CLK_DLY_MASK);
	tmp |= delay << 20;
	DRV_WriteReg32(CLKCON, tmp);

}

void Set_CLK_CONTROL(EMI_CLK_DIVISOR emi_div, ARM_CLK_DIVISOR arm_div)
{
	kal_uint32 tmp = DRV_Reg32(CLKCON);
	tmp &= ~ (EMI_CLKDIV_MASK|ARM_CLKDIV_MASK);
	tmp |= ((emi_div << 18) | (arm_div << 16));
	DRV_WriteReg32(CLKCON, tmp);

}

void Choose_ARM_CLK_SRC(MCU_CLK_SRC clk)
{    
	kal_uint32 tmp = DRV_Reg32(CLKCON);

	tmp &= ~ARMCKSRC_MASK;
	switch(clk)
	{
		case MCU_CLK_EXTERNAL:
			tmp |= (MCU_CLK_EXTERNAL << 4) ;
		break;
		case MCU_CLK_UPLL:
			tmp |= (MCU_CLK_UPLL << 4);
		break;
		case MCU_CLK_MPLL:
			tmp |= (MCU_CLK_MPLL << 4);
		break;
		case MCU_CLK_APLL:
			tmp |= (MCU_CLK_APLL << 4);			
		break;
	}
	DRV_WriteReg32(CLKCON, tmp);

}

void Choose_MM_CLK_SRC(MM_CLK_SRC clk)
{    
	kal_uint32 tmp = DRV_Reg32(CLKCON);

	tmp &= ~MMCKSRC_MASK;
	if(clk)
		tmp |= (MPLL_DIVIDE_2 << 6);		

	DRV_WriteReg32(CLKCON, tmp);

}

void ConfigGMCAsyncMode(void)
{
	kal_uint16 tmp = DRV_Reg(GMC_BASE);
	tmp |= 0x00000008;
	DRV_WriteReg(GMC_BASE, tmp);
}


void ARM_DCM_Enable(kal_bool enable)
{
	kal_uint32 tmp = DRV_Reg32(HW_MISC);
	if (enable)
	{
		tmp |= ARMDCM;
		DRV_WriteReg32(HW_MISC, tmp);
	}
	else
	{
		tmp &= ~ARMDCM;
		DRV_WriteReg32(HW_MISC, tmp);
	}		
}

void ARM_CLK_GATE_Enable(kal_bool enable)
{
	kal_uint32 tmp = DRV_Reg32(HW_MISC);
	if (enable)
	{
		tmp |= ARMCG;
		DRV_WriteReg32(HW_MISC, tmp);
	}
	else
	{
		tmp &= ~ARMCG;
		DRV_WriteReg32(HW_MISC, tmp);
	}		
}

void Set_MCU_MM_Mode(MCU_MODE mode)
{
	kal_uint32 tmp = DRV_Reg32(HW_MISC);


	if (mode) //async mode
	{
		tmp |= ASYNC;
	}
	else //sync mode
	{
		tmp &= ~ASYNC;
	}		
	DRV_WriteReg32(HW_MISC, tmp);
	
}

kal_bool PDN_Get_Peri_Status (PDNCONA_MODE mode)
{
	if(DRV_Reg32(PDNCONA) & (1 << mode))
		return KAL_TRUE;    // this module is POWER DOWN
	else
		return KAL_FALSE;   // this module is POWER UP
}

kal_bool PDN_Get_MM_Status (PDNCONB_MODE mode)
{
	if(DRV_Reg32(PDNCONB) & (1 << mode))
		return KAL_TRUE;    // this module is POWER DOWN
	else
		return KAL_FALSE;   // this module is POWER UP
}

#if 0
void PDN_Power_CONA_DOWN(PDNCONA_MODE module, kal_bool enable)
{
	kal_uint32 tmp = (1 << module);
	if (enable) //power down
		DRV_WriteReg32(PDNSETA, tmp);
	else //power up
    	DRV_WriteReg32(PDNCLRA, tmp);		
}

void PDN_Power_CONB_DOWN(PDNCONB_MODE module, kal_bool enable)
{
	kal_uint32 tmp = (1 << module);
	if (enable) //power down
		DRV_WriteReg32(PDNSETB, tmp);
	else //power up
    	DRV_WriteReg32(PDNCLRB, tmp);		
}

void PDN_Power_DSP(kal_bool enable)
{
    kal_uint32 tmp = DRV_Reg32(CGCON);

	if (enable)
	{ 
	    //power up
        tmp &= ~(0x3 << 2);
        DRV_WriteReg32(CGCON, tmp);
	}
	else
	{
	     //power down
        tmp |= (1 << 2);
    	DRV_WriteReg32(CGCON, tmp);
    }
}
#endif

void CONFIG_DCM(kal_bool enable, DCM_FSEL sel)
{
    kal_uint32 tmp = DRV_Reg32(CGCON);
    kal_uint32 tmp2 = DRV_Reg32(CLKCON);

	tmp2 &= ~ DCM_FSEL_MASK;
		
	if (enable)
	{ 
		tmp2 |= sel;
        DRV_WriteReg32(CLKCON, tmp2);

		tmp |= DCM_EN;        
        DRV_WriteReg32(CGCON, tmp);
	}
	else
	{
		tmp &= ~DCM_EN;        
        DRV_WriteReg32(CGCON, tmp);
    }
}



