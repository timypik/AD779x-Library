/**
  ******************************************************************************
  * @file    ad779x.c
  * @author  Khusainov Timur
  * @version 0.0.0.1
  * @date    10.10.2011
  * @brief   Analog Devices Sigma-Delta ADC AD7792/AD7793 driver (for multiple devices)
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "ad779x.h"

/* Reset state of IO register */
const tAD779X_IORegister gIOReg =
{
	.IEXCEN  = csvDisable, /*!< excitation currents disabled  */
	.IEXCDIR = csdNormal,  /*!< default state */
	.UNUSED  = 0           /*!< must be clear */
};

/* Reset state of MODE register */
const tAD779X_ModeRegister gModeReg =
{
	.MODE = mdsIdle,     /*!< set Idle mode */
	.CLKS = cssInt,      /*!< set internal clock source */
	.FS   = fs16_7_65dB, /*!< 16.7Hz (65dB - 50/60Hz) */
	.UNUSED0 = 0,        /*!< must be clear */
	.UNUSED1 = 0         /*!< must be clear */
};

/**
 * @brief  Init HW and reset ADC
 * @param  None
 * @return None
 */
void AD779X_Init(tAD779X_Device *pDevice)
{
	/* reset ADC */
	//AD779X_Reset(pDevice);
	
	/* Detect type of ADC */
	if (AD779X_HWDetect(pDevice))
	{
		/* set default settings: continuous conversion, internal clock source, 16.7Hz (65dB) */
		AD779X_WriteModeRegister(pDevice, gModeReg.DATA);
		
		/* set default settings: excitation currents disabled */
		AD779X_WriteIORegister(pDevice, gIOReg.DATA);
		
		/* store startup state */
		pDevice->SuState = susActivate;
	}
	else
	{
		/* store startup state */
		pDevice->SuState = susNoHW;
	}
}

/**
 * @brief  Reset ADC
 * @param  None
 * @return None
 */
void AD779X_Reset(tAD779X_Device *pDevice)
{
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* make 32 clk, while dout -> 1 */
	pDevice->TxByte(0xFF);
	pDevice->TxByte(0xFF);
	pDevice->TxByte(0xFF);
	pDevice->TxByte(0xFF);
	
	/* wait until ADC will restart */
	//while(pDevice->RDYState() == rdsBusy);
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
}

/**
 * @brief  Write value in MODE register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteModeRegister(tAD779X_Device *pDevice, unsigned short Data)
{
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: write MODE register */
	pDevice->TxByte(AD779X_WRR_MODE);
	
	/* write data to register */
	pDevice->TxByte(Data >> 8);
	pDevice->TxByte(Data & 0x00FF);
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
}

/**
 * @brief  Write value in CONFIG register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteConfigRegister(tAD779X_Device *pDevice, unsigned short Data)
{
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: write CONFIG register */
	pDevice->TxByte(AD779X_WRR_CONFIG);
	
	/* write data to register */
	pDevice->TxByte(Data >> 8);
	pDevice->TxByte(Data & 0x00FF);
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
}

/**
 * @brief  Write value in IO register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteIORegister(tAD779X_Device *pDevice, unsigned char Data)
{
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: write IO register */
	pDevice->TxByte(AD779X_WRR_IO);
	
	/* write data to register */
	pDevice->TxByte(Data);
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
}

/**
 * @brief  Detect ADC type
 * @param  None
 * @return Model as status of detecting
 */
unsigned short AD779X_HWDetect(tAD779X_Device *pDevice)
{
	unsigned short m_adc_model;
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: read ID register */
	pDevice->TxByte(AD779X_RDR_ID);
	
	/* get ID */
	m_adc_model = pDevice->RxByte();
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	switch (m_adc_model & 0xF)
	{
		case AD7792_PARTID:
			pDevice->Model = ad7792;
		break;
		
		case AD7793_PARTID:
			pDevice->Model = ad7793;
		break;
		
		default:
			pDevice->Model = adNone;
		break;
	}
	
	return pDevice->Model;
}

/**
 * @brief  Get ADC status
 * @param  None
 * @return Value as status
 */
unsigned char AD779X_GetStatus(tAD779X_Device *pDevice)
{
	unsigned char m_adc_status;
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: read STATUS register */
	pDevice->TxByte(AD779X_RDR_STATUS);
	
	/* get value */
	m_adc_status = pDevice->RxByte();
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	return m_adc_status;
}

/**
 * @brief  Check ready status by STATUS_REG
 * @param  None
 * @return true - ready, false - not ready
 */
unsigned char AD779X_CheckReadySW(tAD779X_Device *pDevice)
{
	return (~AD779X_GetStatus(pDevice) & AD779X_SR_RDY);
}

/**
 * @brief  Check ready state by RDI pin
 * @param  None
 * @return true - ready, false - not ready
 */
unsigned char AD779X_CheckReadyHW(tAD779X_Device *pDevice)
{
	unsigned char m_rdy_state;
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	if (pDevice->RDYState())
		m_rdy_state = 0;
	else
		m_rdy_state = 1;
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	return m_rdy_state;
}

