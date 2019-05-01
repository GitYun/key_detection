// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  * @file		key.h
  * @author		Yoki
  * @version	v1.0.0
  * @date		2017-4-16
  * @brief		Button detection header file
  */

#ifndef KEY_H
#define KEY_H

#ifdef __cplusplus
extern "C"	{
#endif

#include <stdint.h>

#define NO_KEY	0xFFFFFFFF

typedef enum _KeyValueIndex {
	KU_KEY_OK = 0,
	KD_KEY_OK,
	KL_KEY_OK,
	KR_KEY_OK,
	ENTER_KEY_OK,
	BACK_KEY_OK,

	// Now maximum support 8 key
	KEY_VALUE_INDEX_END = 8,
} KeyValueIndex_e;

typedef enum _KeyOperateType {
	KEY_DOWN = 0x10,
	KEY_ALWAYS = 0x20,
	KEY_UP = 0x40,
}	KeyOperateTpye_e;

typedef union _RetKeyType {
	uint32_t uwReadKeyByWhole;

	/* [0]Byte as index
	 * [3]-[1]Byte store key value
	 */
	uint8_t ucReadKeyByByte[4];
} RetKeyType_un;

void keyOperateCmd(void);

#ifdef __cplusplus
extern }
#endif

#endif

/**************************************** Copyright (C) 2017 **************************************/
