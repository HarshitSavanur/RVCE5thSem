lib: 
	gcc -m32 -I. -fpic -g -c mylib.c -o mylib.o
	gcc -m32 -shared -fpic -o libmylib.so mylib.o
	objdump --disassemble-all libmylib.so > libmylib.so.diss

driver:
	gcc -m32 -g driver.c -I. -L. -lmylib -o driver
	objdump --disassemble-all driver > driver.diss

assignment:
	gcc -m32 -g assignment.c -I. -L. -lmylib -o assignment
	objdump --disassemble-all assignment > assignment.diss


clean:
	rm -f *.o *.so *.*.diss
	rm -f driver assignment
