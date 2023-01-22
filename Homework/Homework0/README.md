# CS162 Homework0

The document of the CS162 Homework0 is posted as a link https://cs162.org/static/hw/hw-intro/, please refer to that document.

## Counting Words

Shown in `hw_intro/words`

## User Limits

Shown in `hw_intro/limits.c`

## GDB Basics

### Q1.1 What memory address does argv store?

![pic1.png](https://s2.loli.net/2023/01/22/o79zpsajIPCmNrK.png)

### Q1.2 Describe what’s located at that memory address. (What does argv point to?)

The argv points to the arguments that were passed to the main function, indicating the Command-Line Arguments.

### Q1.3 What is the memory address of the recur function?

1 Point

(gdb) display recur

1: recur = {int (int)} 0x5555555546cd <recur>

### Q1.4 What values are in all the registers?

![pic2.png](https://s2.loli.net/2023/01/22/aRC3jLlumqKU6o9.png)

### Q1.5 Which instructions correspond to the return 0 in C?

mov    $0x0,%eax

## From Source Code to Executable

### Q2.1 Generate recurse.S and find which instruction(s) corresponds to the recursive call of recur(i - 1)

![pic3.png](https://s2.loli.net/2023/01/22/mqzpUOscNfEhiXD.png)

### Q2.2 What do the .text and .data sections contain?

The .text section contains the executable instructions. And the .data section contains the declarations of initialized data and constants.

### Q2.3 What command do we use to view the symbols in an ELF file? (Hint: We can use objdump again, look at “man objdump” to find the right flag).

`objdump -t map.o`

### Q2.4 What do the g, O, F, and *UND* flags mean?

g means that it is global. O means that it is the name of an object. F means that it is the name of a function. \*UND\* means the section is referenced in the file being dumped, but not defined in this file.

### Q2.5 Where else can we find a symbol for recur? Which file is this in? Copy and paste the relevant portion of the symbol table.

![pic4.png](https://s2.loli.net/2023/01/22/O25jGpEYfyFrDtl.png)

### Q2.6 Examine the symbol table of the entire map program now. What has changed?

Its size is longer than the sum of the symbol tables of map.o and recurse.o.

### Q2.7 What segment(s)/section(s) contains recur (the function)? (The address of recur in objdump will not be exactly the same as what you saw in gdb. An optional stretch exercise is to think about why. See the Wikipedia article on relocation for a hint.)

The .text section.

### Q2.8 What segment(s)/section(s) contains global variables? Hint: look for the variables foo and stuff.

The .bss and .data sections. 

### Q2.9 Do you see the stack segment anywhere? What about the heap? Explain.

Both are no. Because the dynamic variables are created from initialization, not written in the segment. What's more, the stack and heap segments do not exist in ELF actually, the main parts of an ELF file are .bss, .text, .data, and .rodata, not including the stack or heap segment.

### Q2.10 Based on the output of map, in which direction does the stack grow? Explain.

![pic5.png](https://s2.loli.net/2023/01/22/uLnhvFatAjrGXHP.png)