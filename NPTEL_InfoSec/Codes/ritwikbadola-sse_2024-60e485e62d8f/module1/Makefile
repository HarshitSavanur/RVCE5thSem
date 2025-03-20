FLAGS = -fno-stack-protector -z execstack -m32

EXES = skipinstruction
 
all: $(EXES) 


$(EXES):
	gcc $(FLAGS) $@.c -o $@

clean:
	rm -f $(EXES)
