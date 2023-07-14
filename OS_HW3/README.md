## ITP 30002-{02} Operating Systems, Spring 2023
### Homework 3. bmalloc: buddy heap memory allocator

### *Contributer: Sechang Jang, Narin Han*

---

Since the era of multiprogramming began, effective utilization of the CPU has risen as an important issue. Managing interactivity became the key as many processes use the machine concurrently. Consequently, the protection of the address should be considered, and recent Operating Systems decided to create an abstraction of physical memory separating the process as segments. 

In Buddy allocation, free memory is considered as a big chunk with a size of 2^N. When a certain memory space is requested from the user, free space is recursively divided by two until the block is enough to handle the requested memory space. Buddy allocation works simply to determine the buddy of a particular block. 

To see the result with the example test cases, execute make file. 

> make <br>
./test1 // simple test case to see the overall execution<br>
./test2 // receive integer, negative integer to delete the received integer and 0 to print out, using bmalloc library<br>
./test3 // linked list implemented using bmlloc library <br>
./test4 // test case to see the how bmalloc handle exceptional cases such as requesting size 0 or free not allocated space

To see uncompacked header version, execute *make test*. test2_packed_test.c, bmalloc_packed_test.c and its header file is for this test case. It uses global variable in the file to shows the internal fragmentation. 

> make test <br>
./test2_unpacked // unpacked test case is not compacted. 
./test2_packed // packed test case is successfully compacted using \__attribute__ ((\__packed__)). It shows how internal fragmenation is made due to the compactness in this specfic case

<br>
<br>

##### To see overall execution of algorithm, you can use -D_DEBUG option when you compile the code.