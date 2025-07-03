################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Services/BluetoothTask.c \
../Services/CANReceiveTask.c \
../Services/CANTransmitTask.c \
../Services/MotorTask.c \
../Services/TemperatureTask.c \
../Services/UltrasonicTask.c 

OBJS += \
./Services/BluetoothTask.o \
./Services/CANReceiveTask.o \
./Services/CANTransmitTask.o \
./Services/MotorTask.o \
./Services/TemperatureTask.o \
./Services/UltrasonicTask.o 

C_DEPS += \
./Services/BluetoothTask.d \
./Services/CANReceiveTask.d \
./Services/CANTransmitTask.d \
./Services/MotorTask.d \
./Services/TemperatureTask.d \
./Services/UltrasonicTask.d 


# Each subdirectory must supply rules for building sources it contributes
Services/%.o Services/%.su Services/%.cyclo: ../Services/%.c Services/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Bluetooth/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/LM35/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Motor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Ultrasonic sensor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Services

clean-Services:
	-$(RM) ./Services/BluetoothTask.cyclo ./Services/BluetoothTask.d ./Services/BluetoothTask.o ./Services/BluetoothTask.su ./Services/CANReceiveTask.cyclo ./Services/CANReceiveTask.d ./Services/CANReceiveTask.o ./Services/CANReceiveTask.su ./Services/CANTransmitTask.cyclo ./Services/CANTransmitTask.d ./Services/CANTransmitTask.o ./Services/CANTransmitTask.su ./Services/MotorTask.cyclo ./Services/MotorTask.d ./Services/MotorTask.o ./Services/MotorTask.su ./Services/TemperatureTask.cyclo ./Services/TemperatureTask.d ./Services/TemperatureTask.o ./Services/TemperatureTask.su ./Services/UltrasonicTask.cyclo ./Services/UltrasonicTask.d ./Services/UltrasonicTask.o ./Services/UltrasonicTask.su

.PHONY: clean-Services

