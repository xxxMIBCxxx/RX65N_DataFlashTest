/*
 * FlashApi.c
 *
 *  Created on: 2020/10/31
 *      Author: MIBC
 */
#include "../Common/FlashApi.h"


// FLASH APIグローバル変数
FLASHAPI_GLOBAL_TABLE						g_tFlashApi;


//============================================================================================================
// FlashApI　初期化処理（※必ずシステムがコールすること）
//============================================================================================================
void FlashApi_Init(void)
{
	// RtcAPI用ミューテックスハンドル生成
	g_tFlashApi.MutexHandle = xSemaphoreCreateMutex();
	g_tFlashApi.bOpen = false;
}


//============================================================================================================
// FlashApI　オープン（※必ずシステムがコールすること）
//============================================================================================================
flash_err_t FlashApi_Open(void)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// 既にオープン済みの場合
	if (g_tFlashApi.bOpen == true)
	{
		eFlashResult = FLASH_SUCCESS;
		goto FlashApi_Open_End_Label;
	}

	// FlashApI　オープン
	eFlashResult = R_FLASH_Open();
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Open Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_Open_End_Label;
	}

	g_tFlashApi.bOpen = true;

FlashApi_Open_End_Label:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}


//============================================================================================================
// FlashApI　クローズ（※必ずシステムがコールすること）
//============================================================================================================
flash_err_t FlashApi_Close(void)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// 既にクローズしている場合
	if (g_tFlashApi.bOpen == false)
	{
		eFlashResult = FLASH_SUCCESS;
		goto FlashApi_Close_End_Label;
	}

	// FlashApI　クローズ
	eFlashResult = R_FLASH_Close();
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Close Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_Close_End_Label;
	}

	g_tFlashApi.bOpen = false;

FlashApi_Close_End_Label:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}


//============================================================================================================
// FlashApI　イレース
//============================================================================================================
flash_err_t FlashApi_Erase(flash_block_address_t BlockStartAddress, uint32_t NumBlocks)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// イレース
	eFlashResult = R_FLASH_Erase(BlockStartAddress, NumBlocks);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Erase Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_Erase_End_Lable;
	}

FlashApi_Erase_End_Lable:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}


//============================================================================================================
// FlashApI　ブランクチェック
//============================================================================================================
flash_err_t FlashApi_BlankCheck(uint32_t Address, uint32_t NumBytes, flash_res_t* peBlankCheckResult)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// 引数チェック
	if (peBlankCheckResult == NULL)
	{
		eFlashResult = FLASH_ERR_PARAM;
		goto FlashApi_BlankCheck_End_Lable;
	}

	// ブランクチェック
	eFlashResult = R_FLASH_BlankCheck(Address, NumBytes, peBlankCheckResult);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_BlankCheck Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_BlankCheck_End_Lable;
	}

FlashApi_BlankCheck_End_Lable:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}


//============================================================================================================
// FlashApI　書込み
//============================================================================================================
flash_err_t FlashApi_Write(uint32_t ScrAddress, uint32_t DestAddress, uint32_t NumBytes)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// 書込み
	eFlashResult = R_FLASH_Write(ScrAddress, DestAddress, NumBytes);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Write Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_Write_End_Lable;
	}

FlashApi_Write_End_Lable:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}


//============================================================================================================
// FlashApI　Control
//============================================================================================================
flash_err_t FlashApi_Control(flash_cmd_t eCmd, void* pCfg)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tFlashApi.MutexHandle,portMAX_DELAY);

	// 引数チェック
	if (pCfg == NULL)
	{
		eFlashResult = FLASH_ERR_PARAM;
		goto FlashApi_Control_End_Lable;
	}

	// Control
	eFlashResult = FlashApi_Control(eCmd, pCfg);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("FlashApi_Control(%d) Error.[eFlashResult:%d]\n",eFlashResult);
		goto FlashApi_Control_End_Lable;
	}

FlashApi_Control_End_Lable:

	xSemaphoreGive(g_tFlashApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return eFlashResult;
}















