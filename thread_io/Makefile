######################################################################################
# GNU GCC ARM Embeded Toolchain base directories and binaries 
######################################################################################
#GCC_BASE = /home/dan/dev/arm/opt/crosstool/gcc-3.4.4-glibc-2.3.2/arm-linux/
GCC_BASE = /home/dan/dev/arm/crosstool/gcc3/arm-linux/
GCC_BIN  = $(GCC_BASE)bin/
GCC_LIB  = $(GCC_BASE)arm-linux/lib/
GCC_INC  = $(GCC_BASE)arm-linux/include/
AS       = $(GCC_BIN)arm-linux-as
CC       = $(GCC_BIN)arm-linux-gcc
CPP      = $(GCC_BIN)arm-linux-g++
LD       = $(GCC_BIN)arm-linux-gcc
OBJCOPY  = $(GCC_BIN)arm-linux-objcopy

#TS-7200_CC_FLAGS = -mcpu=arm920t -march=armv5t
#TS-7200_CC_FLAGS = -mcpu=arm920t -mapcs-32 -mthumb-interwork
#TS-7200_CC_FLAGS = -mcpu=arm920t
TS-7200_CC_FLAGS = -mcpu=arm920t -mapcs-32 -mthumb-interwork
ASM_FLAGS = -almns=listing.txt
PROJECT_INC_LIB = -I$(PORT) -I$(SOURCE)

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
PROJECT           = main
MCU_CC_FLAGS      = $(TS-7200_CC_FLAGS)
MCU_LIB_PATH      = $(TS-7200_LIB_PATH)
OPTIMIZE_FOR      = 
DEBUG_LEVEL       = 
OPTIM_LEVEL       = 
PROJECT_OBJECTS   = main.o
PROJECT_LIB_PATHS = -L.
PROJECT_LIBRARIES =
PROJECT_SYMBOLS   = -DTOOLCHAIN_GCC_ARM -DNO_RELOC='0'  

######################################################################################
# Main makefile system configuration
######################################################################################
SYS_OBJECTS = 
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

INCLUDE_PATHS = -I.$(GCC_INC)
LIBRARY_PATHS = $(PROJECT_LIB_LIB) $(SYS_LIB_PATHS)
CC_FLAGS = $(MCU_CC_FLAGS) $(CC_OPTIM_FLAGS) $(CC_DEBUG_FLAGS) -Wall -fno-exceptions -ffunction-sections -fdata-sections 
#-pthread -static-libgcc
# use -static-libgcc instead of -static to get rid of warning: "Using getprotobyname in statically linked apps requires
# at runtime the shared libraries from the glibc version used for linking"
LD_FLAGS = $(MCU_CC_FLAGS) -Wl,--gc-sections $(SYS_LD_FLAGS) -Wl,-Map=$(PROJECT).map 
LD_SYS_LIBS = $(SYS_LIBRARIES)

#GNUCFLAGS = -g -ansi -Wstrict-prototypes	doesn't compile "// ..comments.."
#CC_FLAGS = -static -g -Wstrict-prototypes -mcpu=arm920t -mapcs-32 -mthumb-interwork
CC_FLAGS = -g -Wstrict-prototypes -mcpu=arm920t -mapcs-32
GNULDFLAGS_T = ${GNULDFLAGS} -pthread
#CC_FLAGST = ${CC_FLAGS} + GNULDFLAGS_T
GNUSFLAGS = -D_SVID_SOURCE -D_XOPEN_SOURCE
GNUNOANSI = -g -gnu99 -Wstrict-prototypes

#####################################################
#CFLAGS = ${GNUCFLAGS}
#LDFLAGS = ${GNULDFLAGS}

BULD_TARGET = $(PROJECT)

all : sched init_db list_db
#llist_test_threads llist_test_threads_rw

sched.o: sched.c tasks.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c -pthread sched.c

ioports.o: ioports.c ioports.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c ioports.c

tasks.o: tasks.c tasks.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c -pthread tasks.c

serial_io.o: serial_io.c serial_io.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c serial_io.c

illist_threads_rw.o: queue/illist_threads_rw.c queue/illist_threads_rw.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c queue/illist_threads_rw.c

ollist_threads_rw.o: queue/ollist_threads_rw.c queue/ollist_threads_rw.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c queue/ollist_threads_rw.c

rdwr.o: queue/rdwr.c queue/rdwr.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c queue/rdwr.c

config_file.o: ncurses/config_file.c queue/illist_threads_rw.h queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c ncurses/config_file.c

init_db.o: ncurses/init_db.c queue/illist_threads_rw.h queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c ncurses/init_db.c

list_db.o: ncurses/list_db.c queue/illist_threads_rw.h queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c ncurses/list_db.c

sched: sched.o ioports.o tasks.o illist_threads_rw.o ollist_threads_rw.o rdwr.o serial_io.o config_file.o
# if we don't use -static then the target should have: libpthread.so.0 in the /lib directory
	${CC} -pthread -static sched.o ioports.o tasks.o illist_threads_rw.o ollist_threads_rw.o rdwr.o serial_io.o config_file.o -o sched
#	${CC} -pthread sched.o ioports.o tasks.o illist_threads_rw.o ollist_threads_rw.o rdwr.o serial_io.o config_file.o -o sched

init_db: config_file.o init_db.o
	${CC} init_db.o config_file.o -o init_db

list_db: config_file.o list_db.o
	${CC} list_db.o config_file.o -o list_db

clean :
	rm -f *.o *~ *# core  \
	sched init_db list_db

