################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Core/Src/can.c \
../Core/Core/Src/conversor.c \
../Core/Core/Src/main.c \
../Core/Core/Src/stm32h5xx_hal_msp.c \
../Core/Core/Src/stm32h5xx_it.c \
../Core/Core/Src/syscalls.c \
../Core/Core/Src/sysmem.c \
../Core/Core/Src/system_stm32h5xx.c \
../Core/Core/Src/uart.c 

OBJS += \
./Core/Core/Src/can.o \
./Core/Core/Src/conversor.o \
./Core/Core/Src/main.o \
./Core/Core/Src/stm32h5xx_hal_msp.o \
./Core/Core/Src/stm32h5xx_it.o \
./Core/Core/Src/syscalls.o \
./Core/Core/Src/sysmem.o \
./Core/Core/Src/system_stm32h5xx.o \
./Core/Core/Src/uart.o 

C_DEPS += \
./Core/Core/Src/can.d \
./Core/Core/Src/conversor.d \
./Core/Core/Src/main.d \
./Core/Core/Src/stm32h5xx_hal_msp.d \
./Core/Core/Src/stm32h5xx_it.d \
./Core/Core/Src/syscalls.d \
./Core/Core/Src/sysmem.d \
./Core/Core/Src/system_stm32h5xx.d \
./Core/Core/Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Core/Src/%.o Core/Core/Src/%.su Core/Core/Src/%.cyclo: ../Core/Core/Src/%.c Core/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H533xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Core-2f-Src

clean-Core-2f-Core-2f-Src:
	-$(RM) ./Core/Core/Src/can.cyclo ./Core/Core/Src/can.d ./Core/Core/Src/can.o ./Core/Core/Src/can.su ./Core/Core/Src/conversor.cyclo ./Core/Core/Src/conversor.d ./Core/Core/Src/conversor.o ./Core/Core/Src/conversor.su ./Core/Core/Src/main.cyclo ./Core/Core/Src/main.d ./Core/Core/Src/main.o ./Core/Core/Src/main.su ./Core/Core/Src/stm32h5xx_hal_msp.cyclo ./Core/Core/Src/stm32h5xx_hal_msp.d ./Core/Core/Src/stm32h5xx_hal_msp.o ./Core/Core/Src/stm32h5xx_hal_msp.su ./Core/Core/Src/stm32h5xx_it.cyclo ./Core/Core/Src/stm32h5xx_it.d ./Core/Core/Src/stm32h5xx_it.o ./Core/Core/Src/stm32h5xx_it.su ./Core/Core/Src/syscalls.cyclo ./Core/Core/Src/syscalls.d ./Core/Core/Src/syscalls.o ./Core/Core/Src/syscalls.su ./Core/Core/Src/sysmem.cyclo ./Core/Core/Src/sysmem.d ./Core/Core/Src/sysmem.o ./Core/Core/Src/sysmem.su ./Core/Core/Src/system_stm32h5xx.cyclo ./Core/Core/Src/system_stm32h5xx.d ./Core/Core/Src/system_stm32h5xx.o ./Core/Core/Src/system_stm32h5xx.su ./Core/Core/Src/uart.cyclo ./Core/Core/Src/uart.d ./Core/Core/Src/uart.o ./Core/Core/Src/uart.su

.PHONY: clean-Core-2f-Core-2f-Src

