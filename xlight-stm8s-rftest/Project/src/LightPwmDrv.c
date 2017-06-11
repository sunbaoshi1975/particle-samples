/*;*******************************************************************************
; FILENAME		: LightPwmDrv.c
; PURPOSE		: PWM-based Lighting Driver
; REVISION		: 1.0
; SYSTEM CLOCK	        : 16MHz
; MCU type              : STM8S105K4
;*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "LightPwmDrv.h"
#include "_global.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WATT_PERCENTAGE                 ((uint16_t)100)                  // 50 to 100

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if defined(XSUNNY)
// T2-C1 Cold White
#define COLD_LIGHT_PWM_PIN_PORT         GPIOD
#define COLD_LIGHT_PWM_PIN_ID           GPIO_PIN_4

// T2-C2 Warm White
#define WARM_LIGHT_PWM_PIN_PORT         GPIOD
#define WARM_LIGHT_PWM_PIN_ID           GPIO_PIN_3

#define TIM2_PWM_PERIOD         199
#define TIM2_PWM_PULSE          200
#endif

#if defined(XRAINBOW) || defined(XMIRAGE)
// T2-C1 White
#define W_LIGHT_PWM_PIN_PORT         GPIOD
#define W_LIGHT_PWM_PIN_ID           GPIO_PIN_4

// T2-C2 Red
#define R_LIGHT_PWM_PIN_PORT         GPIOD
#define R_LIGHT_PWM_PIN_ID           GPIO_PIN_3

// T3-C1 Green
#define G_LIGHT_PWM_PIN_PORT         GPIOD
#define G_LIGHT_PWM_PIN_ID           GPIO_PIN_2

// T3-C2 Blue
#define B_LIGHT_PWM_PIN_PORT         GPIOD
#define B_LIGHT_PWM_PIN_ID           GPIO_PIN_0

#define TIM2_PWM_PERIOD         254
#define TIM2_PWM_PULSE          200
#endif

/**
  * @brief  Configure peripherals GPIO for PWM   
  * @param  None
  * @retval None
  */
static void TIM2LightPwm_ClkGpioConfig(void)
{
#if defined(XSUNNY)
  /* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);

  /* TIM2 Channel1 configuration: PB2 */
  GPIO_Init(COLD_LIGHT_PWM_PIN_PORT, COLD_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(WARM_LIGHT_PWM_PIN_PORT, WARM_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
#endif
  
#if defined(XRAINBOW) || defined(XMIRAGE)
  /* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);

  /* Enable TIM3 clock */
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, ENABLE);

  /* TIM2 Channel1, 2 & TIM3 Channel1, 2 configuration */
  GPIO_Init(W_LIGHT_PWM_PIN_PORT, W_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(R_LIGHT_PWM_PIN_PORT, R_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(G_LIGHT_PWM_PIN_PORT, G_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(B_LIGHT_PWM_PIN_PORT, B_LIGHT_PWM_PIN_ID, GPIO_MODE_OUT_PP_LOW_FAST);
#endif
}

/**
  * @brief  Configure TIM2 peripheral   
  * @param  None
  * @retval None
  */
static void TIM2PWMFunction_Config(void)
{
#if defined(XSUNNY)
   TIM2_DeInit();
  /* TIM2 configuration:
     - TIM2 counter is clocked by HSI div 8 2M
      so the TIM2 counter clock used is HSI / 1 = 16M / 1 = 16 MHz
    TIM2 Channel1 output frequency = TIM2CLK / (TIM2 Prescaler * (TIM2_PERIOD + 1))
    = 16M / (8 * 200) = 10K Hz //TIM2_Prescaler_8; TIM2_PWM_PERIOD = 199
  */
  /* Time Base configuration */
  TIM2_TimeBaseInit(TIM2_PRESCALER_8, TIM2_PWM_PERIOD);
  //TIM2_ETRClockMode2Config(TIM2_ExtTRGPSC_DIV4, TIM2_ExtTRGPolarity_NonInverted, 0);

  /* Channel 1 configuration in PWM1 mode */
  /* TIM2 channel Duty cycle is 100 * (TIM2_PERIOD + 1 - TIM2_PULSE) / (TIM2_PERIOD + 1) = 100 * 4/8 = 50 % */
  TIM2_OC1Init(TIM2_OCMODE_PWM2, TIM2_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM2_OCPOLARITY_HIGH);
  TIM2_OC1PreloadConfig(ENABLE);

  /* Channel 2 configuration in PWM1 mode */
  /* TIM2 channel Duty cycle is 100 * (TIM2_PERIOD + 1 - TIM2_PULSE) / (TIM2_PWM_PERIOD + 1) = 100 * 4/8 = 50 % */
  TIM2_OC2Init(TIM2_OCMODE_PWM2, TIM2_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM2_OCPOLARITY_HIGH);
  TIM2_OC2PreloadConfig(ENABLE);

  /* TIM2 Main Output Enable */
  TIM2_ARRPreloadConfig(ENABLE);

  /* TIM2 counter enable */
  TIM2_Cmd(ENABLE);
#endif

#if defined(XRAINBOW) || defined(XMIRAGE)
  TIM2_DeInit();
  TIM3_DeInit();
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_8, TIM2_PWM_PERIOD);
  TIM3_TimeBaseInit(TIM3_PRESCALER_8, TIM2_PWM_PERIOD);

  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM2_OCPOLARITY_HIGH);
  TIM2_OC1PreloadConfig(ENABLE);

  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM2_OCPOLARITY_HIGH);
  TIM2_OC2PreloadConfig(ENABLE);

  TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM3_OCPOLARITY_HIGH);
  TIM3_OC1PreloadConfig(ENABLE);

  TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, TIM2_PWM_PULSE, TIM3_OCPOLARITY_HIGH);
  TIM3_OC2PreloadConfig(ENABLE);
  
  /* TIM2 Main Output Enable */
  TIM2_ARRPreloadConfig(ENABLE);

  /* TIM3 Main Output Enable */
  TIM3_ARRPreloadConfig(ENABLE);
  
  // TIM2 counter enable
  TIM2_Cmd(ENABLE);

  // TIM3 counter enable
  TIM3_Cmd(ENABLE);
  
