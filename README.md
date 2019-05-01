#### The code snippet for button detection without block delay eliminate jitter for STM32
---

##### Usage
- Modify eliminate jitter delay timebase in header file:
	`#define KEY_DELAY_TIMEBASE	(10)`

- In STM32 Systick interrupt ISR, each eliminate jitter delay timebase decrement once variable `g_uwKeyDelayX10ms`.

- Loop call the function `keyOperateCmd()`, example for in `main()` robin-round it.

- Implement 3 funtions with prototype: 
	- `void keyUpHook(RetKeyType_un keyData)`
	- `void keyDownHook(RetKeyType_un keyData)`
	- `void keyAlwaysDownHook(RetKeyType_un keyData)`

  Example for:
  ```C
  void KeyDownHook(RetKeyType_un keyData)
  {
  	switch (keyData.ucReadKeyByByte[0])
  	{
  		// Only a button be pressed
  		case 1:
  			switch (keyData.ucReadKeyByByte[1])
  			{
  				case KU_KEY_OK:
  				// Up button be pressed handle code...
  				break;

  				case KD_KEY_OK:
  				// Down button be pressed handle code...
  				break;

  				case KL_KEY_OK:
  				// Left button be pressed handle code...
  				break;

  				case KR_KEY_OK:
  				// Right button be pressed handle code...
  				break;

  				case ENTER_KEY_OK:
  				// Enter button be pressed handle code...
  				break;

  				case BACK_KEY_OK:
  				// Back button be pressed handle code...
  				break;

  				default:
  				break;
  			}
  		break;

  		// Two buttons are pressed simultaneously
  		case 2:
  		// Here has 6 buttons, each 2 buttons be pressed simultaneously,
  		// so there are 15 different combinations
  		switch (keyData.ucReadKeyByByte[1] + keyData.ucReadKeyByByte[2])
  		{
  			// KU_KEY_OK + KD_KEY_OK
  			case KD_KEY_OK:
  			break;

  			...
  		}
  		break;

  		// Other...
  		default:
  		break;
  	}
  }
  ```

### If you have any suggestions or questions, please send an email to youneihai@126.com or youneihai@gmail.com