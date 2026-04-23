# amolang
An experimental programming language started as a hobby project to learn about compiler design.

## About the Language
### An Introduction
- Origin: probably will write later
- Backstory: i don't know
- Naming: Amolang = Amogus + Language (quite frankly, I'm pretty unserious when it comes to naming hobby projects, and amogus just so happened to be the meme I remembered at that time)

### Features
- Familiar syntax: heavily inspired from C/C++, with all the braces and semicolons you could ever love
- Preprocessor: easily include code from other files or define constants with C-style `#include` and `#define` directives
- Static typing: explicit typing of variable and return types of functions

### Syntax
The entry point to all Amolang programs is the `int main()` function

Directives
- `#include`: include code from another file
- `#define`: define a constant
```
#include "fib.amo"
#define SIZE_BUFFER 15
```

Variables
```
int i = 15;
string s = "this is a test";
```

Functions
```
int foo() {
    printf(3);
    return 3;
}

void bar() {
    printf("hello");
}
```

Comments
```
// random comment goes here
```

Complete sample code
```
#include "other.amo"
#include "other2.amo"
#define TEST 15

int main() {
    int a = 16;
    int b = TEST - 1;
    printf("this is a test");
    return 0;
}
```
### Toolchain
- `amocc`: Amolang Cross Compiler (in development)
- `amo2cpp`: Amolang-to-C++ translator (in development)
- `amold`: Amolang Linker (planned)
- `amolint`: Amolang Linter (planned)

## Getting Started
Prerequisities:
- GNU C Compiler (developed using GCC 15.2)
- GNU Make

Build instructions
- Build with `make`

## Contributing
No guidelines on how to contribute at this point in time, will update later.

## Roadmap (subject to change)
### Language features 
- [ ] Initial
    - [ ] Primitive data types (int, float)
    - [ ] Mathematical operators
    - [ ] Bitwise operators
    - [ ] Expression parsing
- [ ] State and Memory
    - [ ] Variable declarations
    - [ ] Variable assignments
    - [ ] Scoping
- [ ] Logic and Branching
    - [ ] Comparison operators
    - [ ] Logical operators
    - [ ] If / else statements
- [ ] Iteration
    - [ ] For / while loops
    - [ ] Control transfer (break / continue)
- [ ] Modularity
    - [ ] Function declarations + Arguments
    - [ ] Function calls + Parameters
    - [ ] Return statements
- [ ] Extension 1
    - [ ] Arrays
    - [ ] Structs
- [ ] Standard Library
    - [ ] Standard I/O functions (`printf()`, `scanf()`, etc.)
    - [ ] File I/O functions (`open()`, `fprintf()`, `fscanf()`, etc.)
    - [ ] Filesystem operations (`move()`, `copy()`, etc.)
    - [ ] System functions (`rand()`, `time()`, etc.)
- [ ] Extension 2
    - [ ] Error handling
    - [ ] Memory management

### Amolang Cross Compiler
1. Front-end
- [ ] Preprocessing
- [ ] Lexical analysis
- [ ] Parsing
2. Middle-end
- [ ] Symbol table
- [ ] Type checking
- [ ] AST resolution
3. Back-end
- [ ] LLVM IR code generation

## Special Thanks
These videos inspired me to tackle this project, even if some of them falls under different categories from compiler design:
- [How do computers read code? - Frame of Essence (YouTube)](https://www.youtube.com/watch?v=QXjU9qTsYCc)
- [phi - wzid (GitHub)](https://github.com/wzid/phi)
- [I made an entire OS that only runs Tetris - jdh (YouTube)](https://www.youtube.com/watch?v=FaILnmUYS_U)
- [I designed my own 8-bit computer just to play PONG - jdh (YouTube)](https://www.youtube.com/watch?v=7A1SzIIKMho)
- [I made a C compiler in C - lolzdev (YouTube)](https://www.youtube.com/watch?v=UW8LgC-S_co)

## Additional Documentation
1. [Amolang Cross Compiler](./docs/amocc.md)
2. [Amolang-to-C++ translator](./docs/amo2cpp.md)
3. [The Amolang Programming Language](./docs/language.md)