/**
 * @brief  Set ADC mode
 * @param  Mode - need set
 * @return None
 */
void AD779X_SetMode(tAD779X_Device *pDevice, tAD779X_ModeSelect Mode)
{
	/* change and store needed mode */
	pDevice->ModeReg.MODE = Mode;      // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(pDevice, pDevice->ModeReg.DATA);
}

/**
 * @brief  Set ADC clock source
 * @param  ClkSource - need set
 * @return None
 */
void AD779X_SetClkSource(tAD779X_Device *pDevice, tAD779X_ClkSourceSelect ClkSource)
{
	/* change and store needed clock source */
	pDevice->ModeReg.CLKS = ClkSource; // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(pDevice, pDevice->ModeReg.DATA);
}

/**
 * @brief  Set ADC clock update rates
 * @param  ClkSource - need set
 * @return None
 */
void AD779X_SetUpdateRate(tAD779X_Device *pDevice, tAD779X_FilterSelect UpdateRates)
{
	/* change and store needed update rates */
	pDevice->ModeReg.FS = UpdateRates; // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(pDevice, pDevice->ModeReg.DATA);
}

/**
 * @brief  Set ADC excitation currents value
 * @param  excValue - need sat
 * @return None
 */
void AD779X_SetExCurrentValue(tAD779X_Device *pDevice, tAD779X_IEXCENSelect excValue)
{
	/* change and store needed value */
	pDevice->IOReg.IEXCEN = excValue;
	
	/* write value in IO register */
	AD779X_WriteIORegister(pDevice, pDevice->IOReg.DATA);
}

/**
 * @brief  Set ADC excitation currents direction
 * @param  excDirection - need sat
 * @return None
 */
void AD779X_SetExCurrentDirection(tAD779X_Device *pDevice, tAD779X_IEXCDIRSelect excDirection)
{
	/* change and store needed value */
	pDevice->IOReg.IEXCDIR = excDirection;
	
	/* write value in IO register */
	AD779X_WriteIORegister(pDevice, pDevice->IOReg.DATA);
}

/**
 * @brief  Start ADC Zero-Scale Calibration
 * @param  None
 * @return None
 */
void AD779X_StartZSCalibration(tAD779X_Device *pDevice)
{
	AD779X_SetMode(pDevice, mdsIntZeroCal);
}

/**
 * @brief  Start ADC Full-Scale Calibration
 * @param  None
 * @return None
 */
void AD779X_StartFSCalibration(tAD779X_Device *pDevice)
{
	AD779X_SetMode(pDevice, mdsIntFullCal);
}

/**
 * @brief  Read data from ADC (16-bit)
 * @param  None
 * @return Data register value
 */
unsigned short AD779X_ReadDataRegister16(tAD779X_Device *pDevice)
{
	unsigned char m_data[2];
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	pDevice->TxByte(AD779X_RDR_DATA);
	
	/* get value */
	m_data[1] = pDevice->RxByte();
	m_data[0] = pDevice->RxByte();
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	return (m_data[1]<<8)|m_data[0];
}

/**
 * @brief  Read data from ADC (24-bit)
 * @param  None
 * @return Data register value
 */
unsigned long AD779X_ReadDataRegister24(tAD779X_Device *pDevice)
{
	unsigned char m_data[3];
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	pDevice->TxByte(AD779X_RDR_DATA);
	
	/* get value */
	m_data[2] = pDevice->RxByte();
	m_data[1] = pDevice->RxByte();
	m_data[0] = pDevice->RxByte();
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	return (((unsigned long)((m_data[2]<<8)|m_data[1]))<<8)|m_data[0]; // NOTE: iar compiler hell
}

/**
 * @brief  Read data from ADC (all 16-bit)
 * @param  None
 * @return Data sample value
 */
unsigned short AD779X_ReadDataSample(tAD779X_Device *pDevice)
{
	unsigned short m_data_sample = 0;
	
	switch(pDevice->Model)
	{
		case ad7792:
			m_data_sample = AD779X_ReadDataRegister16(pDevice);
		break;
		
		case ad7793:
			m_data_sample = AD779X_ReadDataRegister24(pDevice)>>8;
		break;
	}
	
	return m_data_sample;
}

uint16_t AD779X_ReadDataSample16(tAD779X_Device *pDevice)
{
	uint8_t m_data[3];
	uint16_t m_data_sample = 0;
	
	/* active cs line */
	pDevice->CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	pDevice->TxByte(AD779X_RDR_DATA);
	
	if (pDevice->Model == ad7793)
		m_data[2] = pDevice->RxByte();
	
	/* get value */
	m_data[1] = pDevice->RxByte();
	m_data[0] = pDevice->RxByte();
	
	/* inactive cs line */
	pDevice->CSControl(cssDisable);
	
	switch (pDevice->Model)
	{
		case ad7792:
			m_data_sample = (m_data[1]<<8)|m_data[0];
			break;
			
		case ad7793:
			m_data_sample = (m_data[2]<<8)|m_data[1];
			break;
	}
	
	return m_data_sample;
}
