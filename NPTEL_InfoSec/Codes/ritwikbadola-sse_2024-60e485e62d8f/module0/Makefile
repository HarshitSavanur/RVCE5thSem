FLAGS = -fno-stack-protector -z execstack -no-pie -m32

EXES = example1


all: $(EXES) 


$(EXES):
	gcc $(FLAGS) $@.c -o $@

clean:
	rm -f $(EXES)
