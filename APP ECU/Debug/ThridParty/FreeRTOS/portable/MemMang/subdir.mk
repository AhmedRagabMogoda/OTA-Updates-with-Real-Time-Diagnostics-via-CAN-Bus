################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThridParty/FreeRTOS/portable/MemMang/heap_4.c 

OBJS += \
./ThridParty/FreeRTOS/portable/MemMang/heap_4.o 

C_DEPS += \
./ThridParty/FreeRTOS/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
ThridParty/FreeRTOS/portable/MemMang/%.o ThridParty/FreeRTOS/portable/MemMang/%.su ThridParty/FreeRTOS/portable/MemMang/%.cyclo: ../ThridParty/FreeRTOS/portable/MemMang/%.c ThridParty/FreeRTOS/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Bluetooth/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/LM35/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Motor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Ultrasonic sensor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/APP ECU/Tasks/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThridParty-2f-FreeRTOS-2f-portable-2f-MemMang

clean-ThridParty-2f-FreeRTOS-2f-portable-2f-MemMang:
	-$(RM) ./ThridParty/FreeRTOS/portable/MemMang/heap_4.cyclo ./ThridParty/FreeRTOS/portable/MemMang/heap_4.d ./ThridParty/FreeRTOS/portable/MemMang/heap_4.o ./ThridParty/FreeRTOS/portable/MemMang/heap_4.su

.PHONY: clean-ThridParty-2f-FreeRTOS-2f-portable-2f-MemMang

