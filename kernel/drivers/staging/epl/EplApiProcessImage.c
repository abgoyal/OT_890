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

/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      openPOWERLINK

  Description:  source file for EPL API module (process image)

  License:

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of SYSTEC electronic GmbH nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without prior written permission. For written
       permission, please contact info@systec-electronic.com.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Severability Clause:

        If a provision of this License is or becomes illegal, invalid or
        unenforceable in any jurisdiction, that shall not affect:
        1. the validity or enforceability in that jurisdiction of any other
           provision of this License; or
        2. the validity or enforceability in other jurisdictions of that or
           any other provision of this License.

  -------------------------------------------------------------------------

                $RCSfile: EplApiProcessImage.c,v $

                $Author$

                $Revision$  $Date$

                $State: Exp $

                Build Environment:
                    GCC V3.4

  -------------------------------------------------------------------------

  Revision History:

  2006/10/10 d.k.:   start of the implementation, version 1.00

****************************************************************************/

#include "Epl.h"
//#include "kernel/EplPdokCal.h"

#if (TARGET_SYSTEM == _LINUX_) && defined(__KERNEL__)
#include <asm/uaccess.h>
#endif

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// local types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// modul globale vars
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          C L A S S  EplApi                                              */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
//
// Description:
//
//
/***************************************************************************/

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// local types
//---------------------------------------------------------------------------

#if ((EPL_API_PROCESS_IMAGE_SIZE_IN > 0) || (EPL_API_PROCESS_IMAGE_SIZE_OUT > 0))
typedef struct {
#if EPL_API_PROCESS_IMAGE_SIZE_IN > 0
	BYTE m_abProcessImageInput[EPL_API_PROCESS_IMAGE_SIZE_IN];
#endif
#if EPL_API_PROCESS_IMAGE_SIZE_OUT > 0
	BYTE m_abProcessImageOutput[EPL_API_PROCESS_IMAGE_SIZE_OUT];
#endif

} tEplApiProcessImageInstance;

//---------------------------------------------------------------------------
// local vars
//---------------------------------------------------------------------------

static tEplApiProcessImageInstance EplApiProcessImageInstance_g;
#endif

//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------

//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//

//---------------------------------------------------------------------------
//
// Function:    EplApiProcessImageSetup()
//
// Description: sets up static process image
//
// Parameters:  (none)
//
// Returns:     tEplKernel              = error code
//
//
// State:
//
//---------------------------------------------------------------------------

tEplKernel PUBLIC EplApiProcessImageSetup(void)
{
	tEplKernel Ret = kEplSuccessful;
#if ((EPL_API_PROCESS_IMAGE_SIZE_IN > 0) || (EPL_API_PROCESS_IMAGE_SIZE_OUT > 0))
	unsigned int uiVarEntries;
	tEplObdSize ObdSize;
#endif

#if EPL_API_PROCESS_IMAGE_SIZE_IN > 0
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN;
	ObdSize = 1;
	Ret = EplApiLinkObject(0x2000,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);

	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN;
	ObdSize = 1;
	Ret = EplApiLinkObject(0x2001,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 2;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN / ObdSize;
	Ret = EplApiLinkObject(0x2010,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 2;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN / ObdSize;
	Ret = EplApiLinkObject(0x2011,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 4;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN / ObdSize;
	Ret = EplApiLinkObject(0x2020,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 4;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_IN / ObdSize;
	Ret = EplApiLinkObject(0x2021,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageInput, &uiVarEntries, &ObdSize,
			       1);
#endif

#if EPL_API_PROCESS_IMAGE_SIZE_OUT > 0
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT;
	ObdSize = 1;
	Ret = EplApiLinkObject(0x2030,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);

	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT;
	ObdSize = 1;
	Ret = EplApiLinkObject(0x2031,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 2;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT / ObdSize;
	Ret = EplApiLinkObject(0x2040,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 2;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT / ObdSize;
	Ret = EplApiLinkObject(0x2041,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 4;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT / ObdSize;
	Ret = EplApiLinkObject(0x2050,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);

	ObdSize = 4;
	uiVarEntries = EPL_API_PROCESS_IMAGE_SIZE_OUT / ObdSize;
	Ret = EplApiLinkObject(0x2051,
			       EplApiProcessImageInstance_g.
			       m_abProcessImageOutput, &uiVarEntries, &ObdSize,
			       1);
#endif

	return Ret;
}

//----------------------------------------------------------------------------
// Function:    EplApiProcessImageExchangeIn()
//
// Description: replaces passed input process image with the one of EPL stack
//
// Parameters:  pPI_p                   = input process image
//
// Returns:     tEplKernel              = error code
//
// State:
//----------------------------------------------------------------------------

tEplKernel PUBLIC EplApiProcessImageExchangeIn(tEplApiProcessImage * pPI_p)
{
	tEplKernel Ret = kEplSuccessful;

#if EPL_API_PROCESS_IMAGE_SIZE_IN > 0
#if (TARGET_SYSTEM == _LINUX_) && defined(__KERNEL__)
	copy_to_user(pPI_p->m_pImage,
		     EplApiProcessImageInstance_g.m_abProcessImageInput,
		     min(pPI_p->m_uiSize,
			 sizeof(EplApiProcessImageInstance_g.
				m_abProcessImageInput)));
#else
	EPL_MEMCPY(pPI_p->m_pImage,
		   EplApiProcessImageInstance_g.m_abProcessImageInput,
		   min(pPI_p->m_uiSize,
		       sizeof(EplApiProcessImageInstance_g.
			      m_abProcessImageInput)));
#endif
#endif

	return Ret;
}

//----------------------------------------------------------------------------
// Function:    EplApiProcessImageExchangeOut()
//
// Description: copies passed output process image to EPL stack.
//
// Parameters:  pPI_p                   = output process image
//
// Returns:     tEplKernel              = error code
//
// State:
//----------------------------------------------------------------------------

tEplKernel PUBLIC EplApiProcessImageExchangeOut(tEplApiProcessImage * pPI_p)
{
	tEplKernel Ret = kEplSuccessful;

#if EPL_API_PROCESS_IMAGE_SIZE_OUT > 0
#if (TARGET_SYSTEM == _LINUX_) && defined(__KERNEL__)
	copy_from_user(EplApiProcessImageInstance_g.m_abProcessImageOutput,
		       pPI_p->m_pImage,
		       min(pPI_p->m_uiSize,
			   sizeof(EplApiProcessImageInstance_g.
				  m_abProcessImageOutput)));
#else
	EPL_MEMCPY(EplApiProcessImageInstance_g.m_abProcessImageOutput,
		   pPI_p->m_pImage,
		   min(pPI_p->m_uiSize,
		       sizeof(EplApiProcessImageInstance_g.
			      m_abProcessImageOutput)));
#endif
#endif

	return Ret;
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   F U N C T I O N S                              //
//                                                                         //
//=========================================================================//

// EOF
