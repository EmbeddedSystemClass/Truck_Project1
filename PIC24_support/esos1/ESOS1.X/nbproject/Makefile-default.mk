#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../Truck_App2.c ../../esos/src/esos.c ../../esos/src/esos_cb.c ../../esos/src/esos_comm.c ../../esos/src/esos_mail.c ../../esos/src/pic24/esos_pic24_irq.c ../../esos/src/pic24/esos_pic24_rs232.c ../../esos/src/pic24/esos_pic24_tick.c ../../lib/src/dataXferImpl.c ../../lib/src/pic24_clockfreq.c ../../lib/src/pic24_configbits.c ../../lib/src/pic24_serial.c ../../lib/src/pic24_stdio_uart.c ../../lib/src/pic24_timer.c ../../lib/src/pic24_uart.c ../../lib/src/pic24_util.c /home/dan/dev/PIC24_support/lib/src/pic24_adc.c /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/Truck_App2.o ${OBJECTDIR}/_ext/1022787335/esos.o ${OBJECTDIR}/_ext/1022787335/esos_cb.o ${OBJECTDIR}/_ext/1022787335/esos_comm.o ${OBJECTDIR}/_ext/1022787335/esos_mail.o ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o ${OBJECTDIR}/_ext/957557178/dataXferImpl.o ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o ${OBJECTDIR}/_ext/957557178/pic24_configbits.o ${OBJECTDIR}/_ext/957557178/pic24_serial.o ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o ${OBJECTDIR}/_ext/957557178/pic24_timer.o ${OBJECTDIR}/_ext/957557178/pic24_uart.o ${OBJECTDIR}/_ext/957557178/pic24_util.o ${OBJECTDIR}/_ext/822243278/pic24_adc.o ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/Truck_App2.o.d ${OBJECTDIR}/_ext/1022787335/esos.o.d ${OBJECTDIR}/_ext/1022787335/esos_cb.o.d ${OBJECTDIR}/_ext/1022787335/esos_comm.o.d ${OBJECTDIR}/_ext/1022787335/esos_mail.o.d ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d ${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d ${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d ${OBJECTDIR}/_ext/957557178/pic24_serial.o.d ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d ${OBJECTDIR}/_ext/957557178/pic24_timer.o.d ${OBJECTDIR}/_ext/957557178/pic24_uart.o.d ${OBJECTDIR}/_ext/957557178/pic24_util.o.d ${OBJECTDIR}/_ext/822243278/pic24_adc.o.d ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/Truck_App2.o ${OBJECTDIR}/_ext/1022787335/esos.o ${OBJECTDIR}/_ext/1022787335/esos_cb.o ${OBJECTDIR}/_ext/1022787335/esos_comm.o ${OBJECTDIR}/_ext/1022787335/esos_mail.o ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o ${OBJECTDIR}/_ext/957557178/dataXferImpl.o ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o ${OBJECTDIR}/_ext/957557178/pic24_configbits.o ${OBJECTDIR}/_ext/957557178/pic24_serial.o ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o ${OBJECTDIR}/_ext/957557178/pic24_timer.o ${OBJECTDIR}/_ext/957557178/pic24_uart.o ${OBJECTDIR}/_ext/957557178/pic24_util.o ${OBJECTDIR}/_ext/822243278/pic24_adc.o ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o

# Source Files
SOURCEFILES=../Truck_App2.c ../../esos/src/esos.c ../../esos/src/esos_cb.c ../../esos/src/esos_comm.c ../../esos/src/esos_mail.c ../../esos/src/pic24/esos_pic24_irq.c ../../esos/src/pic24/esos_pic24_rs232.c ../../esos/src/pic24/esos_pic24_tick.c ../../lib/src/dataXferImpl.c ../../lib/src/pic24_clockfreq.c ../../lib/src/pic24_configbits.c ../../lib/src/pic24_serial.c ../../lib/src/pic24_stdio_uart.c ../../lib/src/pic24_timer.c ../../lib/src/pic24_uart.c ../../lib/src/pic24_util.c /home/dan/dev/PIC24_support/lib/src/pic24_adc.c /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ128GB110
MP_LINKER_FILE_OPTION=,--script=p24FJ128GB110.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/Truck_App2.o: ../Truck_App2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Truck_App2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Truck_App2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Truck_App2.c  -o ${OBJECTDIR}/_ext/1472/Truck_App2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Truck_App2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Truck_App2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos.o: ../../esos/src/esos.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos.c  -o ${OBJECTDIR}/_ext/1022787335/esos.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_cb.o: ../../esos/src/esos_cb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_cb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_cb.c  -o ${OBJECTDIR}/_ext/1022787335/esos_cb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_cb.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_cb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_comm.o: ../../esos/src/esos_comm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_comm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_comm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_comm.c  -o ${OBJECTDIR}/_ext/1022787335/esos_comm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_comm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_comm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_mail.o: ../../esos/src/esos_mail.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_mail.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_mail.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_mail.c  -o ${OBJECTDIR}/_ext/1022787335/esos_mail.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_mail.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_mail.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o: ../../esos/src/pic24/esos_pic24_irq.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_irq.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o: ../../esos/src/pic24/esos_pic24_rs232.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_rs232.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o: ../../esos/src/pic24/esos_pic24_tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_tick.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/dataXferImpl.o: ../../lib/src/dataXferImpl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/dataXferImpl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/dataXferImpl.c  -o ${OBJECTDIR}/_ext/957557178/dataXferImpl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o: ../../lib/src/pic24_clockfreq.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_clockfreq.c  -o ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_configbits.o: ../../lib/src/pic24_configbits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_configbits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_configbits.c  -o ${OBJECTDIR}/_ext/957557178/pic24_configbits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_serial.o: ../../lib/src/pic24_serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_serial.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_serial.c  -o ${OBJECTDIR}/_ext/957557178/pic24_serial.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_serial.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_serial.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o: ../../lib/src/pic24_stdio_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_stdio_uart.c  -o ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_timer.o: ../../lib/src/pic24_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_timer.c  -o ${OBJECTDIR}/_ext/957557178/pic24_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_uart.o: ../../lib/src/pic24_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_uart.c  -o ${OBJECTDIR}/_ext/957557178/pic24_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_util.o: ../../lib/src/pic24_util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_util.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_util.c  -o ${OBJECTDIR}/_ext/957557178/pic24_util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_util.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_util.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/822243278/pic24_adc.o: /home/dan/dev/PIC24_support/lib/src/pic24_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/822243278" 
	@${RM} ${OBJECTDIR}/_ext/822243278/pic24_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/822243278/pic24_adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  /home/dan/dev/PIC24_support/lib/src/pic24_adc.c  -o ${OBJECTDIR}/_ext/822243278/pic24_adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/822243278/pic24_adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/822243278/pic24_adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o: /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/373056592" 
	@${RM} ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c  -o ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/Truck_App2.o: ../Truck_App2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Truck_App2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Truck_App2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Truck_App2.c  -o ${OBJECTDIR}/_ext/1472/Truck_App2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Truck_App2.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Truck_App2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos.o: ../../esos/src/esos.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos.c  -o ${OBJECTDIR}/_ext/1022787335/esos.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_cb.o: ../../esos/src/esos_cb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_cb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_cb.c  -o ${OBJECTDIR}/_ext/1022787335/esos_cb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_cb.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_cb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_comm.o: ../../esos/src/esos_comm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_comm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_comm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_comm.c  -o ${OBJECTDIR}/_ext/1022787335/esos_comm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_comm.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_comm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1022787335/esos_mail.o: ../../esos/src/esos_mail.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1022787335" 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_mail.o.d 
	@${RM} ${OBJECTDIR}/_ext/1022787335/esos_mail.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/esos_mail.c  -o ${OBJECTDIR}/_ext/1022787335/esos_mail.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1022787335/esos_mail.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1022787335/esos_mail.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o: ../../esos/src/pic24/esos_pic24_irq.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_irq.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_irq.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o: ../../esos/src/pic24/esos_pic24_rs232.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_rs232.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_rs232.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o: ../../esos/src/pic24/esos_pic24_tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/750133316" 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../esos/src/pic24/esos_pic24_tick.c  -o ${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/750133316/esos_pic24_tick.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/dataXferImpl.o: ../../lib/src/dataXferImpl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/dataXferImpl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/dataXferImpl.c  -o ${OBJECTDIR}/_ext/957557178/dataXferImpl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/dataXferImpl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o: ../../lib/src/pic24_clockfreq.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_clockfreq.c  -o ${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_clockfreq.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_configbits.o: ../../lib/src/pic24_configbits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_configbits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_configbits.c  -o ${OBJECTDIR}/_ext/957557178/pic24_configbits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_configbits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_serial.o: ../../lib/src/pic24_serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_serial.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_serial.c  -o ${OBJECTDIR}/_ext/957557178/pic24_serial.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_serial.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_serial.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o: ../../lib/src/pic24_stdio_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_stdio_uart.c  -o ${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_stdio_uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_timer.o: ../../lib/src/pic24_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_timer.c  -o ${OBJECTDIR}/_ext/957557178/pic24_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_timer.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_uart.o: ../../lib/src/pic24_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_uart.c  -o ${OBJECTDIR}/_ext/957557178/pic24_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_uart.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/957557178/pic24_util.o: ../../lib/src/pic24_util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/957557178" 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_util.o.d 
	@${RM} ${OBJECTDIR}/_ext/957557178/pic24_util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../lib/src/pic24_util.c  -o ${OBJECTDIR}/_ext/957557178/pic24_util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/957557178/pic24_util.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/957557178/pic24_util.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/822243278/pic24_adc.o: /home/dan/dev/PIC24_support/lib/src/pic24_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/822243278" 
	@${RM} ${OBJECTDIR}/_ext/822243278/pic24_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/822243278/pic24_adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  /home/dan/dev/PIC24_support/lib/src/pic24_adc.c  -o ${OBJECTDIR}/_ext/822243278/pic24_adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/822243278/pic24_adc.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/822243278/pic24_adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o: /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/373056592" 
	@${RM} ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  /home/dan/dev/PIC24_support/esos/src/pic24/esos_pic24_spi.c  -o ${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/373056592/esos_pic24_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/ESOS1.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
