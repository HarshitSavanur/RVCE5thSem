------------------------------------------------------------------------------
                        Secure Systems Engineering
              RISE Lab, Indian Institute of Technology Madras
------------------------------------------------------------------------------
-- Handling Functions using PLT: Position Independent Code ---

This is the readme for understanding PIC data present in libraries. 
It involves a simple library that we build, and then we understand how the 
compiler and the loader together makes it relocatable. The library manages
a doubly linked list.
------------------------------------------------------------------------------

 This code should run on an Ubuntu Linux processor. To make the executable,
 run

$ make lib

 This would create a library libmylib.so and the dissassembly of the 
 library libmylib.so.diss. Then run this,

$ make driver

  This would create a driver executable, which invokes various functions
  in the mylib library. The make would also create a dissassembly file 
  driver.diss. A small snippet from the dissassembly of main is shown below:

080485ed <main>:
 80485ed:       55                      push   %ebp
 80485ee:       89 e5                   mov    %esp,%ebp
 80485f0:       83 e4 f0                and    $0xfffffff0,%esp
 80485f3:       83 ec 20                sub    $0x20,%esp
 80485f6:       c7 04 24 2c a0 04 08    movl   $0x804a02c,(%esp)
 80485fd:       e8 ae fe ff ff          call   80484b0 <init@plt>
 8048602:       e8 d9 fe ff ff          call   80484e0 <newnode@plt>
 8048607:       89 44 24 14             mov    %eax,0x14(%esp)
 804860b:       8b 44 24 14             mov    0x14(%esp),%eax
 804860f:       83 c0 08                add    $0x8,%eax
 8048612:       66 c7 00 41 00          movw   $0x41,(%eax)
 8048617:       e8 c4 fe ff ff          call   80484e0 <newnode@plt>
  :                 :                    :       :
  :                 :                    :       :

 Interesting here are the lines 80485fd, 8048602, and 8048617.
 Notice that the compiler has replaced the library functions with
 something@plt. For example newnode is replaced with newnode@plt,
 init is replaced with init@plt. These inserted functions are present
 in a special section of the ELF executable known as the .plt section.

* Let us have a look to find out where the .plt section resides. 
  Run the following command. A sample output is given. Notice section
  [12] is the .plt section. It starts at location 0x08048480. This location
  is important for the future. Remember it ;). Also important is the .got.plt
  section ([23], starting at 0804a000)
  

$ readelf -S ./driver
There are 35 section headers, starting at offset 0x153c:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        08048154 000154 000013 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            08048168 000168 000020 00   A  0   0  4
  [ 3] .note.gnu.build-i NOTE            08048188 000188 000024 00   A  0   0  4
  [ 4] .gnu.hash         GNU_HASH        080481ac 0001ac 000040 04   A  5   0  4
  [ 5] .dynsym           DYNSYM          080481ec 0001ec 000110 10   A  6   1  4
  [ 6] .dynstr           STRTAB          080482fc 0002fc 0000dd 00   A  0   0  1
  [ 7] .gnu.version      VERSYM          080483da 0003da 000022 02   A  5   0  2
  [ 8] .gnu.version_r    VERNEED         080483fc 0003fc 000020 00   A  6   1  4
  [ 9] .rel.dyn          REL             0804841c 00041c 000010 08   A  5   0  4
  [10] .rel.plt          REL             0804842c 00042c 000030 08   A  5  12  4
  [11] .init             PROGBITS        0804845c 00045c 000023 00  AX  0   0  4
  [12] .plt              PROGBITS        08048480 000480 000070 04  AX  0   0 16
  [13] .text             PROGBITS        080484f0 0004f0 000202 00  AX  0   0 16
  [14] .fini             PROGBITS        080486f4 0006f4 000014 00  AX  0   0  4
  [15] .rodata           PROGBITS        08048708 000708 000008 00   A  0   0  4
  [16] .eh_frame_hdr     PROGBITS        08048710 000710 00002c 00   A  0   0  4
  [17] .eh_frame         PROGBITS        0804873c 00073c 0000b0 00   A  0   0  4
  [18] .init_array       INIT_ARRAY      08049f00 000f00 000004 00  WA  0   0  4
  [19] .fini_array       FINI_ARRAY      08049f04 000f04 000004 00  WA  0   0  4
  [20] .jcr              PROGBITS        08049f08 000f08 000004 00  WA  0   0  4
  [21] .dynamic          DYNAMIC         08049f0c 000f0c 0000f0 08  WA  6   0  4
  [22] .got              PROGBITS        08049ffc 000ffc 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        0804a000 001000 000024 04  WA  0   0  4
  [24] .data             PROGBITS        0804a024 001024 000008 00  WA  0   0  4
  [25] .bss              NOBITS          0804a02c 00102c 00001c 00  WA  0   0  4
  [26] .comment          PROGBITS        00000000 00102c 00002b 01  MS  0   0  1
  [27] .debug_aranges    PROGBITS        00000000 001057 000020 00      0   0  1
  [28] .debug_info       PROGBITS        00000000 001077 000159 00      0   0  1
  [29] .debug_abbrev     PROGBITS        00000000 0011d0 0000dc 00      0   0  1
  [30] .debug_line       PROGBITS        00000000 0012ac 000050 00      0   0  1
  [31] .debug_str        PROGBITS        00000000 0012fc 0000fa 01  MS  0   0  1
  [32] .shstrtab         STRTAB          00000000 0013f6 000146 00      0   0  1
  [33] .symtab           SYMTAB          00000000 001ab4 0004c0 10     34  50  4
  [34] .strtab           STRTAB          00000000 001f74 000264 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)


