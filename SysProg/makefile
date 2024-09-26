CC = gcc
TARGET = pctest
TARGET_PNG = libpng/a.out
OBJS = main.o lib/helper.o lib/lib.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

.c.o :
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJS) $(TARGET)
    
.PHONY : clean