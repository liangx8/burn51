.PHONY: clean
#CC=as31
#CC=asemw
#BATTERY TYPE 1:NIHM 2:LIPO
ASRC=${shell ls *.s}
OBJS=${ASRC:.s=.rel}

LFLAGS=-m
LFLAGS+=-M
LFLAGS+=-i

TARGET = br
#TARGET = phone_charger
#TARGET = exercise
#TARGET = test
#AS=d:/p/mcu_dev/sdcc/bin/sdas8051.exe
#LD=d:/p/mcu_dev/sdcc/bin/sdld.exe -i
AS=sdas8051
LD=sdld


CFLAGS=-l -s -o

all:${TARGET}.hex
	@echo "*** Done ***"
${TARGET}.hex:${OBJS}
	${LD} $@ ${LFLAGS} ${OBJS}
clean:
	${RM} -rf *.hex *.rel *.sym *.lst *.map *.mem

%.rel:%.s
	${AS} ${CFLAGS} $@ $<
