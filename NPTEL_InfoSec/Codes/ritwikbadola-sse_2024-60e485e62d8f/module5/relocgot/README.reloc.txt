------------------------------------------------------------------------------
                        Secure Systems Engineering
              RISE Lab, Indian Institute of Technology Madras
------------------------------------------------------------------------------
-- Load Time Relocation ---

This is the readme for understanding Load Time Reloation of data present 
in libraries. It involves a simple library that we build, and then
we understand how the compiler and the loader together makes it relocatable
------------------------------------------------------------------------------

* With relocation, the address of global data is only
  set at runtime. Therefore, permitting shared libraries to be relocatable
  in the virtual address space.

* The code should run on an Ubuntu Linux processor. 
  To make relocatable executables, run

$ make lib_reloc

  This would create a library libmylib.so and the dissassembly of the 
  library libmylib.so.diss.

* Open libmylib.so.diss with your favorite text editor, 
  and search for set_mylib_int. It would look something like this:

0000046c <set_mylib_int>:
 46c:   55                      push   %ebp
 46d:   89 e5                   mov    %esp,%ebp
 46f:   8b 45 08                mov    0x8(%ebp),%eax
 472:   a3 00 00 00 00          mov    %eax,0x0
 477:   5d                      pop    %ebp
 478:   c3                      ret

 These lines show the disassembled instructions for set_mylib_int.
 The first column are memory offsents in the mylib library.
 The second column are assembly mnemonics for x86-64.
 The third columns are the equivalent instructions.

Lines 46c and 46d: creates the stack frame for the function
Line  46f        : moves x into the eax register
Line  472        : stores eax register to a location 0x0. 
                   This 0x0 is what the compiler puts for the global data
                   mylib_int. During load time, the loader in the OS, would 
                   replace the 0x0 with the actual address corresponding to 
                   the global variable mylib_int.
Line 477         : resets the old stackframe
Line 478         : returns from the function.

* How does the loader know where replace 0x0 with the address of mylib_int?
 This information is stored in a section of the executable. This section,
 called .rel.dyn, is added during compilation.

To view the .rel.dyn section, run the following command.

$ readelf -r libmylib.so

Relocation section '.rel.dyn' at offset 0x304 contains 6 entries:
 Offset     Info    Type            Sym.Value  Sym. Name
000015ec  00000008 R_386_RELATIVE   
00000473  00000a01 R_386_32          000015f8   mylib_int
0000047d  00000a01 R_386_32          000015f8   mylib_int
000015cc  00000106 R_386_GLOB_DAT    00000000   __gmon_start__
000015d0  00000206 R_386_GLOB_DAT    00000000   _Jv_RegisterClasses
000015d4  00000306 R_386_GLOB_DAT    00000000   __cxa_finalize

 Note the occurances of mylib_int. The first is at 0x473, which
 corresponds to the mov %eax, 0x0 in the dissassembly snippet
 above. The second corresponds to the load in mylib_int.

 :::Do it yourself::::
 Verify that 47d in the disassembly of get_mylib_int actually contans 0x0 
 and is a load instruction from memory to a register eax.

* Finally we will see the effect of the loader at runtime. We first compile 
  driver.c, which uses our newly formed mylib. 
  This is done as follows:

$ make driver

* To get the driver code to run, we first must export LD_LIBRARY_PATH so 
  that the program knows where to look for mylib library.

$ export LD_LIBRARY_PATH=./

* Then we run the executable through gdb as follows. Follow the gdb commands
  as shown.

$ gdb ./a.out
.
.
.
(gdb) b main
Breakpoint 1 at 0x80484fd: file driver.c, line 9.
(gdb) r
Starting program: /home/chester/sse/aslr/reloc/a.out 

Breakpoint 1, main () at driver.c:9
9		set_mylib_int(100);
(gdb) disass set_mylib_int
Dump of assembler code for function set_mylib_int:
0xb7fde46c <set_mylib_int+0>:	push   %ebp
0xb7fde46d <set_mylib_int+1>:	mov    %esp,%ebp
0xb7fde46f <set_mylib_int+3>:	mov    0x8(%ebp),%eax
0xb7fde472 <set_mylib_int+6>:	mov    %eax,0xb7fdf5f8
0xb7fde477 <set_mylib_int+11>:	pop    %ebp
0xb7fde478 <set_mylib_int+12>:	ret    
End of assembler dump.

** Voila! the loader has replace "mov %eax, 0x0" to "mov  %eax,0xb7fdf5f8"
