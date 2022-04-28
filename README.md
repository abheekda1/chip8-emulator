# CHIP-8 Emulator
A simple CHIP-8 emulator in C++ using ncurses.  
> Note: this is barely functional and probably technically not functional at all. Lots of improvements to be made, with sound, input, and many opcodes to be implemented.

## Building  
This can easily be built with CMake as long as you have ncurses installed:
```
cmake -B build
cmake --build build
```  
It will then be found as a binary called `c8` in the `build/` directory.

## TODO
There is a lot of stuff to be done, so here's just some stuff that works and what needs to be done:
### Opcodes
For now just check the big switch statement in `src/chip8.cpp` but there are some opcodes to be added, fixed, or completely redone. For example, at the moment the `V[0]` register isn't touched when it should be modified for `0x8XYN`.
### Input
- [x] Single-key input
- [ ] Multiple-key input (can probably be done with a map containing a key with the key code pointing to a boolean)
### Sound
- [ ] Implement the `beep()` function properly