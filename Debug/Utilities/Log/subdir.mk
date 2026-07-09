################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Log/lcd_log.c 

OBJS += \
./Utilities/Log/lcd_log.o 

C_DEPS += \
./Utilities/Log/lcd_log.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Log/%.o Utilities/Log/%.su Utilities/Log/%.cyclo: ../Utilities/Log/%.c Utilities/Log/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../FATFS/Target -I"D:/stm32/display/Drivers/BSP" -I"D:/stm32/display/Drivers/BSP/STM32746G-Discovery" -I"D:/stm32/display/Drivers/BSP/Components/Common" -I"D:/stm32/display/Drivers/BSP/Components/ft5336" -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities-2f-Log

clean-Utilities-2f-Log:
	-$(RM) ./Utilities/Log/lcd_log.cyclo ./Utilities/Log/lcd_log.d ./Utilities/Log/lcd_log.o ./Utilities/Log/lcd_log.su

.PHONY: clean-Utilities-2f-Log

