------------------------------------------------------------------------------
                        Secure Systems Engineering
              RISE Lab, Indian Institute of Technology Madras
------------------------------------------------------------------------------
-- Handling Data in PIC: Position Independent Code ---

This is the readme for understanding PIC data present in libraries. 
It involves a simple library that we build, and then we understand how the 
compiler and the loader together makes it relocatable.
------------------------------------------------------------------------------

* With PIC data, the address of functions or global data is only
  set at runtime by a table, which is present in the executable

 This code should run on an Ubuntu Linux processor. To make PIC executables,
 run

$ make lib_pic

 This would create a library libmylib_pic.so and the dissassembly of the 
 library libmylib_pic.so.diss

* Open libmylib_pic.so.diss in your favorite text editor, 
  and search for set_mylib_int. It would look something like this:

0000045c <set_mylib_int>:
 45c:   55                      push   %ebp
 45d:   89 e5                   mov    %esp,%ebp
 45f:   e8 2b 00 00 00          call   48f <__i686.get_pc_thunk.cx>
 464:   81 c1 80 11 00 00       add    $0x1180,%ecx
 46a:   8b 81 f8 ff ff ff       mov    -0x8(%ecx),%eax
 470:   8b 55 08                mov    0x8(%ebp),%edx
 473:   89 10                   mov    %edx,(%eax)
 475:   5d                      pop    %ebp
 476:   c3                      ret

These lines show the disassembled instructions for set_mylib_int.
Lines 45c, 45d: creates the stack frame for the function
Line  45f     : calls a compiler specifed function called 
                __i686.get_pc_thunk.cx. This function moves the address 
                of the next instruction into ecx. This happens because the 
                address of the next instruction is stored in memory pointed 
                to by esp, this function simply moves (esp) to ecx. 
                Thus, here, the address corresponding to 464 will be moved into                 
                ecx. Now, the memory access can be done relative to the eip.
Line 464      : add an offset to ecx. This offset corresponds to the offset
                of the GOT.
Line 46a      : the offset of mylib_int in the GOT is now moved into the 
                eax register.
Line 470      : copy x (function argument) to the edx register
Line 473      : move x to location pointed to by eax, which contains the 
                address of the global mylib_int
Line 477      : resets the old stackframe
Line 478      : returns from the function.

* Where is the GOT table in the process address space?
  To find this run the command 

$ readelf -S dpic

 The output will look something like this.
 Note the [22] entry corresponds to the GOT table

 There are 38 section headers, starting at offset 0xa88:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        08048114 000114 000013 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            08048128 000128 000020 00   A  0   0  4
  [ 3] .note.gnu.build-i NOTE            08048148 000148 000024 00   A  0   0  4
  [ 4] .hash             HASH            0804816c 00016c 000044 04   A  6   0  4
  [ 5] .gnu.hash         GNU_HASH        080481b0 0001b0 00003c 04   A  6   0  4
  [ 6] .dynsym           DYNSYM          080481ec 0001ec 0000c0 10   A  7   1  4
  [ 7] .dynstr           STRTAB          080482ac 0002ac 0000a9 00   A  0   0  1
  [ 8] .gnu.version      VERSYM          08048356 000356 000018 02   A  6   0  2
  [ 9] .gnu.version_r    VERNEED         08048370 000370 000020 00   A  7   1  4
  [10] .rel.dyn          REL             08048390 000390 000008 08   A  6   0  4
  [11] .rel.plt          REL             08048398 000398 000020 08   A  6  13  4
  [12] .init             PROGBITS        080483b8 0003b8 000030 00  AX  0   0  4
  [13] .plt              PROGBITS        080483e8 0003e8 000050 04  AX  0   0  4
  [14] .text             PROGBITS        08048440 000440 00016c 00  AX  0   0 16
  [15] .fini             PROGBITS        080485ac 0005ac 00001c 00  AX  0   0  4
  [16] .rodata           PROGBITS        080485c8 0005c8 000008 00   A  0   0  4
  [17] .eh_frame         PROGBITS        080485d0 0005d0 000004 00   A  0   0  4
  [18] .ctors            PROGBITS        080495d4 0005d4 000008 00  WA  0   0  4
  [19] .dtors            PROGBITS        080495dc 0005dc 000008 00  WA  0   0  4
  [20] .jcr              PROGBITS        080495e4 0005e4 000004 00  WA  0   0  4
  [21] .dynamic          DYNAMIC         080495e8 0005e8 0000d8 08  WA  7   0  4
  [22] .got              PROGBITS        080496c0 0006c0 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        080496c4 0006c4 00001c 04  WA  0   0  4
  [24] .data             PROGBITS        080496e0 0006e0 000008 00  WA  0   0  4
  [25] .bss              NOBITS          080496e8 0006e8 000008 00  WA  0   0  4
  [26] .comment          PROGBITS        00000000 0006e8 00001c 01  MS  0   0  1
  [27] .debug_aranges    PROGBITS        00000000 000704 000020 00      0   0  1
  [28] .debug_pubnames   PROGBITS        00000000 000724 00001b 00      0   0  1
  [29] .debug_info       PROGBITS        00000000 00073f 00008e 00      0   0  1
  [30] .debug_abbrev     PROGBITS        00000000 0007cd 000048 00      0   0  1
  [31] .debug_line       PROGBITS        00000000 000815 000039 00      0   0  1
  [32] .debug_frame      PROGBITS        00000000 000850 00002c 00      0   0  4
  [33] .debug_str        PROGBITS        00000000 00087c 000087 01  MS  0   0  1
  [34] .debug_loc        PROGBITS        00000000 000903 00002c 00      0   0  1
  [35] .shstrtab         STRTAB          00000000 00092f 000156 00      0   0  1
  [36] .symtab           SYMTAB          00000000 001078 0004a0 10     37  53  4
  [37] .strtab           STRTAB          00000000 001518 000208 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings)
  I (info), L (link order), G (group), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)


