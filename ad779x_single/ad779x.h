/**
  ******************************************************************************
  * @file    ad779x.h
  * @author  Khusainov Timur
  * @version v0.0.1.3
  * @date    20.09.2011
  * @brief   Analog Devices Sigma-Delta ADC AD7792/AD7793 driver (for single device)
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#ifndef AD779X_H
#define AD779X_H

/**
 * @brief Register address description
 */
#define AD779X_REG_COMM   (0<<3)  /*!< 8-bit. Communications Register During a Write Operation (WO) */
#define AD779X_REG_STATUS (0<<3)  /*!< 8-bit. Status Register During a Read Operation (RO) */
#define AD779X_REG_ID     (4<<3)  /*!< 8-bit. ID Register (RO) */
#define AD779X_REG_IO     (5<<3)  /*!< 8-bit. IO Register (R/W) */
#define AD779X_REG_MODE   (1<<3)  /*!< 16-bit. Mode Register (R/w) */
#define AD779X_REG_CONFIG (2<<3)  /*!< 16-bit. Configuration Register (R/W) */
#define AD779X_REG_DATA   (3<<3)  /*!< 16/24-bit. Data Register (RO) */
#define AD779X_REG_OFFSET (6<<3)  /*!< 16/24-bit. Offset Register (R/W) */
#define AD779X_REG_FSCALE (7<<3)  /*!< 16/24-bit. Full-Scale Register (R/W) */

/**
 * @brief Register Power-On/Reset value
 */
#define AD779X_RDV_MODE   0x000A  /*!< Default value of MODE register */
#define AD779X_RDV_CONFIG 0x0710  /*!< Default value of CONFIG register */
#define AD779X_RDV_IO     0x0000  /*!< Default value of IO register */

/**
 * @brief Communications modes masks
 */
#define AD779X_COMM_WMODE 0x00    /*!< Communication is write mode */
#define AD779X_COMM_RMODE 0x40    /*!< Communication is read mode */
#define AD779X_COMM_CREED 0x04    /*!< Communication is continuous read */
#define AD779X_COMM_CMACK 0x7C    /*!< Communication correct operation mask */

/**
 * @brief Write operations with registers
 */
#define AD779X_WRR_COMM   ((AD779X_REG_COMM   | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)
#define AD779X_WRR_IO     ((AD779X_REG_IO     | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)
#define AD779X_WRR_MODE   ((AD779X_REG_MODE   | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)
#define AD779X_WRR_CONFIG ((AD779X_REG_CONFIG | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)
#define AD779X_WRR_OFFSET ((AD779X_REG_OFFSET | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)
#define AD779X_WRR_FSCLAE ((AD779X_REG_FSCALE | AD779X_COMM_WMODE) & AD779X_COMM_CMACK)

/**
 * @brief Read operations with registers
 */
#define AD779X_RDR_STATUS ((AD779X_REG_STATUS | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_ID     ((AD779X_REG_ID     | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_IO     ((AD779X_REG_IO     | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_MODE   ((AD779X_REG_MODE   | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_DATA   ((AD779X_REG_DATA   | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_CONFIG ((AD779X_REG_CONFIG | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_OFFSET ((AD779X_REG_OFFSET | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)
#define AD779X_RDR_FSCLAE ((AD779X_REG_FSCALE | AD779X_COMM_RMODE) & AD779X_COMM_CMACK)

/**
 * @brief ADC type's ID
 */
#define AD7792_PARTID 0x0A
#define AD7793_PARTID 0x0B

/**
 * @brief Status register bit mask
 */
#define AD779X_SR_RDY 0x80  /*!< Ready Bit for ADC. Cleared when data is written to the ADC data register */
#define AD779X_SR_ERR 0x40  /*!< ADC Error Bit. 1 - result ADC data register has been clamped: all 0s or all 1s */
#define AD779X_SR_CLR 0x30  /*!< These bits must be programmed with a Logic 0 for correct operation */
#define AD779X_SR_PID 0x08  /*!< 0 - AD7792, 1 - AD7793 */
#define AD779X_SR_CHC 0x07  /*!< These bits indicate which channel is being converted by the ADC */
/**
 * @brief Mode register mask
 */
#define AD779X_MODE_MD  0xE000  /*!< Mode Select */
#define AD779X_MODE_CLK 0x00C0  /*!< Ñlock source Select */
#define AD779X_MODE_FS  0x000F  /*!< Filter Update Rate Select */
#define AD779X_MODE_COM 0xE0CF  /*!< Communication correct operation mask */

