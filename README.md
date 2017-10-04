# developerdenis
A group of C/C++ files I reuse across many different projects. These are all constantly being updated and changed.

### Denis_Types
A collection of typedefs that I like to use to simplify using 8, 16, 32, or 64 bit variables.

### Denis_Math
Contains structures like vectors and rectangles and operations that can be performed on them, as well as other commonly needed math operations.

### Denis_Strings
Basic functions that work on C-style strings, and some for char arrays. Functions like, checking for equality, duplication, and tokenization.

### Denis_Win32
Windows specific functions like opening file dialogs or getting the program's full path that may be useful in many programs.

### LL
Bare-bones linked list implementation.

### Win32_Layer
Fairly simple Windows platform layer for programs. Gives programs a user resizable buffer of pixels representing the screen, a fixed size memory block, andi nput from mouse, keyboard, and pen devices. The main feature is that the layer is an executable file, but the actual application is a DLL that is reloaded whenever it is updated, allowing for live code editing in C similar to a scripted language (inspired by the Handmade Hero project).
