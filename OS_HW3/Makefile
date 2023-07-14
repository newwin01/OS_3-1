all: bmalloc.h bmalloc.c test1.c test2.c test3.c
	gcc -o test1 test1.c bmalloc.c
	gcc -o test2 test2.c bmalloc.c
	gcc -o test3 test3.c bmalloc.c 
	gcc -o test4_1 test4.c bmalloc.c
	gcc -o test4_2 test4.c bmalloc.c -D_1
 
test:
	gcc -o test2_packed test2_packed_test.c bmalloc_packed_test.c -D_PACKED
	gcc -o test2_unpacked test2_packed_test.c bmalloc_packed_test.c

clean:
	rm -rf test1 test2 test3 test4_1 test4_2 test2_packed test2_unpacked bmalloc.o
