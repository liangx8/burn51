.PHONY: clean
#CC=as31
#CC=asemw
#BATTERY TYPE 1:NIHM 2:LIPO
ASRC= ${shell ls *.s}
#ASRC=main.s burn.s data.s
OBJS=${ASRC:.s=.rel}

LFLAGS=-m
LFLAGS+=-u
LFLAGS+=-M
LFLAGS+=-i
LFLAGS+=-b CSEG=0x7e
#LFLAGS+=-b _CODE=0x80
LFLAGS+=-b DSEG=0x30


TARGET = br
#TARGET = phone_charger
#TARGET = exercise
#TARGET = test
#AS=d:/p/mcu_dev/sdcc/bin/sdas8051.exe
#LD=d:/p/mcu_dev/sdcc/bin/sdld.exe -i
AS=sdas8051
LD=sdld


CFLAGS=-l

all:${TARGET}.ihx
	@mv ${TARGET}.ihx ${TARGET}.hex
	d52 -d  ${TARGET}.hex
	@echo "*** Done ***"
${TARGET}.ihx:${OBJS}
	${LD} $@ ${LFLAGS} ${OBJS}
clean:
	${RM} -rf *.hex *.rel *.sym *.lst *.map *.mem *.rst

%.rel:%.s
	${AS} ${CFLAGS} -o $@ $<
