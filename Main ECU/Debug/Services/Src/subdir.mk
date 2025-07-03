################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Services/Src/CANReceiveTask.c \
../Services/Src/CANTransmitTask.c \
../Services/Src/ControlTask.c \
../Services/Src/DTC.c \
../Services/Src/Diagnostics.c \
../Services/Src/DiagnosticsTask.c \
../Services/Src/OTAManagerTask.c \
../Services/Src/RTC.c \
../Services/Src/TaskHandle.c 

OBJS += \
./Services/Src/CANReceiveTask.o \
./Services/Src/CANTransmitTask.o \
./Services/Src/ControlTask.o \
./Services/Src/DTC.o \
./Services/Src/Diagnostics.o \
./Services/Src/DiagnosticsTask.o \
./Services/Src/OTAManagerTask.o \
./Services/Src/RTC.o \
./Services/Src/TaskHandle.o 

C_DEPS += \
./Services/Src/CANReceiveTask.d \
./Services/Src/CANTransmitTask.d \
./Services/Src/ControlTask.d \
./Services/Src/DTC.d \
./Services/Src/Diagnostics.d \
./Services/Src/DiagnosticsTask.d \
./Services/Src/OTAManagerTask.d \
./Services/Src/RTC.d \
./Services/Src/TaskHandle.d 


# Each subdirectory must supply rules for building sources it contributes
Services/Src/%.o Services/Src/%.su Services/Src/%.cyclo: ../Services/Src/%.c Services/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/Main ECU/Services/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Services-2f-Src

clean-Services-2f-Src:
	-$(RM) ./Services/Src/CANReceiveTask.cyclo ./Services/Src/CANReceiveTask.d ./Services/Src/CANReceiveTask.o ./Services/Src/CANReceiveTask.su ./Services/Src/CANTransmitTask.cyclo ./Services/Src/CANTransmitTask.d ./Services/Src/CANTransmitTask.o ./Services/Src/CANTransmitTask.su ./Services/Src/ControlTask.cyclo ./Services/Src/ControlTask.d ./Services/Src/ControlTask.o ./Services/Src/ControlTask.su ./Services/Src/DTC.cyclo ./Services/Src/DTC.d ./Services/Src/DTC.o ./Services/Src/DTC.su ./Services/Src/Diagnostics.cyclo ./Services/Src/Diagnostics.d ./Services/Src/Diagnostics.o ./Services/Src/Diagnostics.su ./Services/Src/DiagnosticsTask.cyclo ./Services/Src/DiagnosticsTask.d ./Services/Src/DiagnosticsTask.o ./Services/Src/DiagnosticsTask.su ./Services/Src/OTAManagerTask.cyclo ./Services/Src/OTAManagerTask.d ./Services/Src/OTAManagerTask.o ./Services/Src/OTAManagerTask.su ./Services/Src/RTC.cyclo ./Services/Src/RTC.d ./Services/Src/RTC.o ./Services/Src/RTC.su ./Services/Src/TaskHandle.cyclo ./Services/Src/TaskHandle.d ./Services/Src/TaskHandle.o ./Services/Src/TaskHandle.su

.PHONY: clean-Services-2f-Src

