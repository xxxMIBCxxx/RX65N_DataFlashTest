/*
 * RtcApi.c
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "RtcApi.h"


// RtcAPIグローバル変数
RTCAPI_GLOBAL_TABLE						g_tRtcApi;


// プロトタイプ宣言
static void rtc_callback(void* pArgs);


//============================================================================================================
// RtcApI　初期化処理（※必ずシステムがコールすること）
//============================================================================================================
void RtcApi_Init(void)
{
	// RtcAPI用ミューテックスハンドル生成
	g_tRtcApi.MutexHandle = xSemaphoreCreateMutex();
	memset(&g_tRtcApi.tRtcInit,0x00,sizeof(rtc_init_t));
	g_tRtcApi.bOpen = false;

}


//============================================================================================================
// RtcApI オープン（※必ずシステムがコールすること）
//============================================================================================================
int8_t RtcApi_Open(void)
{
	int8_t						Result = 0;
	rtc_err_t					eRetResult = RTC_SUCCESS;
	tm_t						tDate;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tRtcApi.MutexHandle,portMAX_DELAY);

	// 既にオープン済みの場合
	if (g_tRtcApi.bOpen == true)
	{
		Result = 0;
		goto RtcApi_Open_End_Lable;
	}

	// RtcApI オープン
	memset(&tDate,0x00,sizeof(tDate));
	memset(&g_tRtcApi.tRtcInit,0x00,sizeof(rtc_init_t));
	g_tRtcApi.tRtcInit.set_time = false;									// RTCの初期化および日時設定(true：実行, false:スキップ)
	g_tRtcApi.tRtcInit.output_freq = RTC_OUTPUT_OFF;						// クロック出力の周波数(set_time = false指定時：設定値無効)
	g_tRtcApi.tRtcInit.periodic_freq = RTC_PERIODIC_OFF;					// 周期割込みの周期を設定
	g_tRtcApi.tRtcInit.periodic_priority = 0;								// 周期割込みの割込み優先レベル(0 - 15, 0:割込み禁止)
	g_tRtcApi.tRtcInit.p_callback = rtc_callback;							// コールバック関数
	eRetResult = R_RTC_Open(&g_tRtcApi.tRtcInit, &tDate);
	if (eRetResult != RTC_SUCCESS)
	{
		printf("R_RTC_Open Error. [eRetResult:%d]\n",eRetResult);
		Result = -1;
		goto RtcApi_Open_End_Lable;
	}

	g_tRtcApi.bOpen = true;
	Result = 0;

RtcApi_Open_End_Lable:

	xSemaphoreGive(g_tRtcApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return Result;
}


//============================================================================================================
// RtcApI クローズ（※必ずシステムがコールすること）
//============================================================================================================
void RtcApi_Close(void)
{
	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tRtcApi.MutexHandle,portMAX_DELAY);

	// 既にクローズ済みの場合
	if (g_tRtcApi.bOpen == false)
	{
		goto RtcApi_Close_End_Lable;
	}

	// RtcApI クローズ
	R_RTC_Close();
	g_tRtcApi.bOpen = false;

RtcApi_Close_End_Lable:

	xSemaphoreGive(g_tRtcApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲
}


//============================================================================================================
// 時刻取得
//============================================================================================================
int8_t RtcApi_GetCurrentTime(tm_t* ptCurrentTime)
{
	int8_t						Result = 0;
	rtc_err_t					eRetResult = RTC_SUCCESS;


	// 引数チェック
	if (ptCurrentTime == NULL)
	{
		return -1;
	}

	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tRtcApi.MutexHandle,portMAX_DELAY);

	// 時刻取得
	eRetResult = R_RTC_Read(ptCurrentTime, FIT_NO_PTR);
	if (eRetResult != RTC_SUCCESS)
	{
		printf("R_RTC_Read Error. [eRetResult:%d]\n",eRetResult);
		Result = -1;
		goto RtcApi_GetCurrentTime_End_Lable;
	}

	Result = 0;

RtcApi_GetCurrentTime_End_Lable:

	xSemaphoreGive(g_tRtcApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return Result;
}


//============================================================================================================
// 時刻設定
//============================================================================================================
int8_t RtcApi_SetCurrentTime(tm_t* ptCurrentTime)
{
	int8_t						Result = 0;
	rtc_err_t					eRetResult = RTC_SUCCESS;


	// 引数チェック
	if (ptCurrentTime == NULL)
	{
		return -1;
	}

	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tRtcApi.MutexHandle,portMAX_DELAY);

	// 時刻設定
	eRetResult = R_RTC_Control(RTC_CMD_SET_CURRENT_TIME, ptCurrentTime);
	if (eRetResult != RTC_SUCCESS)
	{
		printf("R_RTC_Control[RTC_CMD_SET_CURRENT_TIME] Error. [eRetResult:%d]\n",eRetResult);
		Result = -1;
		goto RtcApi_SetCurrentTime_End_Lable;
	}

	Result = 0;

RtcApi_SetCurrentTime_End_Lable:

	xSemaphoreGive(g_tRtcApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return Result;
}


//============================================================================================================
// クロック出力設定
//============================================================================================================
int8_t RtcApi_SetOutput(rtc_output_t eRtcOutput)
{
	int8_t						Result = 0;
	rtc_err_t					eRetResult = RTC_SUCCESS;


	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tRtcApi.MutexHandle,portMAX_DELAY);

	// クロック出力設定
	eRetResult = R_RTC_Control(RTC_CMD_SET_OUTPUT, &eRtcOutput);
	if (eRetResult != RTC_SUCCESS)
	{
		printf("R_RTC_Control[RTC_CMD_SET_OUTPUT] Error. [eRetResult:%d]\n",eRetResult);
		Result = -1;
		goto RtcApi_SetOutput_End_Lable;
	}

	Result = 0;

RtcApi_SetOutput_End_Lable:

	xSemaphoreGive(g_tRtcApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return Result;
}


//============================================================================================================
// RTCコールバック
//============================================================================================================
static void rtc_callback(void* pArgs)
{
}
