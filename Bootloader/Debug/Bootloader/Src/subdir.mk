################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bootloader/Src/Bootloader.c 

OBJS += \
./Bootloader/Src/Bootloader.o 

C_DEPS += \
./Bootloader/Src/Bootloader.d 


# Each subdirectory must supply rules for building sources it contributes
Bootloader/Src/%.o Bootloader/Src/%.su Bootloader/Src/%.cyclo: ../Bootloader/Src/%.c Bootloader/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Bootloader/Bootloader/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Bootloader-2f-Src

clean-Bootloader-2f-Src:
	-$(RM) ./Bootloader/Src/Bootloader.cyclo ./Bootloader/Src/Bootloader.d ./Bootloader/Src/Bootloader.o ./Bootloader/Src/Bootloader.su

.PHONY: clean-Bootloader-2f-Src

