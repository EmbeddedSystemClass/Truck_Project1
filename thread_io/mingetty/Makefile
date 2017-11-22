# Makefile3
######################################################################################
# GNU GCC ARM Embeded Toolchain base directories and binaries 
######################################################################################
#GCC_BASE = /home/dan/dev/arm/crosstool/arm-none-linux-gnueabi/
#GCC_BASE = /home/dan/dev/arm/crosstool/gcc-3.4.4-glibc-2.3.2/arm-linux/
#GCC_BASE = /home/dan/dev/arm/crosstool/gcc-4.0.1-glibc-2.3.5/arm-linux/
GCC_BASE = $(CROSS_COMPILE)
GCC_BIN  = $(GCC_BASE)bin/
GCC_LIB  = $(GCC_BASE)lib/
GCC_INC  = $(GCC_BASE)include/
AS       = $(GCC_BIN)arm-linux-as
CC       = $(GCC_BIN)arm-linux-gcc
CPP      = $(GCC_BIN)arm-linux-g++
LD       = $(GCC_BIN)arm-linux-gcc
OBJCOPY  = $(GCC_BIN)arm-linux-objcopy
OBJDUMP  = $(GCC_BIN)arm-linux-objdump

TS-7200_CC_FLAGS = -mcpu=arm920t
#TS-7200_CC_FLAGS = -mcpu=arm920t -march=armv5t
#TS-7200_CC_FLAGS = -mcpu=arm920t -mapcs-32 -mthumb-interwork
#this option gives "./testarm not found" when using eabi
#TS-7200_CC_FLAGS = -mcpu=arm920t -mthumb -mthumb-interwork
TS-7200_LIB_PATH = $(GCC_LIB)armv5t-m
ASM_FLAGS = -almns=listing.txt

# to debug use: make print-<variable_name>
# e.g. make print-GCC_BASE
print-%:
	@echo '$*=$($*)'
#	@echo $(.SOURCE)
#	@echo $(.TARGET)

######################################################################################
# Main makefile project configuration
#    PROJECT      = <name of the target to be built>
#    MCU_CC_FLAGS = <one of the CC_FLAGS above>
#    MCU_LIB_PATH = <one of the LIB_PATH above>
#    OPTIMIZE_FOR = < SIZE or nothing >
#    DEBUG_LEVEL  = < -g compiler option or nothing >
#    OPTIM_LEVEL  = < -O compiler option or nothing >
######################################################################################
PROJECT           = mingetty
MCU_CC_FLAGS      = $(TS-7200_CC_FLAGS)
MCU_LIB_PATH      = $(TS-7200_LIB_PATH)
OPTIMIZE_FOR      = 
DEBUG_LEVEL       = 
OPTIM_LEVEL       = 
PROJECT_LIB_PATHS = -L.
PROJECT_LIBRARIES =
PROJECT_SYMBOLS   = -DTOOLCHAIN_GCC_ARM -DNO_RELOC='0'  

######################################################################################
# Main makefile system configuration
######################################################################################
SYS_OBJECTS = 
SYS_INC_PATHS = -I$(GCC_INC) 
SYS_LIB_PATHS = -L$(MCU_LIB_PATH)
ifeq (OPTIMIZE_FOR, SIZE)
SYS_LIBRARIES = -lstdc++_s -lsupc++_s -lm -lc_s -lg_s -lnosys
SYS_LD_FLAGS  = --specs=nano.specs -u _printf_float -u _scanf_float
else 
SYS_LIBRARIES = -lstdc++ -lsupc++ -lm -lc -lg -lnosys
SYS_LD_FLAGS  = 
endif

############################################################################### 
# Command line building
###############################################################################
ifdef DEBUG_LEVEL
CC_DEBUG_FLAGS = -g$(DEBUG_LEVEL)
CC_SYMBOLS = -DDEBUG $(PROJECT_SYMBOLS)
else
CC_DEBUG_FLAGS =
CC_SYMBOLS = -DNODEBUG $(PROJECT_SYMBOLS)
endif 

ifdef OPTIM_LEVEL
CC_OPTIM_FLAGS = -O$(OPTIM_LEVEL)
else 
CC_OPTIM_FLAGS = 
endif

INCLUDE_PATHS = -I. $(SYS_INC_PATHS)
LIBRARY_PATHS = $(PROJECT_LIB_LIB) $(SYS_LIB_PATHS)
CC_FLAGS = $(MCU_CC_FLAGS) $(CC_OPTIM_FLAGS) $(CC_DEBUG_FLAGS) -fno-common -fmessage-length=0 -Wall -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer
LD_FLAGS = $(MCU_CC_FLAGS) -Wl,--gc-sections $(SYS_LD_FLAGS) -Wl,-Map=$(PROJECT).map 
LD_SYS_LIBS = $(SYS_LIBRARIES)

BULD_TARGET = $(PROJECT)

all:	main

main:
	$(CC) -c $(INCLUDE_PATHS) $(CC_FLAGS) mingetty.c
	$(CC) -o $(PROJECT) mingetty.o

clean:
	rm *.o; rm mingetty