/**
 * @brief Mode select value
 */
typedef enum
{
	mdsContinuous, /*!< Continuous Conversion Mode (Default) */
	mdsSingle,     /*!< Single Conversion Mode */
	mdsIdle,       /*!< Idle Mode */
	mdsPowerDown,  /*!< Power-Down Mode */
	mdsIntZeroCal, /*!< Internal Zero-Scale Calibration */
	mdsIntFullCal, /*!< Internal Full-Scale Calibration */
	mdsSysZeroCal, /*!< System Zero-Scale Calibration */
	mdsSysFullCal  /*!< System Full-Scale Calibration */
} tAD779X_ModeSelect;

/**
 * @brief Clock source select value
 */
typedef enum
{
	cssInt,    /*!< Internal 64 kHz Clock. Internal clock is not available at the CLK pin */
	cssIntOut, /*!< Internal 64 kHz Clock. This clock is made available at the CLK pin */
	cssExt,    /*!< External 64 kHz Clock Used. An external clock gives better 50 Hz/60 Hz rejection */
	cssExtDiv2 /*!< External Clock Used. The external clock is divided by 2 within the AD7792/AD7793 */
} tAD779X_ClkSourceSelect;

/**
 * @brief Filter update rate select value
 */
typedef enum
{
	fsNone,      /*!< f_adc = x, t_adc = x */
	fs500,       /*!< f_adc = 500 Hz, t_adc = 2 ms */
	fs250,       /*!< f_adc = 250 Hz, t_adc = 4 ms */
	fs152,       /*!< f_adc = 125 Hz, t_adc = 8 ms */
	fs62_5,      /*!< f_adc = 62.5 Hz, t_adc = 16 ms */
	fs50,        /*!< f_adc = 50.0 Hz, t_adc = 20 ms */
	fs39_2,      /*!< f_adc = 39.3 Hz, t_adc = 24 ms */
	fs33_3,      /*!< f_adc = 33.3 Hz, t_adc = 30 ms */
	fs19_6_90dB, /*!< f_adc = 19.6 Hz, t_adc = 50.5 ms, Rejection = 90 dB (60 Hz only) */
	fs16_7_80dB, /*!< f_adc = 16.7 Hz, t_adc = 60.0 ms, Rejection = 80 dB (50 Hz only) */
	fs16_7_65dB, /*!< f_adc = 16.7 Hz, t_adc = 60.0 ms, Rejection = 65 dB (50/60 Hz) */
	fs12_5_66dB, /*!< f_adc = 12.5 Hz, t_adc = 80.0 ms, Rejection = 66 dB (50/60 Hz) */
	fs10_69dB,   /*!< f_adc = 10.0 Hz, t_adc = 100 ms, Rejection = 69 dB (50/60 Hz) */
	fs8_33_70dB, /*!< f_adc = 8.33 Hz, t_adc = 120 ms, Rejection = 70 dB (50/60 Hz) */
	fs6_25_72dB, /*!< f_adc = 6.25 Hz, t_adc = 160 ms, Rejection = 72 dB (50/60 Hz) */
	fs4_17_74dB  /*!< f_adc = 4.17 Hz, t_adc = 240 ms, Rejection = 74 dB (50/60 Hz) */
} tAD779X_FilterSelect;

/**
 * @brief Mode register description
 */
typedef union
{
	struct
	{
		unsigned short FS      :4; /*!< Filter Update Rate Select */
		unsigned short UNUSED1 :2; /*!< These bits must be programmed with a Logic 0 for correct operation */
		unsigned short CLKS    :2; /*!< Clock source Select */
		unsigned short UNUSED0 :5; /*!< These bits must be programmed with a Logic 0 for correct operation */
		unsigned short MODE    :3; /*!< Mode Select */
	};
	unsigned short DATA; 
} tAD779X_ModeRegister;

/**
 * @brief Config register mask
 */
#define AD779X_CONFIG_VBIAS  0xC000  /*!< Bias Voltage Generator */
#define AD779X_CONFIG_BO     0x2000  /*!< Burnout Current */
#define AD779X_CONFIG_UB     0x1000  /*!< Unipolar/Bipolar */
#define AD779X_CONFIG_BOOST  0x0800  /*!< This bit is used in conjunction with the VBIAS1 and VBIAS0 bits */
#define AD779X_CONFIG_GAIN   0x0700  /*!< Gain Select */
#define AD779X_CONFIG_REFSEL 0x0080  /*!< Reference Select */
#define AD779X_CONFIG_BUF    0x0010  /*!< Configures the ADC for buffered or unbuffered mode of operation */
#define AD779X_CONFIG_CHSEL  0x0007  /*!< Channel Select */ 
#define AD779X_CONFIG_COM    0xFF97  /*!< Communication correct operation mask */

/**
 * @brief VBIAS select value
 */
typedef enum
{
	biasNone, /*!< Bias voltage generator disabled */
	biasAIN1, /*!< Bias voltage connected to AIN1(-) */
	biasAIN2  /*!< Bias voltage connected to AIN2(-) */
} tAD779X_VbiasSelect;

/**
 * @brief Burnout select value
 */
typedef enum
{
	boDisable, /*!< The burnout currents are disabled */
	boEnable   /*!< The burnout currents can be enabled only when the buffer or in-amp is active */
} tAD779X_BOSelect;

/**
 * @brief Unipolar/Bipolar select value
 */
typedef enum
{
	ubBipolar, /*!< Enable bipolar coding */
	ubUnipolar /*!< Enable unipolar coding */
} tAD779X_BSelect;

/**
 * @brief BOOST select value
 */
typedef enum
{
	boostDisable, /*!< None */
	boostEnable   /*!< Current consumed by the bias voltage generator is increased */
} tAD779X_BoostSelect;

/**
 * @brief Gain select value
 */
typedef enum
{
	gain1, /*!< In-amp not used */
	gain2, /*!< In-amp not used */
	gain4,
	gain8,
	gain16,
	gain32,
	gain64,
	gain128
} tAD779X_GainSelect;

/**
 * @brief Reference Select value
 */
typedef enum
{
	refExt, /*!< External Reference Applied between REFIN(+) and REFIN(–) */
	refInt  /*!< Internal Reference Selected */
} tAD779X_RefSelect;

/**
 * @brief Buf select value
 */
typedef enum
{
	bufDisable, /*!< ADC operates in unbuffered mode */
	bufEnable   /*!< ADC operates in buffered mode */
} tAD779X_BufSelect;

/**
 * @brief Channel select value
 */
typedef enum
{
	chsAIN1, /*!< AIN1(+) – AIN1(–), Calibration Pair = 0 */
	chsAIN2, /*!< AIN2(+) – AIN2(–), Calibration Pair = 1 */
	chsAIN3, /*!< AIN3(+) – AIN3(–), Calibration Pair = 2 */
	chsAIN1_AIN1,  /*!< AIN1(–) – AIN1(–), Calibration Pair = 0 */
	chsReserved0,  /*!< Reserved */
	chsReserved1,  /*!< Reserved */
	chsTempSensor, /*!< Automatically selects gain = 1 and internal reference */
	chsAVMonitor   /*!< Automatically selects gain = 1/6 and 1.17 V reference */
} tAD779X_ChSelect;

/**
 * @brief Config register description
 */
typedef union
{
	struct
	{
		unsigned short CHSEL   :3; /*!< Channel Select */
		unsigned short UNUSED0 :1; /*!< These bits must be programmed with a Logic 0 for correct operation */
		unsigned short BUF     :1; /*!< Configures the ADC for buffered or unbuffered mode of operation */
		unsigned short UNUSED1 :2; /*!< These bits must be programmed with a Logic 0 for correct operation */
		unsigned short REFSEL  :1; /*!< Reference Select */
		unsigned short GAIN    :3; /*!< Gain Select */
		unsigned short BOOST   :1; /*!< This bit is used in conjunction with the VBIAS1 and VBIAS0 bits */
		unsigned short UB      :1; /*!< Unipolar/Bipolar */
		unsigned short BO      :1; /*!< Burnout Current */
		unsigned short VBIAS   :2; /*!< Bias Voltage Generator */
    };
	unsigned short DATA;
} tAD779X_ConfigRegister;

#define AD779X_IO_IEXCDIR 0x0C  /*!< Current Source Direction */
#define AD779X_IO_IEXCEN  0x03  /*!< Current Source Value */
#define AD779X_IO_COM     0x0F  /*!< Communication correct operation mask */

/**
 * @brief Current Source Direction (IEXCDIR) select value
 */
typedef enum
{
	csdNormal,  /*!< IEXC1 -> IOUT1, IEXC2 -> IOUT2 */
	csdInverse, /*!< IEXC1 -> IOUT2, IEXC2 -> IOUT1 */
	csdBcsOut1, /*!< Both current sources -> IOUT1. Current sources: 10uA or 210uA only */
	csdBcsOut2  /*!< Both current sources -> IOUT2. Current sources: 10uA or 210uA only */
} tAD779X_IEXCDIRSelect;

