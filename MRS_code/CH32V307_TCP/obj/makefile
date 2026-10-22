################################################################################
# MRS Version: 3.0.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf
FLAGS_C :=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/NetLib" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/Core" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/Debug" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/Peripheral/inc" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/User" -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/User/u8g2" -std=gnu99
FLAGS_C_FILE := .compile_c_flags

FLAGS_CPP :=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -std=gnu++11 -fabi-version=0
FLAGS_CPP_FILE := .compile_cpp_flags

FLAGS_ASM :=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -x assembler-with-cpp -I"c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/Startup"
FLAGS_ASM_FILE := .compile_asm_flags

FLAGS_LD:=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "c:\Users\WaspYL\Desktop\c-language-practice\MRS_code\CH32V307_TCP\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"c:\Users\WaspYL\Desktop\c-language-practice\MRS_code\CH32V307_TCP\NetLib" -Wl,-Map,"CH32V307_TCP.map" --specs=nano.specs --specs=nosys.specs -lprintf
FLAGS_LD_FILE := .ld_flags

$(FLAGS_C_FILE): FORCE
	@echo '$(FLAGS_C)' | cmp -s - $@ || echo '$(FLAGS_C)' > $@
$(FLAGS_CPP_FILE): FORCE
	@echo '$(FLAGS_CPP)' | cmp -s - $@ || echo '$(FLAGS_CPP)' > $@
$(FLAGS_ASM_FILE): FORCE
	@echo '$(FLAGS_ASM)' | cmp -s - $@ || echo '$(FLAGS_ASM)' > $@
$(FLAGS_LD_FILE): FORCE
	@echo '$(FLAGS_LD)' | cmp -s - $@ || echo '$(FLAGS_LD)' > $@
.PHONY: FORCE
FORCE:

# All of the sources participating in the build are defined here
-include sources.mk
-include User/u8g2/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include NetLib/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

SOURCES_LIST := .sources_list
$(SOURCES_LIST): FORCE
	@echo '$(C_SRCS) $(CPP_SRCS) $(S_UPPER_SRCS)' | tr ' ' '\n' | sort > $@.tmp
	@cmp -s $@.tmp $@ || mv $@.tmp $@
	@rm -f $@.tmp

LD_SCRIPTS := c:/Users/WaspYL/Desktop/c-language-practice/MRS_code/CH32V307_TCP/Ld/Link.ld

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CH32V307_TCP.hex \

SECONDARY_LIST += \
CH32V307_TCP.lst \

SECONDARY_SIZE += \
CH32V307_TCP.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: CH32V307_TCP.elf secondary-outputs

# Tool invocations
CH32V307_TCP.elf: $(OBJS) $(USER_OBJS_ESCAPE) $(SOURCES_LIST) $(FLAGS_LD_FILE) $(LD_SCRIPTS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "c:\Users\WaspYL\Desktop\c-language-practice\MRS_code\CH32V307_TCP\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"c:\Users\WaspYL\Desktop\c-language-practice\MRS_code\CH32V307_TCP\NetLib" -Wl,-Map,"CH32V307_TCP.map" --specs=nano.specs --specs=nosys.specs -o "CH32V307_TCP.elf" $(OBJS) $(LIBS) -lprintf
CH32V307_TCP.hex: CH32V307_TCP.elf
	@	riscv-none-embed-objcopy -O ihex "CH32V307_TCP.elf" "CH32V307_TCP.hex"
CH32V307_TCP.lst: CH32V307_TCP.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle -M xw --line-numbers --wide "CH32V307_TCP.elf" > "CH32V307_TCP.lst"
CH32V307_TCP.siz: CH32V307_TCP.elf
	riscv-none-embed-size --format=berkeley "CH32V307_TCP.elf"

# Other Targets
clean:
	-$(RM) $(DIR_OBJS) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE) CH32V307_TCP.elf
	-$(RM) $(DIR_EXPANDS) $(CALLGRAPH_DOT)
	-$(RM) $(DIR_DEPS)
	-$(RM) $(FLAGS_C_FILE) $(FLAGS_CPP_FILE) $(FLAGS_ASM_FILE) $(FLAGS_LD_FILE) $(SOURCES_LIST)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets