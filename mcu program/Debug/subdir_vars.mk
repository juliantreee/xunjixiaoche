################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../empty.syscfg 

C_SRCS += \
../Graysensor.c \
../Motor.c \
../delay.c \
../empty.c \
./ti_msp_dl_config.c \
C:/TI/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c \
../encoder.c 

GEN_CMDS += \
./device_linker.cmd 

GEN_FILES += \
./device_linker.cmd \
./device.opt \
./ti_msp_dl_config.c 

C_DEPS += \
./Graysensor.d \
./Motor.d \
./delay.d \
./empty.d \
./ti_msp_dl_config.d \
./startup_mspm0g350x_ticlang.d \
./encoder.d 

GEN_OPTS += \
./device.opt 

OBJS += \
./Graysensor.o \
./Motor.o \
./delay.o \
./empty.o \
./ti_msp_dl_config.o \
./startup_mspm0g350x_ticlang.o \
./encoder.o 

GEN_MISC_FILES += \
./device.cmd.genlibs \
./ti_msp_dl_config.h \
./Event.dot 

OBJS__QUOTED += \
"Graysensor.o" \
"Motor.o" \
"delay.o" \
"empty.o" \
"ti_msp_dl_config.o" \
"startup_mspm0g350x_ticlang.o" \
"encoder.o" 

GEN_MISC_FILES__QUOTED += \
"device.cmd.genlibs" \
"ti_msp_dl_config.h" \
"Event.dot" 

C_DEPS__QUOTED += \
"Graysensor.d" \
"Motor.d" \
"delay.d" \
"empty.d" \
"ti_msp_dl_config.d" \
"startup_mspm0g350x_ticlang.d" \
"encoder.d" 

GEN_FILES__QUOTED += \
"device_linker.cmd" \
"device.opt" \
"ti_msp_dl_config.c" 

C_SRCS__QUOTED += \
"../Graysensor.c" \
"../Motor.c" \
"../delay.c" \
"../empty.c" \
"./ti_msp_dl_config.c" \
"C:/TI/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c" \
"../encoder.c" 

SYSCFG_SRCS__QUOTED += \
"../empty.syscfg" 


