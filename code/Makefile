.PHONY: build all clean test

all: build run

FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11  `pkg-config --cflags --libs gtk+-3.0`

build:
	gcc list.c queue.c process_generator.c -o process_generator.out $(FLAGS)
	gcc clk.c -o clk.out
	gcc list.c queue.c minHeap.c memory.c scheduler.c -o scheduler.out $(FLAGS)
	gcc list.c process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

run:
	./process_generator.out

