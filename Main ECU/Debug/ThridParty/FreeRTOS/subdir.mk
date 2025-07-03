################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThridParty/FreeRTOS/croutine.c \
../ThridParty/FreeRTOS/event_groups.c \
../ThridParty/FreeRTOS/list.c \
../ThridParty/FreeRTOS/queue.c \
../ThridParty/FreeRTOS/stream_buffer.c \
../ThridParty/FreeRTOS/tasks.c \
../ThridParty/FreeRTOS/timers.c 

OBJS += \
./ThridParty/FreeRTOS/croutine.o \
./ThridParty/FreeRTOS/event_groups.o \
./ThridParty/FreeRTOS/list.o \
./ThridParty/FreeRTOS/queue.o \
./ThridParty/FreeRTOS/stream_buffer.o \
./ThridParty/FreeRTOS/tasks.o \
./ThridParty/FreeRTOS/timers.o 

C_DEPS += \
./ThridParty/FreeRTOS/croutine.d \
./ThridParty/FreeRTOS/event_groups.d \
./ThridParty/FreeRTOS/list.d \
./ThridParty/FreeRTOS/queue.d \
./ThridParty/FreeRTOS/stream_buffer.d \
./ThridParty/FreeRTOS/tasks.d \
./ThridParty/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
ThridParty/FreeRTOS/%.o ThridParty/FreeRTOS/%.su ThridParty/FreeRTOS/%.cyclo: ../ThridParty/FreeRTOS/%.c ThridParty/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/include" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS/portable/GCC/ARM_CM3" -I"D:/studying/GitHub/Graduation Project/Main ECU/ThridParty/FreeRTOS" -I"D:/studying/GitHub/Graduation Project/Main ECU/Diagnostics/Inc" -I"D:/studying/GitHub/Graduation Project/Main ECU/Tasks/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThridParty-2f-FreeRTOS

clean-ThridParty-2f-FreeRTOS:
	-$(RM) ./ThridParty/FreeRTOS/croutine.cyclo ./ThridParty/FreeRTOS/croutine.d ./ThridParty/FreeRTOS/croutine.o ./ThridParty/FreeRTOS/croutine.su ./ThridParty/FreeRTOS/event_groups.cyclo ./ThridParty/FreeRTOS/event_groups.d ./ThridParty/FreeRTOS/event_groups.o ./ThridParty/FreeRTOS/event_groups.su ./ThridParty/FreeRTOS/list.cyclo ./ThridParty/FreeRTOS/list.d ./ThridParty/FreeRTOS/list.o ./ThridParty/FreeRTOS/list.su ./ThridParty/FreeRTOS/queue.cyclo ./ThridParty/FreeRTOS/queue.d ./ThridParty/FreeRTOS/queue.o ./ThridParty/FreeRTOS/queue.su ./ThridParty/FreeRTOS/stream_buffer.cyclo ./ThridParty/FreeRTOS/stream_buffer.d ./ThridParty/FreeRTOS/stream_buffer.o ./ThridParty/FreeRTOS/stream_buffer.su ./ThridParty/FreeRTOS/tasks.cyclo ./ThridParty/FreeRTOS/tasks.d ./ThridParty/FreeRTOS/tasks.o ./ThridParty/FreeRTOS/tasks.su ./ThridParty/FreeRTOS/timers.cyclo ./ThridParty/FreeRTOS/timers.d ./ThridParty/FreeRTOS/timers.o ./ThridParty/FreeRTOS/timers.su

.PHONY: clean-ThridParty-2f-FreeRTOS

