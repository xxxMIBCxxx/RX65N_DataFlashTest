/*
 * LogApi.c
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */
#include "LogApi.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"



// LogApiグローバル変数
LOGAPI_GLOBAL_TABLE					g_tLogApi;


// プロトタイプ宣言
static int8_t logapi_Write(LOGAPI_LOG_INFO_TABLE* ptLogInfo);
static int8_t logapi_SetLogAddress(uint32_t LogAddress);
static int8_t logapi_AllClear(void);
static int8_t logapi_Clear(uint32_t LogAddress, uint32_t NumBlocks);


//============================================================================================================
// LogApi 初期化処理（※必ずシステムがコールすること）
//============================================================================================================
void LogApi_Init(void)
{
	// LogAPI用ミューテックスハンドル生成
	g_tLogApi.MutexHandle = xSemaphoreCreateMutex();
	memset(&g_tLogApi.tHeaderInfo, 0x00, sizeof(g_tLogApi.tHeaderInfo));
	memset(&g_tLogApi.tLogInfo, 0x00, sizeof(g_tLogApi.tLogInfo));
}


//============================================================================================================
// ログ出力
//============================================================================================================
void LogApi_Output(const uint8_t *pszFileName, uint32_t Line, const uint8_t *format,...)
{
	va_list						ap;
	uint8_t*					pPos = NULL;
	uint32_t					len = 0;


	// 引数チェック
	if (format == NULL)
	{
		return;
	}

	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tLogApi.MutexHandle,portMAX_DELAY);

	// ログ情報を初期化
	memset(&g_tLogApi.tHeaderInfo, 0x00, sizeof(g_tLogApi.tHeaderInfo));
	memset(&g_tLogApi.tLogInfo, 0x00, sizeof(g_tLogApi.tLogInfo));

	// ファイル名
	if (pszFileName != NULL)
	{
		pPos = (uint8_t*)strrchr((const char*)pszFileName, '/');
		if (pPos != NULL)
		{
			strncpy((char *)g_tLogApi.tHeaderInfo.szFileName, (const char*)(pPos+1), LOGAPI_FUNC_NAME_SIZE);
		}
		else
		{
			strncpy((char *)g_tLogApi.tHeaderInfo.szFileName, (const char*)pszFileName, LOGAPI_FUNC_NAME_SIZE);
		}
	}

	// ログ出力日時をRTCから取得
	RtcApi_GetCurrentTime(&g_tLogApi.tHeaderInfo.tTm);

	snprintf((char *)g_tLogApi.tLogInfo.LogBuff, LOGAPI_DATA_SIZE_MAX,"%04d/%02d/%02d %02d:%02d:%02d %s(%d) : ",
			(g_tLogApi.tHeaderInfo.tTm.tm_year + 1900), (g_tLogApi.tHeaderInfo.tTm.tm_mon + 1), g_tLogApi.tHeaderInfo.tTm.tm_mday,
			g_tLogApi.tHeaderInfo.tTm.tm_hour, g_tLogApi.tHeaderInfo.tTm.tm_min, g_tLogApi.tHeaderInfo.tTm.tm_sec, g_tLogApi.tHeaderInfo.szFileName, Line);
	len = strlen((const char*)g_tLogApi.tLogInfo.LogBuff);

	// ログ
	va_start(ap, format);
	vsnprintf((char *)&g_tLogApi.tLogInfo.LogBuff[len],(LOGAPI_DATA_SIZE_MAX - len),(const char*)format,ap);
	va_end(ap);

	// ログ書込み
	logapi_Write(&g_tLogApi.tLogInfo);

	xSemaphoreGive(g_tLogApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return;
}


//============================================================================================================
// ログクリア
//============================================================================================================
void LogApi_Clear(void)
{
	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tLogApi.MutexHandle,portMAX_DELAY);

	logapi_AllClear();

	xSemaphoreGive(g_tLogApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return;
}


