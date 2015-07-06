CC=g++
WINDRES=windres
CFLAG=-Wall -g
LIBS=-lintl -lcomctl32
OBJS=fw.o groupasm.o

fw.exe:$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)
.cpp.o:
	$(CC) -c $< $(CFLAG)

%.res: %.rc resource.h
	${WINDRES} -DGCC_WINDRES -O coff $< $@

clean:
	rm -rf *.exe *.o *.pot *.c~ *.cpp~
