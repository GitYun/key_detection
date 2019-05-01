// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  * @file		key.c
  * @author		Yoki
  * @version	v1.0.0
  * @date		2017-4-16
  * @brief		Button detection functions implement without block delay eliminate jitter
  */

#include "key.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"

#define KEY_NUM_OF_MEANWHILE	(2)		/*!< Number of key active simultaneously, maximum support 3 key simultaneously */
#define KEY_DELAY_TIMEBASE		(10)	/*!< Key delay timebase, use timer interrupt counter */
#define KEY_JITTER_DELAY_COUNT	(3)		/*!< Number of key eliminate jitter delay, timebase 10ms then delay is 30ms */
#define KEY_ALWAYS_DELAY_COUNT	(50)	/*!< Number of press key always delay */

uint32_t g_uwKeyDelayX10ms = 0;			/*!< Timer interrupt key delay counter, timebase can modify */
bool g_bKeyDelayEnable = false;			/*!< Timer interrupt counter enable flag */
bool g_bKeyHoldDelayEnable = false;
bool g_bKeyDownEventSend = false;
bool g_bKeyAlwaysEventSend = false;
RetKeyType_un g_unKeyData;

static uint8_t ReadKey(void);
static uint32_t KeyScan(void);
static void KeyEvent(RetKeyType_un keyData, KeyOperateTpye_e operateType);

static uint8_t ReadKey(void)
{
	uint8_t keyValue;
	static uint8_t keyValueCode[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 }; // Now maximum support 8 key

	if (HAL_GPIO_ReadPin(KU_KEY_GPIO_Port, KU_KEY_Pin) == 0)			keyValue |= keyValueCode[KU_KEY_OK];
	if (HAL_GPIO_ReadPin(KD_KEY_GPIO_Port, KD_KEY_Pin) == 0)			keyValue |= keyValueCode[KD_KEY_OK];
	if (HAL_GPIO_ReadPin(BLACK_KEY_GPIO_Port, BLACK_KEY_Pin) == 0) 		keyValue |= keyValueCode[KL_KEY_OK];
	if (HAL_GPIO_ReadPin(WHITE_KEY_GPIO_Port, WHITE_KEY_Pin) == 0)		keyValue |= keyValueCode[KR_KEY_OK];
	if (HAL_GPIO_ReadPin(VOLUP_KEY_GPIO_Port, VOLUP_KEY_Pin) == 0) 		keyValue |= keyValueCode[ENTER_KEY_OK];
	if (HAL_GPIO_ReadPin(VOLDOWN_KEY_GPIO_Port, VOLDOWN_KEY_Pin) == 0)	keyValue |= keyValueCode[BACK_KEY_OK]; 

	return keyValue;
}

static uint32_t KeyScan(void)
{
	uint8_t keyValue, keyCodeTemp;
	RetKeyType_un ret;
	
	ret.uwReadKeyByWhole = 0xffffff00;	/*!< cortex-M3 default use little-endian mode, MSB(MemH)-->LSB(MemL) */
	keyCodeTemp = ReadKey();

	if (keyCodeTemp != 0)
	{
		for (keyValue = 0; keyValue < KEY_VALUE_INDEX_END; keyValue++)
		{
			if (keyCodeTemp & 0x1)
			{
				ret.ucReadKeyByByte[0]++;
				if (ret.ucReadKeyByByte[0] <= KEY_NUM_OF_MEANWHILE)
					ret.ucReadKeyByByte[ret.ucReadKeyByByte[0]] = (KeyValueIndex_e)keyValue;
			}
			keyCodeTemp >>= 1;
		}
	}

	if ((ret.uwReadKeyByWhole != 0xffffff00) && (ret.ucReadKeyByByte[0] <= KEY_NUM_OF_MEANWHILE))
	{
		return ret.uwReadKeyByWhole;
	}
	else
	{
		return NO_KEY;
	}
}

void keyOperateCmd()
{
	RetKeyType_un keyData;
	
	keyData.uwReadKeyByWhole = KeyScan();
	if (keyData.uwReadKeyByWhole != NO_KEY)
	{
		if (g_bKeyDelayEnable == false)
		{
			g_uwKeyDelayX10ms = KEY_JITTER_DELAY_COUNT;	// delay 30ms eliminate mechanical jitter
			g_unKeyData.uwReadKeyByWhole = keyData.uwReadKeyByWhole;
			g_bKeyDelayEnable = true;					// Enable 10ms timer interrupt delay counter
		}
		
		if (g_uwKeyDelayX10ms == 0)						// end of delay for jitter or press always
		{
			if (keyData.uwReadKeyByWhole == g_unKeyData.uwReadKeyByWhole)
			{
				if (g_bKeyDownEventSend == false)
				{
					g_bKeyDownEventSend = true;
					KeyEvent(g_unKeyData, KEY_DOWN);		// send key pressed event
					return;
				}
					
				if (g_bKeyHoldDelayEnable == false)
				{
					g_bKeyHoldDelayEnable = true;
					g_uwKeyDelayX10ms = KEY_ALWAYS_DELAY_COUNT;	// least hold on KEY_ALWAYS_DELAY_COUNT*KEY_DELAY_TIMEBASE milliseconds					
				}
				else
				{
					g_bKeyAlwaysEventSend = true;
					KeyEvent(g_unKeyData, KEY_ALWAYS);	// send key always pressed envent
				}
			}
			else
			{
				g_bKeyDelayEnable = false;
				g_bKeyHoldDelayEnable = false;
				g_bKeyDownEventSend = false;
				g_bKeyAlwaysEventSend = false;
				g_unKeyData.uwReadKeyByWhole = NO_KEY;
			}
		}
	}
	else
	{
		g_bKeyDelayEnable = false;
		g_bKeyHoldDelayEnable = false;
		g_uwKeyDelayX10ms = 0;		
		
		if (g_bKeyDownEventSend == true || g_bKeyAlwaysEventSend == true)
		{
			g_bKeyDownEventSend = false;
			g_bKeyAlwaysEventSend = false;
			if (g_unKeyData.uwReadKeyByWhole != NO_KEY)
				KeyEvent(g_unKeyData, KEY_UP);		// send key released evnet
		}
		
		g_unKeyData.uwReadKeyByWhole = NO_KEY;
	}
}

__weak void keyDownHook(RetKeyType_un keyData)
{

}

__weak void keyAlwaysDownHook(RetKeyType_un keyData)
{

}

__weak void keyUpHook(RetKeyType_un keyData)
{

}


static void KeyEvent(RetKeyType_un keyData, KeyOperateTpye_e operateType)
{
	switch (operateType)
	{
		case KEY_DOWN:
			keyDownHook(keyData);
			break;
		case KEY_ALWAYS:
			keyAlwaysDownHook(keyData);
			break;
		case KEY_UP:
			keyUpHook(keyData);
			break;
		default:
			break;
	}
}

/**************************************** Copyright (C) 2017 **************************************/