//============================================================================================================
// ログ書込み
//============================================================================================================
static int8_t logapi_Write(LOGAPI_LOG_INFO_TABLE* ptLogInfo)
{
	int8_t						Result = 0;
	flash_err_t					eFlashResult = FLASH_SUCCESS;
	uint32_t*					pLogAddress = (uint32_t*)FLASH_DF_BLOCK_0;
	uint32_t					LogAddress = 0;
	uint32_t 					NumBlocks = LOGAPI_DATA_SIZE_MAX / FLASHAPI_ERASE_BLOCK_SIZE;


	// 引数チェック
	if (ptLogInfo == NULL)
	{
		return -1;
	}

	// ログ書込み位置取得(データフラッシュの先頭ブロックのアドレス)
	LogAddress = *pLogAddress;
	if ((LogAddress >= FLASH_DF_BLOCK_1) && (LogAddress < FLASH_DF_BLOCK_INVALID))
	{
		if ((LogAddress % FLASHAPI_ERASE_BLOCK_SIZE) != 0)
		{
			LogAddress = FLASH_DF_BLOCK_1;
			//LogAddress = FLASH_DF_BLOCK_505;			// test : 0x00107E40
			//LogAddress = FLASH_DF_BLOCK_506;     		// test : 0x00107E80
			logapi_AllClear();
		}
	}
	else
	{
		LogAddress = FLASH_DF_BLOCK_1;
		//LogAddress = FLASH_DF_BLOCK_505;			// test : 0x00107E40
		//LogAddress = FLASH_DF_BLOCK_506;     		// test : 0x00107E80
		logapi_AllClear();
	}

	// 指定位置のログ情報をクリアする
	Result = logapi_Clear(LogAddress, NumBlocks);
	if (Result != 0)
	{
		goto LogApi_Write_End_Lable;
	}

	// ログ書込み
	eFlashResult = R_FLASH_Write((uint32_t)ptLogInfo,LogAddress,sizeof(LOGAPI_LOG_INFO_TABLE));
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Write Error. [eFlashResult:%d]\n",eFlashResult);
		Result = -1;
		goto LogApi_Write_End_Lable;
	}

	//　ログ出力先アドレスを書込む
	LogAddress += LOGAPI_DATA_SIZE_MAX;
	if (LogAddress > (FLASH_DF_BLOCK_INVALID - LOGAPI_DATA_SIZE_MAX))
	{
		LogAddress = FLASH_DF_BLOCK_1;
	}
	Result = logapi_SetLogAddress(LogAddress);
	if (Result != 0)
	{
		goto LogApi_Write_End_Lable;
	}

	Result = 0;

LogApi_Write_End_Lable:

	return Result;
}


//============================================================================================================
//　ログ出力先アドレスを書込む
//============================================================================================================
static int8_t logapi_SetLogAddress(uint32_t LogAddress)
{
	int8_t						Result = 0;
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// 書込み先をイレース
	Result = logapi_Clear(FLASH_DF_BLOCK_0, 1);
	if (Result != 0)
	{
		return -1;
	}

	// ログ出力先アドレスを書込む
	eFlashResult = FlashApi_Write((uint32_t)&LogAddress,FLASH_DF_BLOCK_0,sizeof(LogAddress));
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("FlashApi_Write Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}


//============================================================================================================
// ログ全クリア
//============================================================================================================
static int8_t logapi_AllClear(void)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;
	uint32_t					NumBlocks = ((FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0) / 64);


	// ログを全てクリア
	eFlashResult = FlashApi_Erase(FLASH_DF_BLOCK_0, NumBlocks);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("FlashApi_Erase Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}


//============================================================================================================
// 指定位置のログ情報をクリアする
//============================================================================================================
static int8_t logapi_Clear(uint32_t LogAddress, uint32_t NumBlocks)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// 指定位置のログ情報をクリア
	eFlashResult = FlashApi_Erase((flash_block_address_t)LogAddress, NumBlocks);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("FlashApi_Erase Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}




