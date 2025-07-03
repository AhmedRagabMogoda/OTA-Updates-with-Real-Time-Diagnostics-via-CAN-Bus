################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.c 

OBJS += \
./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.o 

C_DEPS += \
./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.o: ../HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.c HAL/Ultrasonic\ sensor/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Bluetooth/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/LM35/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Motor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Ultrasonic sensor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/APP ECU/Tasks/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"HAL/Ultrasonic sensor/Src/Ultrasonic_program.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-HAL-2f-Ultrasonic-20-sensor-2f-Src

clean-HAL-2f-Ultrasonic-20-sensor-2f-Src:
	-$(RM) ./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.cyclo ./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.d ./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.o ./HAL/Ultrasonic\ sensor/Src/Ultrasonic_program.su

.PHONY: clean-HAL-2f-Ultrasonic-20-sensor-2f-Src