* PLT works in the following way.
  1. The first invocation of a function (say newnode in the C code) invokes
     newnode@plt. This plt function, would use a resolver present in an 
     inbuilt library to resolve the address of newnode. This address of
     newnode is filled in in the .plt.got table. Then, the function 
     newnode is executed. 
  2. Every subsequent invocation of newnode looks up the .plt.got table 
     to directly determine the address of newnode (no need to call the
     resolver). The execution is directly goes to newnode.

  The first invocation of the function has considerabl overheads because
  of the additional resolver that gets invoked. The subsequent invocation
  of newnode would have less overheads. There would be an additional jmp
  instruction... that's it!

* Now lets look at how this works in practice. We are going to look at
  the first and second invocation of newnode (in driver.c). 
  To start of, set the LD_LIBRARY_PATH so that the mylib library location
  can be found (refer to relocgot README files for details). Then, run our 
  favorite gdb

$ gdb ./driver
  :
  :
  :
(gdb) disassemble main
Dump of assembler code for function main:
   0x080485ed <+0>:	push   %ebp
   0x080485ee <+1>:	mov    %esp,%ebp
   0x080485f0 <+3>:	and    $0xfffffff0,%esp
   0x080485f3 <+6>:	sub    $0x20,%esp
   0x080485f6 <+9>:	movl   $0x804a02c,(%esp)
   0x080485fd <+16>:	call   0x80484b0 <init@plt>
   0x08048602 <+21>:	call   0x80484e0 <newnode@plt>
   0x08048607 <+26>:	mov    %eax,0x14(%esp)
   0x0804860b <+30>:	mov    0x14(%esp),%eax
   0x0804860f <+34>:	add    $0x8,%eax
   0x08048612 <+37>:	movw   $0x41,(%eax)
   0x08048617 <+42>:	call   0x80484e0 <newnode@plt>

(gdb) break 12    # the line number of the first invocation of newnode (driver.c)
(gdb) break 15    # the line number of the 2nd invocation of newnode (driver.c)
(gdb) run
12		n1 = newnode();
(gdb) si
0x080484e0 in newnode@plt ()  # we have now entered the plt function

# disassemble it
(gdb) disassemble 
Dump of assembler code for function newnode@plt:
=> 0x080484e0 <+0>:	jmp    *0x804a020
   0x080484e6 <+6>:	push   $0x28
   0x080484eb <+11>:	jmp    0x8048480
End of assembler dump.

# First note that this code is in the .plt section of the executable.
# The first line (0x080484e0) makes an indirect jmp to (0x804a020).
# Let us see what this location contains:

(gdb) x/x 0x804a020
0x804a020 <newnode@got.plt>:	0x080484e6

# This is the .got.plt entry for the newnode function.
# The indirect jmp (jmp *), would jump to location stored in (0x804a020).
# Note that this location contains 0x080484e6, which is the second 
# instruction in newnode@plt. 
# So, essentially, there is a jump to the next instruction in the code.

# The third line at 0x080484eb, is a jmp to the resolver, which resolves
# the address of newnode and fills it in the .got.plt (at location 0x804a020).
# Lets see this happen...

(gdb) si
0x080484e6 in newnode@plt ()
(gdb) si
0x080484eb in newnode@plt ()
(gdb) 
0x08048480 in ?? ()
(gdb) 
0x08048486 in ?? ()
(gdb) 
0xf7ff0620 in ?? () from /lib/ld-linux.so.2  # invokes the resolver
(gdb) 
0xf7ff0621 in ?? () from /lib/ld-linux.so.2  
(gdb) finish                             # completes execution of the resolver
Run till exit from #0  0xf7ff0621 in ?? () from /lib/ld-linux.so.2
0x08048607 in main (argc=1, argv=0xffffd154) at driver.c:12
12		n1 = newnode();

# Let's check the contents of .got.plt now.

(gdb) x/x 0x804a020
0x804a020 <newnode@got.plt>:	0xf7fd6651
(gdb) p newnode
$1 = {node_t *()} 0xf7fd6651 <newnode>

# Voila! the got.plt now has a pointer to newnode, which is present
# at 0xf7fd6651. In every subsequent call to newnode, the jmp * 
# in newnode@plt, # would jump to 0xf7fd6651, there skipping the 
# resolver invocation.

