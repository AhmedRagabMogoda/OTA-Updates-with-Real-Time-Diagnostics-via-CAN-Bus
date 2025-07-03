################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Diagnostics/Src/DTC.c \
../Diagnostics/Src/RTC.c \
../Diagnostics/Src/UDS.c 

OBJS += \
./Diagnostics/Src/DTC.o \
./Diagnostics/Src/RTC.o \
./Diagnostics/Src/UDS.o 

C_DEPS += \
./Diagnostics/Src/DTC.d \
./Diagnostics/Src/RTC.d \
./Diagnostics/Src/UDS.d 


# Each subdirectory must supply rules for building sources it contributes
Diagnostics/Src/%.o Diagnostics/Src/%.su Diagnostics/Src/%.cyclo: ../Diagnostics/Src/%.c Diagnostics/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Main ECU/Diagnostics/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/Tasks/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Diagnostics-2f-Src

clean-Diagnostics-2f-Src:
	-$(RM) ./Diagnostics/Src/DTC.cyclo ./Diagnostics/Src/DTC.d ./Diagnostics/Src/DTC.o ./Diagnostics/Src/DTC.su ./Diagnostics/Src/RTC.cyclo ./Diagnostics/Src/RTC.d ./Diagnostics/Src/RTC.o ./Diagnostics/Src/RTC.su ./Diagnostics/Src/UDS.cyclo ./Diagnostics/Src/UDS.d ./Diagnostics/Src/UDS.o ./Diagnostics/Src/UDS.su

.PHONY: clean-Diagnostics-2f-Src

