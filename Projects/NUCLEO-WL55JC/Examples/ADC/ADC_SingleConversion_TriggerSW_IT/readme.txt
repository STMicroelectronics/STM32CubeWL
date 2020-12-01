/**
  @page ADC_SingleConversion_TriggerSW_IT ADC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_SingleConversion_TriggerSW_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_SingleConversion_TriggerSW_IT example.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description
Use ADC to convert a single channel at each SW start, 
conversion performed using programming model: interrupt 

Example configuration:
ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.
ADC interruption enabled: EOC (end of conversion of ADC group regular).

Example execution:
The ADC performs 1 conversion of the selected channel. When conversion is completed,
ADC interruption occurs. IRQ handler callback function reads conversion data from
ADC data register and stores it into a variable, LED2 is turned on.

For debug: variables to monitor with debugger watch window:
 - "uhADCxConvertedData": ADC group regular conversion data
 - "uhADCxConvertedData_Voltage_mVolt": ADC conversion data computation to physical values

Connection needed:
None, if ADC channel and DAC channel are selected on the same GPIO.
Otherwise, connect a wire between DAC channel output and ADC input.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PA10 (Arduino connector CN8 pin 3, Morpho connector CN7 pin 32)
  DAC
  1 GPIO for DAC channel output PA10 (Arduino connector CN8 pin 3, Morpho connector CN7 pin 32)
  1 GPIO for push button

Board settings:
 - ADC is configured to convert ADC_CHANNEL_6 (Arduino connector CN8 pin 3, Morpho connector CN7 pin 32).
 - The voltage input on ADC channel is provided from DAC (DAC_CHANNEL_1).
   ADC input from pin PA10 and DAC output to pin PA10:
   If same pin is used no connection is required, it is done internally. Otherwise, user need to connect a wire between Arduino connector CN8 pin 3, Morpho connector CN7 pin 32 and Arduino connector CN8 pin 3, Morpho connector CN7 pin 32
 - Voltage is increasing at each click on User push-button (B1), from 0 to maximum range in 4 steps.
   Clicks on User push-button (B1) follow circular cycles: At clicks counter maximum value reached, counter is set back to 0.


To observe voltage level applied on ADC channel through GPIO, connect a voltmeter on
pin PA10 (Arduino connector CN8 pin 3, Morpho connector CN7 pin 32).

NUCLEO-WL55JC RevC board LED is be used to monitor the program execution status:
 - Normal operation: LED2 is turned-on/off in function of ADC conversion
   result.
    - "On" upon conversion completion
    - "Off" during conversion
 - Error: In case of error, LED2 is toggling twice at a frequency of 1Hz.

@par Keywords

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, trigger

@par Directory contents 

  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wlxx_hal_conf.h    HAL configuration file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/main.h                        Header for main.c module  
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/stm32wlxx_it.c          Interrupt handlers
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/main.c                        Main program
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/stm32wlxx_hal_msp.c     HAL MSP module
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/system_stm32wlxx.c      STM32WLxx system source file


@par Hardware and Software environment

  - This example runs on STM32WLxx devices.
    
  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
