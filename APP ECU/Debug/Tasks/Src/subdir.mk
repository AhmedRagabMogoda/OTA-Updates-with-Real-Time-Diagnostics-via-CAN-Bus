################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/Src/BluetoothTask.c \
../Tasks/Src/CANReceiveTask.c \
../Tasks/Src/CANTransmitTask.c \
../Tasks/Src/MotorTask.c \
../Tasks/Src/TaskHandle.c \
../Tasks/Src/TemperatureTask.c \
../Tasks/Src/UARTReceiveTask.c \
../Tasks/Src/UARTTransmitTask.c \
../Tasks/Src/UltrasonicTask.c 

OBJS += \
./Tasks/Src/BluetoothTask.o \
./Tasks/Src/CANReceiveTask.o \
./Tasks/Src/CANTransmitTask.o \
./Tasks/Src/MotorTask.o \
./Tasks/Src/TaskHandle.o \
./Tasks/Src/TemperatureTask.o \
./Tasks/Src/UARTReceiveTask.o \
./Tasks/Src/UARTTransmitTask.o \
./Tasks/Src/UltrasonicTask.o 

C_DEPS += \
./Tasks/Src/BluetoothTask.d \
./Tasks/Src/CANReceiveTask.d \
./Tasks/Src/CANTransmitTask.d \
./Tasks/Src/MotorTask.d \
./Tasks/Src/TaskHandle.d \
./Tasks/Src/TemperatureTask.d \
./Tasks/Src/UARTReceiveTask.d \
./Tasks/Src/UARTTransmitTask.d \
./Tasks/Src/UltrasonicTask.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/Src/%.o Tasks/Src/%.su Tasks/Src/%.cyclo: ../Tasks/Src/%.c Tasks/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Bluetooth/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/LM35/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Motor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/HAL/Ultrasonic sensor/Inc" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/APP ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/APP ECU/Tasks/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Tasks-2f-Src

clean-Tasks-2f-Src:
	-$(RM) ./Tasks/Src/BluetoothTask.cyclo ./Tasks/Src/BluetoothTask.d ./Tasks/Src/BluetoothTask.o ./Tasks/Src/BluetoothTask.su ./Tasks/Src/CANReceiveTask.cyclo ./Tasks/Src/CANReceiveTask.d ./Tasks/Src/CANReceiveTask.o ./Tasks/Src/CANReceiveTask.su ./Tasks/Src/CANTransmitTask.cyclo ./Tasks/Src/CANTransmitTask.d ./Tasks/Src/CANTransmitTask.o ./Tasks/Src/CANTransmitTask.su ./Tasks/Src/MotorTask.cyclo ./Tasks/Src/MotorTask.d ./Tasks/Src/MotorTask.o ./Tasks/Src/MotorTask.su ./Tasks/Src/TaskHandle.cyclo ./Tasks/Src/TaskHandle.d ./Tasks/Src/TaskHandle.o ./Tasks/Src/TaskHandle.su ./Tasks/Src/TemperatureTask.cyclo ./Tasks/Src/TemperatureTask.d ./Tasks/Src/TemperatureTask.o ./Tasks/Src/TemperatureTask.su ./Tasks/Src/UARTReceiveTask.cyclo ./Tasks/Src/UARTReceiveTask.d ./Tasks/Src/UARTReceiveTask.o ./Tasks/Src/UARTReceiveTask.su ./Tasks/Src/UARTTransmitTask.cyclo ./Tasks/Src/UARTTransmitTask.d ./Tasks/Src/UARTTransmitTask.o ./Tasks/Src/UARTTransmitTask.su ./Tasks/Src/UltrasonicTask.cyclo ./Tasks/Src/UltrasonicTask.d ./Tasks/Src/UltrasonicTask.o ./Tasks/Src/UltrasonicTask.su

.PHONY: clean-Tasks-2f-Src

