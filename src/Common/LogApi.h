/*
 * LogApi.h
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */

#ifndef LOGAPI_H_
#define LOGAPI_H_
#include "FreeRTOS.h"
#include "semphr.h"
#include "RtcApi.h"
#include "FlashApi.h"

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define LOGAPI_DATA_SIZE_MAX				( (FLASHAPI_ERASE_BLOCK_SIZE * 2) )
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define LOGAPI_FUNC_NAME_SIZE				( 32 )


//------------------------------------------------
// ログヘッダー情報構造体
//------------------------------------------------
typedef struct
{
	tm_t								tTm;										// 発生日時
	uint8_t								szFileName[LOGAPI_FUNC_NAME_SIZE + 1];		// ファイル名
//	uint32_t							Line;										// 行数

} LOGAPI_HEADER_INFO_TABLE;

typedef struct
{
	uint8_t								LogBuff[LOGAPI_DATA_SIZE_MAX];				// ログ出力バッファ

} LOGAPI_LOG_INFO_TABLE;


typedef struct
{
	QueueHandle_t						MutexHandle;								// LOGAPI用ミューテックスハンドル
	LOGAPI_HEADER_INFO_TABLE			tHeaderInfo;								// ログヘッダー情報
	LOGAPI_LOG_INFO_TABLE				tLogInfo;									// ログ情報

} LOGAPI_GLOBAL_TABLE;


//============================================================================================================
// LogApi 初期化処理（※必ずシステムがコールすること）
//============================================================================================================
void LogApi_Init(void);


//============================================================================================================
// ログ出力
//============================================================================================================
void LogApi_Output(const uint8_t *pszFileName, uint32_t Line, const uint8_t *format,...);


//============================================================================================================
// ログクリア
//============================================================================================================
void LogApi_Clear(void);


#endif /* LOGAPI_H_ */