#endif
}

void initTim2PWMFunction (void)
{
  TIM2LightPwm_ClkGpioConfig ();
  TIM2PWMFunction_Config ();
}

#if defined(XSUNNY)  
void regulateColdLightPulseWidth (unsigned char ucPercent)
{
  // uint16_t pulseWidth;
  if (ucPercent > 100)
    ucPercent = 100;
  //pulseWidth = 100 * (TIM2_PWM_PERIOD + 1) / ucPercent;
  ucPercent = ucPercent * WATT_PERCENTAGE / 100;
    
  //TIM2_CtrlPWMOutputs(DISABLE);
  //TIM2_Cmd(DISABLE);
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, ucPercent * 2, TIM2_OCPOLARITY_HIGH);
  //TIM2_OC1PreloadConfig(ENABLE);
  //TIM2_CtrlPWMOutputs(ENABLE);
  //TIM2_Cmd(ENABLE);
}

void regulateWarmLightPulseWidth (unsigned char ucPercent)
{
  //uint16_t pulseWidth;
  if (ucPercent > 100)
    ucPercent = 100;
  ucPercent = ucPercent * WATT_PERCENTAGE / 100;
  
  //pulseWidth = ucPercent * 2;
  //pulseWidth = 100 * (TIM2_PWM_PERIOD + 1) / pulseWidth;
  
  
  //TIM2_CtrlPWMOutputs(DISABLE);
  //TIM2_Cmd(DISABLE);
  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, ucPercent * 2, TIM2_OCPOLARITY_HIGH);  
  //TIM2_OC2PreloadConfig(ENABLE);
  //TIM2_CtrlPWMOutputs(ENABLE);
  //TIM2_Cmd(ENABLE);
}
#endif

void driveColdWarmLightPwm (unsigned char ucCold, unsigned char ucWarm)
{
#if defined(XSUNNY)
  regulateColdLightPulseWidth (ucCold );
  regulateWarmLightPulseWidth (ucWarm );
#endif  
}

#if defined(XRAINBOW) || defined(XMIRAGE)
void RGBWCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue, uint8_t WValue)
{
  TIM2_SetCompare1(WValue);
  TIM2_SetCompare2(RValue);
  TIM3_SetCompare1(GValue);
  TIM3_SetCompare2(BValue);
  //TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, WValue, TIM2_OCPOLARITY_HIGH);
  //TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, RValue, TIM2_OCPOLARITY_HIGH);
  //TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, GValue, TIM3_OCPOLARITY_HIGH);
  //TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, BValue, TIM3_OCPOLARITY_HIGH);
}

void RGBCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue)
{
  TIM2_SetCompare2(RValue);
  TIM3_SetCompare1(GValue);
  TIM3_SetCompare2(BValue);
  //TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, RValue, TIM2_OCPOLARITY_HIGH);
  //TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, GValue, TIM3_OCPOLARITY_HIGH);
  //TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, BValue, TIM3_OCPOLARITY_HIGH);
}
#endif  

void LightRGBWBRCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue, uint8_t WValue, uint8_t BRPercent)
{
  if(BRPercent > 100)
    BRPercent = 100;

  /*
  RValue = RValue * 80 / 255;
  GValue = GValue * 80 / 255;
  BValue = BValue * 80 / 255;
  WValue = WValue * 80 / 255;
  */
  
  RValue = RValue * (WATT_PERCENTAGE - 1) / 100 * BRPercent / 100;
  GValue = GValue * (WATT_PERCENTAGE - 15) / 100 * BRPercent / 100;
  BValue = BValue * (WATT_PERCENTAGE - 15) / 100 * BRPercent / 100;
  WValue = WValue * (WATT_PERCENTAGE - 15) / 100 * BRPercent / 100;
  
#if defined(XRAINBOW) || defined(XMIRAGE)
  RGBWCtrl(RValue, GValue, BValue, WValue);
#endif
}

void LightRGBBRCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue, uint8_t BRPercent)
{
  if(BRPercent > 100)
    BRPercent = 100;
  
  /*
  RValue = RValue * 80 / 255;
  GValue = GValue * 80 / 255;
  BValue = BValue * 80 / 255;
  */
  
  RValue = RValue * WATT_PERCENTAGE / 100 * BRPercent / 100;
  GValue = GValue * WATT_PERCENTAGE / 100 * BRPercent / 100;
  BValue = BValue * WATT_PERCENTAGE / 100 * BRPercent / 100;
  
#if defined(XRAINBOW) || defined(XMIRAGE)
  RGBCtrl(RValue, GValue, BValue);
#endif
}

#if defined(XRAINBOW) || defined(XMIRAGE)
typedef struct {
  uint16_t cct;  
  uint8_t w;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} CCTTable_t;

#define CCT_TABLE_ROWS          19
const CCTTable_t cctTable[] = {
  // CCT        W       R       G       B
  {2700,        21,     254,    51,     0},
  {2800,        26,     254,    52,     0},
  {3000,        39,     254,    54,     0},
  {3200,        59,     254,    56,     0},
  {3400,        93,     254,    58,     0},
  {3600,        149,    234,    55,     0},
  {3800,        197,    136,    33,     0},
  {4000,        254,    19,     7,      2},
  {4200,        251,    0,      22,     13},
  {4400,        239,    0,      41,     24},
  {4600,        228,    0,      57,     35},
  {4800,        218, 	0, 	71, 	45}, 
  {5000,        210, 	0, 	84, 	54}, 
  {5200,        202, 	0, 	95, 	63},
  {5400,        195, 	0, 	105, 	71},
  {5600,        188, 	0, 	113, 	78},
  {5800,        182, 	0, 	121, 	85},
  {6000,        177, 	0, 	128, 	91},
  {6500,        166, 	0, 	142, 	105}
};

void ConvertCCT2RGBW(uint16_t CCTValue, uint8_t *RValue, uint8_t *GValue, uint8_t *BValue, uint8_t *WValue)
{
  for( uint8_t i = 0; i < CCT_TABLE_ROWS; i++ ) {
    if( CCTValue <= cctTable[i].cct ) {
      *WValue = cctTable[i].w;
      *RValue = cctTable[i].r;
      *GValue = cctTable[i].g;
      *BValue = cctTable[i].b;
      return;
    }
  }
  *WValue = cctTable[CCT_TABLE_ROWS - 1].w;
  *RValue = cctTable[CCT_TABLE_ROWS - 1].r;
  *GValue = cctTable[CCT_TABLE_ROWS - 1].g;
  *BValue = cctTable[CCT_TABLE_ROWS - 1].b;  
}
#endif

/******************* (C) COPYRIGHT 2016 Eastfield Lighting *****END OF FILE****/