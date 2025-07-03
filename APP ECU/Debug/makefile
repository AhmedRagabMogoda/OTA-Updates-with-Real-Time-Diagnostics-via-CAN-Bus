################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ThridParty/FreeRTOS/portable/MemMang/subdir.mk
-include ThridParty/FreeRTOS/portable/GCC/ARM_CM3/subdir.mk
-include ThridParty/FreeRTOS/subdir.mk
-include Tasks/Src/subdir.mk
-include HAL/Ultrasonic\ sensor/Src/subdir.mk
-include HAL/Motor/Src/subdir.mk
-include HAL/LM35/Src/subdir.mk
-include HAL/Bluetooth/Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := APP ECU
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
APP\ ECU.elf \

MAP_FILES += \
APP\ ECU.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
APP\ ECU.list \

OBJCOPY_HEX += \
APP\ ECU.hex \

OBJCOPY_BIN += \
APP\ ECU.bin \


# All Target
all: main-build

# Main-build Target
main-build: APP\ ECU.elf secondary-outputs

# Tool invocations
APP\ ECU.elf APP\ ECU.map: $(OBJS) $(USER_OBJS) D:\studying\GitHub\Graduation\ Project\APP\ ECU\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "APP ECU.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\studying\GitHub\Graduation Project\APP ECU\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="APP ECU.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

APP\ ECU.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "APP ECU.list"
	@echo 'Finished building: $@'
	@echo ' '

APP\ ECU.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "APP ECU.hex"
	@echo 'Finished building: $@'
	@echo ' '

APP\ ECU.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "APP ECU.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) APP\ ECU.bin APP\ ECU.elf APP\ ECU.hex APP\ ECU.list APP\ ECU.map APP\\ ECU.elf default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
