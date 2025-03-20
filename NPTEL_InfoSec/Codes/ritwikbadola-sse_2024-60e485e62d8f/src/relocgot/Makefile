lib_reloc: 
	gcc -m32 -g -c mylib.c -o mylib.o
	gcc -m32 -shared -o libmylib.so mylib.o
	objdump --disassemble-all libmylib.so > libmylib.so.diss


lib_pic: 
	gcc -m32 -fpic -g -c mylib.c -o mylib.o
	gcc -m32 -fpic -shared -o libmylib_pic.so mylib.o
	objdump --disassemble-all libmylib_pic.so > libmylib_pic.so.diss

driver:
	gcc -g -m32 driver.c -L. -lmylib -o dreloc

driver_pic:
	gcc -g -m32 driver.c -L. -lmylib_pic -o dpic
clean:
	rm -f *.o *.so *.*.diss
	rm -f dreloc dpic
