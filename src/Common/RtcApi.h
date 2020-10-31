/*
 * RtcApi.h
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */

#ifndef RTCAPI_H_
#define RTCAPI_H_
#include "FreeRTOS.h"
#include "semphr.h"
#include "r_rtc_rx_if.h"								// RTC I/F


typedef struct
{
	QueueHandle_t						MutexHandle;						// RtcAPI用ミューテックスハンドル
	rtc_init_t							tRtcInit;							// RTC初期設定構造体
	bool								bOpen;								// オープンフラグ(false:未オープン, true：オープン)

} RTCAPI_GLOBAL_TABLE;


//============================================================================================================
// RtcApI　初期化処理（※必ずシステムがコールすること）
//============================================================================================================
void RtcApi_Init(void);


//============================================================================================================
// RtcApI オープン（※必ずシステムがコールすること）
//============================================================================================================
int8_t RtcApi_Open(void);


//============================================================================================================
// RtcApI クローズ（※必ずシステムがコールすること）
//============================================================================================================
void RtcApi_Close(void);


//============================================================================================================
// 時刻取得
//============================================================================================================
int8_t RtcApi_GetCurrentTime(tm_t* ptCurrentTime);


//============================================================================================================
// 時刻設定
//============================================================================================================
int8_t RtcApi_SetCurrentTime(tm_t* ptCurrentTime);


//============================================================================================================
// クロック出力設定
//============================================================================================================
int8_t RtcApi_SetOutput(rtc_output_t eRtcOutput);


#endif /* RTCAPI_H_ */