################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.c 

OBJS += \
./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.o 

C_DEPS += \
./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.d 


# Each subdirectory must supply rules for building sources it contributes
ThridParty/FreeRTOS/portable/GCC/ARM_CM3/%.o ThridParty/FreeRTOS/portable/GCC/ARM_CM3/%.su ThridParty/FreeRTOS/portable/GCC/ARM_CM3/%.cyclo: ../ThridParty/FreeRTOS/portable/GCC/ARM_CM3/%.c ThridParty/FreeRTOS/portable/GCC/ARM_CM3/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/Main ECU/Diagnostics/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/Tasks/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThridParty-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM3

clean-ThridParty-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM3:
	-$(RM) ./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.cyclo ./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.d ./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.o ./ThridParty/FreeRTOS/portable/GCC/ARM_CM3/port.su

.PHONY: clean-ThridParty-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM3

