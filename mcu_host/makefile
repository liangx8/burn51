.PHONY: clean
#CC=as31
CC=asemw
#BATTERY TYPE 1:NIHM 2:LIPO

CFLAG=/COLUMNS 
TARGET = burn
#TARGET = phone_charger
#TARGET = exercise
#TARGET = test
AS=d:/p/mcu_dev/sdcc/bin/sdas8051.exe
LD=d:/p/mcu_dev/sdcc/bin/sdld.exe -i
#AS=sdas8051.exe
#LD=sdld.exe -i


CFLAGS=-l -s -o

all:b320.ihx
	@echo "*** Done ***"
$(TARGET).hex:$(TARGET).asm
	${CC} ${CFLAG} ${TARGET}.asm
b320.ihx:b320.ref
	${LD} $@ $<
clean:
	${RM} -rf *.ihx *.ref *.sym *.lst

%.ref:%.asm
	${AS} ${CFLAGS} $@ $<