* Now lets see how it works at runtime.

$ make driver

* To get the driver code to run, we first must export LD_LIBRARY_PATH so 
  that the program knows where to look for mylib library.

$ export LD_LIBRARY_PATH=./

* Then we run the executable through gdb as follows. Follow the gdb commands
  as shown.

$ gdb ./dpic
.
.
.
(gdb) b set_mylib_int
Breakpoint 1 at 0x8048418
(gdb) r
Starting program: /home/chester/sse/aslr/reloc/dpic 

Breakpoint 1, set_mylib_int (x=100) at mylib.c:5
5		mylib_int = x;

(gdb) p/x &mylib_int
$1 = 0xb7fdf604

## This is the address assigned to mylib_int by the loader. Now, the
## GOT table should also have an entry that points to this memory location.

(gdb) dissemble 
Dump of assembler code for function set_mylib_int:
0xb7fde45c <set_mylib_int+0>:	push   %ebp
0xb7fde45d <set_mylib_int+1>:	mov    %esp,%ebp
0xb7fde45f <set_mylib_int+3>:	call   0xb7fde48f <__i686.get_pc_thunk.cx>
0xb7fde464 <set_mylib_int+8>:	add    $0x1180,%ecx
0xb7fde46a <set_mylib_int+14>:	mov    -0x8(%ecx),%eax
0xb7fde470 <set_mylib_int+20>:	mov    0x8(%ebp),%edx
0xb7fde473 <set_mylib_int+23>:	mov    %edx,(%eax)
0xb7fde475 <set_mylib_int+25>:	pop    %ebp
0xb7fde476 <set_mylib_int+26>:	ret    
End of assembler dump.

## So, The function _i686.get_pc_thunk.cx should push the return address
## to the ecx register. Thus, ecx will contain 0xb7fde464.
## The GOT is present at location 0xb7fde464 + 0x1180 = 0xB7FDF5E4
## The GOT entry for mylib_int is present at an offset of -0x8. 
## This should be 0xB7FDF5E4 - 0x8 = 0xB7FDF5DC. 
## The contents of 0xB7FDF5DC should point to mylib_int (at 0xb7fdf604). 
   Let us verify this ...

(gdb) x/4x 0xB7FDF5DC
0xb7fdf5dc:	0xb7fdf604	0xb7eb45f0	0x0000150c	0xb7fe18b0

 tada! we have 0xb7fd604 in the GOT. This is the pointer to the addres of
 mylib_int.

** Try it yourself **
 Using gdb, change the contents in the GOT to point to glob so that the program 
 outputs the contents of glob rather than mylib_int.
 You can change the contents of a memory location using gdb using the following
 command

(gdb) set *((int *) 0xb7fdf5dc) = 0xaabbccdd

## This would change the contents of location 0xb7fdf5dc with 0xaabbccdd.

