CC = gcc
TARGET = a.out
OBJS = cimin_try.o lib/helper.o lib/structs.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

.c.o :
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJS) $(TARGET)
    
.PHONY : clean