build:
	gcc main.c -o /tmp/memtest
run: build
	/tmp/memtest
	rm /tmp/memtest
