################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fileManager.c \
../src/firstRead.c \
../src/mmn_14.c 

OBJS += \
./src/fileManager.o \
./src/firstRead.o \
./src/mmn_14.o 

C_DEPS += \
./src/fileManager.d \
./src/firstRead.d \
./src/mmn_14.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


