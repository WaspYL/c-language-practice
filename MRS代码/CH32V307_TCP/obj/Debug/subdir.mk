################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

C_DEPS += \
./Debug/debug.d 

OBJS += \
./Debug/debug.o 

DIR_OBJS += \
./Debug/*.o \

DIR_DEPS += \
./Debug/*.d \

DIR_EXPANDS += \
./Debug/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/NetLib" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Core" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Debug" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Peripheral/inc" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

