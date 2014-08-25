/**
  ******************************************************************************
  * @file    ad779x.c
  * @author  Khusainov Timur
  * @version v0.0.1.3
  * @date    20.09.2011
  * @brief   Analog Devices Sigma-Delta ADC AD779x driver (for single device)
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
void AD779X_Init()
{
	/* Detect type of ADC */
	if (AD779X_HWDetect())
	{
		/* set default settings: continuous conversion, internal clock source, 16.7Hz (65dB) */
		AD779X_WriteModeRegister(gModeReg.DATA);
		
		/* set default settings: excitation currents disabled */
		AD779X_WriteIORegister(gIOReg.DATA);
		
		switch (ADCDevice.Model)
		{
			case ad7792:
			{
				ADCDevice.OfReg.u16 = AD779X_OFFSET_RESET_16;
				ADCDevice.FsReg.u16 = AD779X_FULLSCALE_RESET_16;
			}
			break;
			
			case ad7793:
			{
				ADCDevice.OfReg.u32 = AD779X_OFFSET_RESET_24;
				ADCDevice.FsReg.u32 = AD779X_FULLSCALE_RESET_24;
			}
			break;
		}
		
		/* store startup state */
		ADCDevice.SuState = susActivate;
	}
	else
	{
		/* store startup state */
		ADCDevice.SuState = susNoHW;
	}
}

/**
 * @brief  Reset ADC
 * @param  None
 * @return None
 */
void AD779X_Reset()
{
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* make 32 clk, while dout -> 1 */
	ADCDevice.TxByte(0xFF);
	ADCDevice.TxByte(0xFF);
	ADCDevice.TxByte(0xFF);
	ADCDevice.TxByte(0xFF);
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
}

/**
 * @brief  Write value in MODE register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteModeRegister(unsigned short Data)
{
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: write MODE register */
	ADCDevice.TxByte(AD779X_WRR_MODE);
	
	/* write data to register */
	ADCDevice.TxByte(Data >> 8);
	ADCDevice.TxByte(Data & 0x00FF);
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
}

/**
 * @brief  Write value in CONFIG register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteConfigRegister(unsigned short Data)
{
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: write CONFIG register */
	ADCDevice.TxByte(AD779X_WRR_CONFIG);
	
	/* write data to register */
	ADCDevice.TxByte(Data >> 8);
	ADCDevice.TxByte(Data & 0x00FF);
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
}

/**
 * @brief  Write value in IO register
 * @param  Data - need write
 * @return None
 */
void AD779X_WriteIORegister(unsigned char Data)
{
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: write IO register */
	ADCDevice.TxByte(AD779X_WRR_IO);
	
	/* write data to register */
	ADCDevice.TxByte(Data);
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
}

/**
 * @brief  Detect ADC type
 * @param  None
 * @return Model as status of detecting
 */
unsigned short AD779X_HWDetect()
{
	unsigned short m_adc_model;
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read ID register */
	ADCDevice.TxByte(AD779X_RDR_ID);
	
	/* get ID */
	m_adc_model = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	switch (m_adc_model & 0xF)
	{
		case AD7792_PARTID: ADCDevice.Model = ad7792; break;
		case AD7793_PARTID: ADCDevice.Model = ad7793; break;
		
		default: ADCDevice.Model = adNone; break;
	}
	
	return ADCDevice.Model;
}

/**
 * @brief  Get ADC status
 * @param  None
 * @return Value as status
 */
unsigned char AD779X_GetStatus()
{
	unsigned char m_adc_status;
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read STATUS register */
	ADCDevice.TxByte(AD779X_RDR_STATUS);
	
	/* get value */
	m_adc_status = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return m_adc_status;
}

/**
 * @brief  Check ready status by STATUS_REG
 * @param  None
 * @return true - ready, false - not ready
 */
unsigned char AD779X_CheckReadySW()
{
	return (~AD779X_GetStatus() & AD779X_SR_RDY);
}

/**
 * @brief  Check ready state by RDI pin
 * @param  None
 * @return true - ready, false - not ready
 */
unsigned char AD779X_CheckReadyHW()
{
	unsigned char m_rdy_state;
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	if (ADCDevice.RDYState())
		m_rdy_state = 0;
	else
		m_rdy_state = 1;
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return m_rdy_state;
}

/**
 * @brief  Set ADC mode
 * @param  Mode - need set
 * @return None
 */
void AD779X_SetMode(tAD779X_ModeSelect Mode)
{
	/* change and store needed mode */
	ADCDevice.ModeReg.MODE = Mode;      // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(ADCDevice.ModeReg.DATA);
}

/**
 * @brief  Set ADC clock source
 * @param  ClkSource - need set
 * @return None
 */
void AD779X_SetClkSource(tAD779X_ClkSourceSelect ClkSource)
{
	/* change and store needed clock source */
	ADCDevice.ModeReg.CLKS = ClkSource; // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(ADCDevice.ModeReg.DATA);
}

/**
 * @brief  Set ADC clock update rates
 * @param  ClkSource - need set
 * @return None
 */
void AD779X_SetUpdateRate(tAD779X_FilterSelect UpdateRates)
{
	/* change and store needed update rates */
	ADCDevice.ModeReg.FS = UpdateRates; // FIXME: need assembler optimisation
	
	/* write value in MODE register */
	AD779X_WriteModeRegister(ADCDevice.ModeReg.DATA);
}

/**
 * @brief  Set ADC excitation currents value
 * @param  excValue - need sat
 * @return None
 */
