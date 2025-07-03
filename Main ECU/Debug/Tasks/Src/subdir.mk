################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/Src/CANReceiveTask.c \
../Tasks/Src/CANTransmitTask.c \
../Tasks/Src/ControlTask.c \
../Tasks/Src/DiagnosticsTask.c \
../Tasks/Src/OTAManagerTask.c \
../Tasks/Src/TaskHandle.c \
../Tasks/Src/UARTReceiveTask.c 

OBJS += \
./Tasks/Src/CANReceiveTask.o \
./Tasks/Src/CANTransmitTask.o \
./Tasks/Src/ControlTask.o \
./Tasks/Src/DiagnosticsTask.o \
./Tasks/Src/OTAManagerTask.o \
./Tasks/Src/TaskHandle.o \
./Tasks/Src/UARTReceiveTask.o 

C_DEPS += \
./Tasks/Src/CANReceiveTask.d \
./Tasks/Src/CANTransmitTask.d \
./Tasks/Src/ControlTask.d \
./Tasks/Src/DiagnosticsTask.d \
./Tasks/Src/OTAManagerTask.d \
./Tasks/Src/TaskHandle.d \
./Tasks/Src/UARTReceiveTask.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/Src/%.o Tasks/Src/%.su Tasks/Src/%.cyclo: ../Tasks/Src/%.c Tasks/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Main ECU/Diagnostics/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/Tasks/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Tasks-2f-Src

clean-Tasks-2f-Src:
	-$(RM) ./Tasks/Src/CANReceiveTask.cyclo ./Tasks/Src/CANReceiveTask.d ./Tasks/Src/CANReceiveTask.o ./Tasks/Src/CANReceiveTask.su ./Tasks/Src/CANTransmitTask.cyclo ./Tasks/Src/CANTransmitTask.d ./Tasks/Src/CANTransmitTask.o ./Tasks/Src/CANTransmitTask.su ./Tasks/Src/ControlTask.cyclo ./Tasks/Src/ControlTask.d ./Tasks/Src/ControlTask.o ./Tasks/Src/ControlTask.su ./Tasks/Src/DiagnosticsTask.cyclo ./Tasks/Src/DiagnosticsTask.d ./Tasks/Src/DiagnosticsTask.o ./Tasks/Src/DiagnosticsTask.su ./Tasks/Src/OTAManagerTask.cyclo ./Tasks/Src/OTAManagerTask.d ./Tasks/Src/OTAManagerTask.o ./Tasks/Src/OTAManagerTask.su ./Tasks/Src/TaskHandle.cyclo ./Tasks/Src/TaskHandle.d ./Tasks/Src/TaskHandle.o ./Tasks/Src/TaskHandle.su ./Tasks/Src/UARTReceiveTask.cyclo ./Tasks/Src/UARTReceiveTask.d ./Tasks/Src/UARTReceiveTask.o ./Tasks/Src/UARTReceiveTask.su

.PHONY: clean-Tasks-2f-Src

