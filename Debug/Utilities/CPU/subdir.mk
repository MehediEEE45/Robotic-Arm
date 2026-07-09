################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/CPU/cpu_utils.c 

OBJS += \
./Utilities/CPU/cpu_utils.o 

C_DEPS += \
./Utilities/CPU/cpu_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/CPU/%.o Utilities/CPU/%.su Utilities/CPU/%.cyclo: ../Utilities/CPU/%.c Utilities/CPU/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../FATFS/Target -I"D:/stm32/display/Drivers/BSP" -I"D:/stm32/display/Drivers/BSP/STM32746G-Discovery" -I"D:/stm32/display/Drivers/BSP/Components/Common" -I"D:/stm32/display/Drivers/BSP/Components/ft5336" -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities-2f-CPU

clean-Utilities-2f-CPU:
	-$(RM) ./Utilities/CPU/cpu_utils.cyclo ./Utilities/CPU/cpu_utils.d ./Utilities/CPU/cpu_utils.o ./Utilities/CPU/cpu_utils.su

.PHONY: clean-Utilities-2f-CPU

