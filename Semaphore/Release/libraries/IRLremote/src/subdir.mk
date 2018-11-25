################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:\eclipse-oxygen-ardu\arduinoPlugin\libraries\IRLremote\2.0.2\src\IRLremote.cpp 

LINK_OBJ += \
.\libraries\IRLremote\src\IRLremote.cpp.o 

CPP_DEPS += \
.\libraries\IRLremote\src\IRLremote.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\IRLremote\src\IRLremote.cpp.o: C:\eclipse-oxygen-ardu\arduinoPlugin\libraries\IRLremote\2.0.2\src\IRLremote.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\eclipse-oxygen-ardu\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR   -I"C:\eclipse-oxygen-ardu\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\cores\arduino" -I"C:\eclipse-oxygen-ardu\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\variants\eightanaloginputs" -I"C:\eclipse-oxygen-ardu\arduinoPlugin\libraries\IRLremote\2.0.2" -I"C:\eclipse-oxygen-ardu\arduinoPlugin\libraries\IRLremote\2.0.2\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


