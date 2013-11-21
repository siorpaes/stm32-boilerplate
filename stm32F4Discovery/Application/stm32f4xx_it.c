/**
  ******************************************************************************
  * @file    IO_Toggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

__asm void HardFault_Handler(void) 
	{ 
	IMPORT hard_fault_handler_c 
	TST LR, #4 
	ITE EQ 
	MRSEQ R0, MSP 
	MRSNE R0, PSP 
	B hard_fault_handler_c 
} 

/**
 * @brief  This function handles Hard Fault exception.
 *         Cfr.
 * @param  None
 * @retval None
 */
void hard_fault_handler_c(unsigned int * hardfault_args)
{
	
	//Big thanks to joseph.yiu for this handler. This is simply an adaptation of: 
//http://www.st.com/mcu/forums-cat-6778-23.html 
//**************************************************** 
//To test this application, you can use this snippet anywhere: 
// //Let's crash the MCU! 
// asm (" MOVS r0, #1 \n" 
// " LDM r0,{r1-r2} \n" 
// " BX LR; \n"); 
// Note that this works as-is in IAR 5.20, but could have a different syntax 
// in other compilers. 
//**************************************************** 
// hardfault_args is a pointer to your stack. you should change the adress 
// assigned if your compiler places your stack anywhere else! 

	unsigned int stacked_r0; 
	unsigned int stacked_r1; 
	unsigned int stacked_r2; 
	unsigned int stacked_r3; 
	unsigned int stacked_r12; 
	unsigned int stacked_lr; 
	unsigned int stacked_pc; 
	unsigned int stacked_psr; 

	stacked_r0 = ((unsigned long) hardfault_args[0]); 
	stacked_r1 = ((unsigned long) hardfault_args[1]); 
	stacked_r2 = ((unsigned long) hardfault_args[2]); 
	stacked_r3 = ((unsigned long) hardfault_args[3]); 

	stacked_r12 = ((unsigned long) hardfault_args[4]); 
	stacked_lr = ((unsigned long) hardfault_args[5]); 
	stacked_pc = ((unsigned long) hardfault_args[6]); 
	stacked_psr = ((unsigned long) hardfault_args[7]); 

	printf ("[Hard fault handler]\n"); 
	printf ("R0 = %x\n", stacked_r0); 
	printf ("R1 = %x\n", stacked_r1); 
	printf ("R2 = %x\n", stacked_r2); 
	printf ("R3 = %x\n", stacked_r3); 
	printf ("R12 = %x\n", stacked_r12); 
	printf ("LR = %x\n", stacked_lr); 
	printf ("PC = %x\n", stacked_pc); 
	printf ("PSR = %x\n", stacked_psr); 
	printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38)))); 
	printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28)))); 
	printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C)))); 
	printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30)))); 
	printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C)))); 

	while (1) {}
}


/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
