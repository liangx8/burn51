.PHONY: clean
CC=gcc
RM=rm -rf
WINDRES=windres
CFLAGS= -Wall -g -DUNICODE -m32
SRC=${wildcard *.c}
OBJS=${SRC:.c=.o}
RCS=${wildcard *.rc}
RES=${RCS:.rc=.res}
LDFLAGS=${FLAG} siutil.lib
LDFLAGS+=-lcomctl32
LDFLAGS+=--enable-auto-import
LDFLAGS+=--static
#LDFLAGS+=-lmingw32
#LDFLAGS+=-lgdi32
TARGET=wburn

all:${TARGET}.exe
	@echo "*** Done ***"
${TARGET}.exe:${OBJS} ${RES}
	${CC} ${OBJS} ${RES} -o $@ ${LDFLAGS}

clean:
	${RM} *.o *.exe *.res

.c.o:
	${CC} -c $< ${CFLAGS} -o $@

%.res:%.rc
	${WINDRES} -DGCC_WINDRES -O coff $< $@