/**
 * @brief Current Source Value (IEXCEN) select value
 */
typedef enum
{
	csvDisable, /*!< Excitation Current Disabled */
	csv10uA,    /*!< 10 uA */
	csv210uA,   /*!< 210 uA */
	csv1mA      /*!< 1 mA */
} tAD779X_IEXCENSelect;

/**
 * @brief IO register description
 */
typedef union
{
	struct
	{
		unsigned char IEXCEN  :2; /*!< These bits are used to enable and disable the current sources */
		unsigned char IEXCDIR :2; /*!< Direction of current sources */
		unsigned char UNUSED  :4; /*!< These bits must be programmed with a Logic 0 for correct operation */
	};
	unsigned char DATA;
} tAD779X_IORegister;

/**
 * @brief Data as: int, short and byte(4)
 */
typedef union
{
	unsigned long  u32;   /*!< 32 bit */
	unsigned short u16;   /*!< 16 bit */
	unsigned char  u8[4]; /*!< 4x8 bit */
} tAD779X_DataSample;

/**
 * @brief Power-On/Reset offset register value
 */
#define AD779X_OFFSET_RESET_24 (0x800000)
#define AD779X_OFFSET_RESET_16 (0x8000)

/**
 * @brief Power-On/Reset full-scale register value
 */
#define AD779X_FULLSCALE_RESET_24 (0x500000)
#define AD779X_FULLSCALE_RESET_16 (0x5000)

/**
 * @brief Model of AD779X
 */
typedef enum
{
	adNone,
	ad7792,
	ad7793
} tAD779X_Model;

/**
 * @brief AD779X startup status
 */
typedef enum
{
	susNone,     /*!< default state - non start */
	susNoHW,     /*!< ADC type none detect */
	susActivate  /*!< ADC start-up normal */
} tAD779X_StartUpState;

typedef enum
{
	pwrDisable,
	pwrEnable
} tAD779X_PWRState;

typedef enum
{
	cssDisable,
	cssEnable
} tAD779X_CSState;

typedef enum
{
	rdsFree,
	rdsBusy
} tAD779X_RDState;

typedef void (* tAD779X_PWRControl)(unsigned char State);
typedef void (* tAD779X_TxByte)(unsigned char Data);
typedef unsigned char (* tAD779X_RxByte)(void);
typedef void (* tAD779X_CSControl)(unsigned char State);
typedef unsigned char (* tAD779X_RDYState)(void);

typedef struct
{
	tAD779X_Model Model;
	tAD779X_StartUpState SuState;
	tAD779X_ModeRegister ModeReg;
	tAD779X_IORegister IOReg;
	tAD779X_DataSample OfReg;
	tAD779X_DataSample FsReg;
	tAD779X_PWRControl PWRControl;
	tAD779X_CSControl CSControl;
	tAD779X_RDYState RDYState;
	tAD779X_TxByte TxByte;
	tAD779X_RxByte RxByte;
} tAD779X_Device;

extern tAD779X_Device ADCDevice;

void AD779X_Init();
void AD779X_Reset();
void AD779X_WriteModeRegister(unsigned short Data);
void AD779X_WriteConfigRegister(unsigned short Data);
void AD779X_WriteIORegister(unsigned char Data);
unsigned short AD779X_HWDetect();
unsigned char AD779X_GetStatus();
unsigned char AD779X_CheckReadySW();
unsigned char AD779X_CheckReadyHW();
void AD779X_SetMode(tAD779X_ModeSelect Mode);
void AD779X_SetClkSource(tAD779X_ClkSourceSelect ClkSource);
void AD779X_SetUpdateRate(tAD779X_FilterSelect UpdateRates);
void AD779X_SetExCurrentValue(tAD779X_IEXCENSelect excValue);
void AD779X_SetExCurrentDirection(tAD779X_IEXCDIRSelect excDirection);
void AD779X_SetExCurrent(tAD779X_IEXCENSelect excValue, tAD779X_IEXCDIRSelect excDirection);
void AD779X_StartZSCalibration();
void AD779X_StartFSCalibration();
unsigned short AD779X_ReadOffsetRegister16();
unsigned long  AD779X_ReadOffsetRegister24();
unsigned short AD779X_ReadFScaleRegister16();
unsigned long  AD779X_ReadFScaleRegister24();
unsigned short AD779X_ReadDataRegister16();
unsigned long  AD779X_ReadDataRegister24();
unsigned short AD779X_ReadDataSample16();
unsigned long  AD779X_ReadDataSample24();

#endif
