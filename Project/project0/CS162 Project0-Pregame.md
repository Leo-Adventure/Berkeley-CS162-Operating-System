# CS162 Project0-Pregame

The document of the CS162 Project0 is posted as a link https://cs162.org/static/proj/proj-pregame/, please refer to that document.

## Find the faulting instruction

### Q1 What virtual address did the program try to access from userspace that caused it to crash?

0xc0000008

### Q2 What is the virtual address of the instruction that resulted in the crash?

0x80488ee

### Q3 To investigate, disassemble the `do-nothing` binary using `objdump` (you used this tool in Homework 0). What is the name of the function the program was in when it crashed? Copy the disassembled code for that function onto Gradescope, and identify the instruction at which the program crashed.

### Q4 Find the C code for the function you identified above (hint: it was executed in userspace, so it’s either in `do-nothing.c` or one of the files in `proj0/src/lib` or `proj0/src/lib/user`), and copy it onto Gradescope. For each instruction in the disassembled function in #3, explain in a few words why it’s necessary and/or what it’s trying to do. Hint: see 80x86 Calling Convention

### Q5 Why did the instruction you identified in Q3 try to access memory at the virtual address you identified in Q1? Don’t explain this in terms of the values of registers; we’re looking for a higher level explanation.

## Step through the crash

## Debug

