.PHONY: build all clean test

all: build run

build:
	gcc list.c queue.c process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc list.c queue.c minHeap.c scheduler.c -o scheduler.out
	gcc list.c process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

run:
	./process_generator.out

