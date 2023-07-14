make:
	gcc -pthread multiple_producer.c -lm -o multi
	gcc -pthread single_producer.c -lm -o single
timer:
	gcc single_producer_timer.c -lm -pthread -o single_timer
	gcc multiple_producer_timer.c -lm -pthread -o multi_timer
debug:
	gcc -Wall -D_DEBUG -pthread multiple_producer.c -lm -o multi
	gcc -Wall -D_DEBUG -pthread single_producer.c -lm -o single