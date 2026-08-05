################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADC.c \
../src/Delay.c \
../src/EEPROM.c \
../src/GPIO.c \
../src/HMI.c \
../src/LCD.c \
../src/main.c 

OBJS += \
./src/ADC.o \
./src/Delay.o \
./src/EEPROM.o \
./src/GPIO.o \
./src/HMI.o \
./src/LCD.o \
./src/main.o 

C_DEPS += \
./src/ADC.d \
./src/Delay.d \
./src/EEPROM.d \
./src/GPIO.d \
./src/HMI.d \
./src/LCD.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/ADC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


