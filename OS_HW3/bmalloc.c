#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h"
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

bm_option bm_mode = BestFit;
bm_header bm_list_head = {0, 0, 0x0}; // initial location

void *sibling(void *h)
{
	bm_header_ptr itr = bm_list_head.next;
	bm_header_ptr prev;
	bm_header *header = (bm_header *)h;
	long long result = 1;

	int offset = 1;
	int head_offset = 1;
	int LeftorRight;

	for (itr = bm_list_head.next; itr != 0x0; itr = itr->next)
	{ // Find two consecutive same one
		result = result + (1 << itr->size);
		if (itr == header)
		{
			result = result / (1 << itr->size);
			LeftorRight = result % 2;
			if (LeftorRight == 0)
			{
				if (prev->size == itr->size)
				{
					return prev;
				}
			}
			else
			{
				if (itr->size == itr->next->size)
				{
					return itr->next;
				}
			}
		}
		prev = itr;
	}

	return 0x0; // no sibling
}

int fitting(size_t s)
{
	int exponent = 1;
	int count = 0;
	int num = s;

	while (exponent < num)
	{
		exponent = exponent * 2;
		count++;
	}

	#ifdef _DEBUG
		printf("Fitting size: %d\n\n", count);
	#endif

	return count;
}

void split(bm_header_ptr head, size_t fit_size)
{ // split and split -> recursion split?

	#ifdef _DEBUG
		fprintf(stderr, "Initial Head size: %d\n", head->size);
	#endif

	int new_size;
	int offset = 1;
	void *new_addr;
	while (new_size > fit_size)
	{
		new_size = head->size - 1;
		head->size = head->size - 1;
		offset = 1 << new_size;
		new_addr = (void *)head + offset;
		bm_header *new_bm_header = (bm_header *)new_addr;
	#ifdef _DEBUG
		fprintf(stderr, "size of offset: %d\n", offset);
		fprintf(stderr, "size of new_size: %d\n", new_size);
		fprintf(stderr, "location of head: %p\n", head);
		fprintf(stderr, "location of new_addr: %p\n", new_addr);
		fprintf(stderr, "location of new_addr: %p\n\n\n", new_bm_header);
	#endif
		(*new_bm_header).next = head->next;
		(*new_bm_header).size = new_size;
		(*new_bm_header).used = 0;
		head->next = new_bm_header;
	}
}

void assign_new_page(bm_header_ptr head)
{
	void *start_addr;
	start_addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

	if (start_addr == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}

	bm_header *new_bm_header = (bm_header *)start_addr; // assign to the memory

	(*new_bm_header).next = 0x0;
	(*new_bm_header).size = 12;
	(*new_bm_header).used = 0;

	head->next = new_bm_header;


}

void cleaning(void *h)
{
	long offset = 1;
	int new_size;
	void *addr;
	bm_header *old_bm_header = (bm_header *)h;
	new_size = old_bm_header->size;

	addr = h + sizeof(bm_header);

	offset = 1 << new_size;
	
	memset(addr, 0, offset - sizeof(bm_header));
}

