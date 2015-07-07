CC=gcc

CFLAG=-Wall -g
LIBS=-lintl
OBJS=fw.o

fw.exe:$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)
.c.o:
	$(CC) -c $< $(CFLAG)

clean:
	rm -rf *.exe *.o *.pot *.c~ *.cpp~
