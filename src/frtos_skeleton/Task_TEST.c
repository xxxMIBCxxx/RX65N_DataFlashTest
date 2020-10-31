/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "task_function.h"
/* Start user code for import. Do not edit comment generated here */
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "platform.h"
#include "RtcApi.h"
#include "FlashApi.h"
#include "LogApi.h"

uint8_t			g_szBuff[255 + 1];
/* End user code. Do not edit comment generated here */

void Task_TEST(void * pvParameters)
{
/* Start user code for function. Do not edit comment generated here */
	tm_t			tDate;
	//uint32_t		Count = 0;
	uint8_t			ch = 'A';

	// SW
	PORTA.PDR.BIT.B2 = 0;
	PORTA.PCR.BIT.B2 = 1;

	RtcApi_Init();
	FlashApi_Init();
	LogApi_Init();

	RtcApi_Open();
	FlashApi_Open();


//	RtcApi_SetOutput(RTC_OUTPUT_1_HZ);

	memset(&tDate,0x00,sizeof(tDate));
	tDate.tm_year = 120;
	tDate.tm_mon = 9;
	tDate.tm_mday = 31;
	tDate.tm_hour = 15;
	tDate.tm_min = 58;
	tDate.tm_sec = 0;
	tDate.tm_wday = 6;
	RtcApi_SetCurrentTime(&tDate);

	LogApi_Clear();

	while(1)
	{
		// SWが押された場合
		if (PORTA.PIDR.BIT.B2 == 0)
		{
			memset(g_szBuff,0x00,sizeof(g_szBuff));
			for(uint32_t i = 0 ; i < 255 ; i++)
			{
				g_szBuff[i] = ch;
			}
			ch++;
			if (ch > 'Z')
			{
				ch ='A';
			}
			LogApi_Output(__FILE__, __LINE__, g_szBuff );
			vTaskDelay(500);
		}

		vTaskDelay(100);
	}



/* End user code. Do not edit comment generated here */
}
/* Start user code for other. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