void AD779X_SetExCurrentValue(tAD779X_IEXCENSelect excValue)
{
	/* change and store needed value */
	ADCDevice.IOReg.IEXCEN = excValue;
	
	/* write value in IO register */
	AD779X_WriteIORegister(ADCDevice.IOReg.DATA);
}

/**
 * @brief  Set ADC excitation currents direction
 * @param  excDirection - need sat
 * @return None
 */
void AD779X_SetExCurrentDirection(tAD779X_IEXCDIRSelect excDirection)
{
	/* change and store needed value */
	ADCDevice.IOReg.IEXCDIR = excDirection;
	
	/* write value in IO register */
	AD779X_WriteIORegister(ADCDevice.IOReg.DATA);
}

/**
 * @brief  Set ADC excitation current
 * @param  excValue - need sat
 * @return None
 */
void AD779X_SetExCurrent(tAD779X_IEXCENSelect excValue, tAD779X_IEXCDIRSelect excDirection)
{
	/* change and store needed value */
	ADCDevice.IOReg.IEXCEN = excValue;
	ADCDevice.IOReg.IEXCDIR = excDirection;
	
	/* write value in IO register */
	AD779X_WriteIORegister(ADCDevice.IOReg.DATA);
}

/**
 * @brief  Start ADC Zero-Scale Calibration
 * @param  None
 * @return None
 */
void AD779X_StartZSCalibration()
{
	AD779X_SetMode(mdsIntZeroCal);
}

/**
 * @brief  Start ADC Full-Scale Calibration
 * @param  None
 * @return None
 */
void AD779X_StartFSCalibration()
{
	AD779X_SetMode(mdsIntFullCal);
}

/**
 * @brief  Read offset register value (16-bit)
 * @param  None
 * @return offset register value
 */
unsigned short AD779X_ReadOffsetRegister16()
{
	unsigned char m_data[2];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_OFFSET);
	
	/* get value */
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (m_data[1]<<8)|m_data[0];
}

/**
 * @brief  Read offset register value (24-bit)
 * @param  None
 * @return offset register value
 */
unsigned long AD779X_ReadOffsetRegister24()
{
	unsigned char m_data[3];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_OFFSET);
	
	/* get value */
	m_data[2] = ADCDevice.RxByte();
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (((unsigned long)((m_data[2]<<8)|m_data[1]))<<8)|m_data[0]; // NOTE: iar compiler hell
}

/**
 * @brief  Read offset register value (16-bit)
 * @param  None
 * @return offset register value
 */
unsigned short AD779X_ReadFScaleRegister16()
{
	unsigned char m_data[2];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_FSCLAE);
	
	/* get value */
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (m_data[1]<<8)|m_data[0];
}

/**
 * @brief  Read offset register value (24-bit)
 * @param  None
 * @return offset register value
 */
unsigned long AD779X_ReadFScaleRegister24()
{
	unsigned char m_data[3];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_FSCLAE);
	
	/* get value */
	m_data[2] = ADCDevice.RxByte();
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (((unsigned long)((m_data[2]<<8)|m_data[1]))<<8)|m_data[0]; // NOTE: iar compiler hell
}

/**
 * @brief  Read data from ADC (16-bit)
 * @param  None
 * @return Data register value
 */
unsigned short AD779X_ReadDataRegister16()
{
	unsigned char m_data[2];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_DATA);
	
	/* get value */
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (m_data[1]<<8)|m_data[0];
}

/**
 * @brief  Read data from ADC (24-bit)
 * @param  None
 * @return Data register value
 */
unsigned long AD779X_ReadDataRegister24()
{
	unsigned char m_data[3];
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_DATA);
	
	/* get value */
	m_data[2] = ADCDevice.RxByte();
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	return (((unsigned long)((m_data[2]<<8)|m_data[1]))<<8)|m_data[0]; // NOTE: iar compiler hell
}

/**
 * @brief  Read data from ADC (16-bit range)
 * @param  None
 * @return Data sample value
 */
unsigned short AD779X_ReadDataSample16()
{
	unsigned char m_data[3];
	unsigned short m_data_sample = 0;
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_DATA);
	
	if (ADCDevice.Model == ad7793)
		m_data[2] = ADCDevice.RxByte();
	
	/* get value */
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	switch (ADCDevice.Model)
	{
		case ad7792: m_data_sample = ((m_data[1] << 8)| m_data[0]); break;
		case ad7793: m_data_sample = ((m_data[2] << 8)| m_data[1]); break;
	}
	
	return m_data_sample;
}

/**
 * @brief  Read data from ADC (24-bit range)
 * @param  None
 * @return Data sample value
 */
unsigned long AD779X_ReadDataSample24()
{
	unsigned char m_data[3];
	unsigned long m_data_sample = 0;
	
	/* active cs line */
	ADCDevice.CSControl(cssEnable);
	
	/* send cmd: read DATA register */
	ADCDevice.TxByte(AD779X_RDR_DATA);
	
	if (ADCDevice.Model == ad7793)
		m_data[2] = ADCDevice.RxByte();
	
	/* get value */
	m_data[1] = ADCDevice.RxByte();
	m_data[0] = ADCDevice.RxByte();
	
	/* inactive cs line */
	ADCDevice.CSControl(cssDisable);
	
	switch (ADCDevice.Model)
	{
		case ad7792:
			m_data_sample = (((unsigned long)((m_data[1] << 8)| m_data[0])) << 8);
			break;
		
		case ad7793:
			m_data_sample = ((((unsigned long)((m_data[2] << 8)| m_data[1])) << 8) | m_data[0]);
			break;
	}
	
	return m_data_sample;
}