void *bmalloc(size_t s)
{
	
	if (s == 0) // in case of receiving 0
	{
		return 0x0;
	}
	else if (s > 4096 - sizeof(bm_header)) // in case of receiving more than 4096 - sizeof(bm_header)
	{
		printf("\nsize error, the size you requested exceeds the maximum available size\n");
		exit(1);
	}

	// TODO
	void *start_addr;
	size_t fit_size;

	// if it is not initialized yet
	if (bm_list_head.next == NULL)
	{
		start_addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		if (start_addr == MAP_FAILED)
		{
			perror("mmap");
			exit(1);
		}

	#ifdef _DEBUG
		fprintf(stderr, "start address: %p\n\n", start_addr);
	#endif

		bm_header *new_bm_header = (bm_header *)start_addr; // assign to the memory

		(*new_bm_header).next = 0x0;
		(*new_bm_header).size = 12;
		(*new_bm_header).used = 0;

		bm_list_head.next = new_bm_header;
		bm_list_head.used = 1;


	}

	fit_size = fitting(s + sizeof(bm_header)); // size of header


	int min_size = 12;
	// if size existed
	bm_header_ptr itr;
	bm_header_ptr prev;
	bm_header_ptr split_pointer = NULL;

	void *addr;

	if (bm_mode == FirstFit)
	{
		for (itr = bm_list_head.next; itr != 0x0; itr = itr->next)
		{ // First Fit
			if (itr->size >= fit_size && itr->used == 0)
			{ // The first one
				split_pointer = itr;
				break;
			}
			prev = itr; // prev pointer
		}
		if (split_pointer == NULL)
		{ // need to allocate new page and link
			assign_new_page(prev);
			split_pointer = prev->next;
		}

		if (split_pointer->size == fit_size)
		{ // Find the proper one, so allocate
			split_pointer->used = 1;
			addr = split_pointer;

			cleaning(addr);
			return addr + sizeof(bm_header);
		}
		else
		{
			fprintf(stderr, "%s", "");
			split(split_pointer, fit_size);
			for (itr = bm_list_head.next; itr->next != 0x0; itr = itr->next)
			{ // First Fit
				if (itr->size >= fit_size && itr->used == 0)
				{ // The first one
					itr->used = 1;
					addr = itr;

					cleaning(addr);
					return addr + sizeof(bm_header);
				}
			}
		}
	}

	else if (bm_mode == BestFit)
	{ // find the smallest one
		for (itr = bm_list_head.next; itr != 0x0; itr = itr->next)
		{ // First Fit
			if (itr->size >= fit_size && itr->used == 0)
			{ // update min_size with the minimum
				if (min_size >= itr->size)
				{
					min_size = itr->size;
					split_pointer = itr;
				}
			}
			prev = itr; // prev pointer
		}
		if (split_pointer == NULL)
		{ // need to allocate new page and link
			assign_new_page(prev);
			split_pointer = prev->next;
		}

		if (split_pointer->size == fit_size)
		{ // Find the proper one, so allocate
			split_pointer->used = 1;
			addr = split_pointer;


			cleaning(addr);
			return addr + sizeof(bm_header);
		}
		else
		{
			fprintf(stderr, "%s", "");
			split(split_pointer, fit_size);
		#ifdef DEBUG
			for (itr = bm_list_head.next; itr != 0x0; itr = itr->next)
			{ // Iteration
				fprintf(stderr, "%d\n", itr->size);
			}
		#endif
			for (itr = split_pointer; itr->next != 0x0; itr = itr->next)
			{ // Best Fit
				if (itr->size == fit_size && itr->used == 0)
				{ // The best one
					itr->used = 1;
					addr = itr;

					cleaning(addr);
					return addr + sizeof(bm_header);
				}
			}
		}
	}

	// if there is no appropriate size

	return 0x0; // error
}



