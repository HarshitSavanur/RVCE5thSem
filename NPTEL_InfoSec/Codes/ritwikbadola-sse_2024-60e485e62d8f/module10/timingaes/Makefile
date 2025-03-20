attack: attack.c common.o
	gcc -I. -Ilib/ -O3 attack.c -o attack lib/aes_1024.o common.o -lm
#	gcc -I. -O3 attack.c -o attack lib/aes_2048.o common.o -lm
#	gcc -I. -O3 attack.c -o attack lib/aes8_1024.o common.o -lm



common.o: common.c
	gcc -O3 common.c  -c -o common.o



clean:
	rm -f attack correlate *.o
