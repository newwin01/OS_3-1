#include <stdio.h>
#include "bmalloc.h"

int 
main ()
{
	void *p1, *p2, *p3, *p4 ;

	bmprint() ;    

	p1 = bmalloc(2000) ; 
	printf("bmalloc(2000):%p\n", p1) ; 
	bmprint() ;

	int a = 10 ;
	int* temp = (int*) p1 ;
	*temp = a ;

	bmprint() ;
	
	// succesfully brealloc or not
	p2 = brealloc(p1, 10) ;
	printf("brealloc(%p, 10):%p\n", p1, p2) ; 
	bmprint() ;

	// in case of requesting 0 size
    p3 = bmalloc(0) ;
    printf("bmalloc(0):%p\n", p3) ; 
    bmprint() ;

	#ifdef _1
	// in case of requesting more than the maximum capacity
	p4 = bmalloc(10000) ;
    printf("bmalloc(10000):%p\n", p4) ; 
    bmprint() ;
	#endif

	// try to free the null pointer
	bfree(0x0);
	bmprint() ;

	// try to free that is not allocated
	p4 = (void*) p1 - 10;
	bfree(p4);
	bmprint();
}