void bfree(void *p)
{
	if (p == 0x0)
		return; // when null pointer free requested, should not do any action

	bm_header_ptr itr;

	for (itr = bm_list_head.next; itr != NULL; itr = itr->next)
	{
		if (p == (char *)itr + sizeof(bm_header))
		{
			break;
		}
	}

	if (itr == NULL)
	{
		printf("\nfree error, you may have requested to free the memory that is not allocated.\n");
		exit(1);
	}

	bm_header *old_bm_header = (bm_header *)(p - sizeof(bm_header));

	old_bm_header->used = 0;


	//freeing and merging

	bm_header *sibling_bm_header = sibling(old_bm_header);
	#ifdef _DEBUG
		fprintf(stderr, "Old_bm_header: %p\n", old_bm_header);
		fprintf(stderr, "Sibling Node: %p\n", sibling_bm_header);
	#endif

	while (sibling_bm_header != 0x0 && sibling_bm_header->used == 0)
	{
		// case of merging
		// which one is the faster one

		if (sibling_bm_header->next == old_bm_header)
		{ // sibling head is faster
			sibling_bm_header->size = sibling_bm_header->size + 1;
			memcpy( &(*sibling_bm_header).next, &(*old_bm_header).next, sizeof(bm_header_ptr));
			
			memset(old_bm_header, 0 , sizeof(bm_header));
			
			if (sibling_bm_header->size == 12)
				break;
			memcpy( &old_bm_header, &sibling_bm_header, sizeof(bm_header_ptr));
			
			sibling_bm_header = sibling(sibling_bm_header);
		}
		else
		{
			old_bm_header->size = old_bm_header->size + 1;
			memcpy( &(*old_bm_header).next, &(*sibling_bm_header).next, sizeof(bm_header_ptr));

			memset(sibling_bm_header, 0 , sizeof(bm_header));
			if (old_bm_header->size == 12)
				break;
			
			// memcpy(old_bm_header, sibling_bm_header, sizeof(bm_header_ptr));
			sibling_bm_header = sibling(old_bm_header);
			
		}
		#ifdef _DEBUG
			fprintf(stderr, "Sibling Node: %p\n", sibling_bm_header);
			fprintf(stderr, "Old Node: %p\n", old_bm_header);
			
		#endif

	}

	// release memory mapped

	bm_header_ptr prev = &bm_list_head;

	for (itr = bm_list_head.next; itr != 0x0; itr = itr->next)
	{
		if (itr->size == 12 && itr->used == 0)
		{ // need to be unmapped and discard
			prev->next = itr->next;
			munmap(itr, 4096);
			break;
		}
		prev = itr;
	}
}

void *brealloc(void *p, size_t s)
{
	int offset = 1;
	int temp;

	if (s == 0)
	{
		return 0x0;
	}
	else
	{
		temp = fitting(s + sizeof(bm_header));
	}

	if (p == NULL)
	{ // just like realloc
		return bmalloc(s);
	}

	bm_header *old_bm_header = (bm_header *)(p - sizeof(bm_header)); // from the start of the block

	if (old_bm_header->size == temp)
	{ // no change
		return p;
	}

	// Allocate a new block of memory of size s

	void *new_p = bmalloc(s);

	// Copy the contents of the old block to the new block
	memcpy(new_p, p, ((1 << temp) - sizeof(bm_header)));

	// Free the old block
	bfree(p);

	// Return the pointer to the new block
	return new_p;
}

void bmconfig(bm_option opt)
{
	bm_mode = opt;
}

void bmprint()
{
	bm_header_ptr itr;
	size_t all = 0, given = 0, available, sumRequested = 0;
	int i, j;

	printf("==================== bm_list ====================\n");
	for (itr = bm_list_head.next, i = 0; itr != 0x0; itr = itr->next, i++)
	{
		printf("%3d:%p:%1d %8d:", i, ((void *)itr) + sizeof(bm_header), (int)itr->used, (int)itr->size);
		all += (1 << itr->size);
		if (itr->used == 1)
		{
			given += (1 << itr->size);
		}
		char *s = ((char *)itr) + sizeof(bm_header);
		for (j = 0; j < (itr->size >= 8 ? 8 : itr->size); j++)
			printf("%02x ", s[j]);
		printf("\n");
	}
	printf("=================================================\n");

	// TODO: print out the stat's
	printf("==================== Statistics ====================\n");

	// the total amount of all given memory
	printf("The total amount of all given memory : %zu bytes\n", all);
	// the total amount of memory given to the users
	printf("The total amount of memory given to the users : %zu bytes\n", given);
	// the total amount of available memory
	available = all - given;
	printf("The total amount of available memory : %zu bytes\n", available);

	printf("====================================================\n");
}