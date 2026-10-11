################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NetLib/eth_driver_10M.c 

C_DEPS += \
./NetLib/eth_driver_10M.d 

OBJS += \
./NetLib/eth_driver_10M.o 

DIR_OBJS += \
./NetLib/*.o \

DIR_DEPS += \
./NetLib/*.d \

DIR_EXPANDS += \
./NetLib/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
NetLib/%.o: ../NetLib/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/NetLib" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Core" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Debug" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/Peripheral/inc" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS代码/CH32V307_TCP/User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

