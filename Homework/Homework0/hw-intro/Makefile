CC?=gcc
CFLAGS?=-g
BINARIES=map limits

default:
	make map
	make limits

map: map.c
	$(CC) $(CFLAGS) map.c recurse.c -o map

limits: limits.c
	$(CC) $(CFLAGS) limits.c -o limits

clean:
	rm -f $(BINARIES)
