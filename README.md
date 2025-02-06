# **Welcome to BreezeLang!** 🌬️🌬️

BreezeLang is a programming language inspired by the features I cherish most from the diverse languages I’ve used. It’s designed to blend practicality, simplicity, and elegance, making programming both efficient and enjoyable. It is a simple, interpreted language built with **Flex** and **Bison** in **C**. It supports user-defined functions, local (function-level) scoping, arithmetic operations, loops, conditionals, string manipulation, and basic I/O.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Code Examples](#code-examples)
- [Project Structure](#project-structure)
- [Build and Run](#build-and-run)
- [How It Works](#how-it-works)
  - [Lexical Analysis](#lexical-analysis)
  - [Parsing and AST](#parsing-and-ast)
  - [Evaluation and Function-Level Scoping](#evaluation-and-function-level-scoping)
- [Future Directions](#future-directions)
- [License](#license)

## Overview

This language is designed to be concise and **C-like** in its syntax but offers unique constructs for loops (`w{}`, `f{}`), if-statements (`i{}`, `e{}`, `ie{}`), and function definitions (`d{ myFunc() -> ... }`). Internally, the language uses:

- A **flex-based** scanner to convert source code into tokens.
- A **bison-based** parser that builds an **Abstract Syntax Tree (AST)**.
- A **tree-walk interpreter** that evaluates AST nodes in sequence.
- A **function-level scope** system (implemented with a scope stack), ensuring local variables are preserved independently during function calls and recursion.

## Features

1. **Data Types**:  
   - **Integer**: `x = 5;`  
   - **Float**: `y = 3.14;`  
   - **Boolean**: `b = true` or `false`  
   - **String**: `str = "Hello World"` with support for escape sequences like `\n`, `\t`.

2. **Arithmetic & Expressions**:  
   - `+`, `-`, `*`, `/`, `**` (exponentiation)  
   - Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`  
   - Boolean operators: `&&`, `||`, `!`.

3. **Control Flow**:  
   - **If**: `i{ condition -> ... }`  
   - **Else**: `e{ -> ... }`  
   - **While**: `w{ condition -> ... }`  
   - **For**: `f{ init, condition, update -> ... }`  
   - **Break / Continue** for loops.

4. **Functions**:  
   - Defined with `d{ funcName(param1, param2) -> ... }`.  
   - Local (function-level) scoping: each function call pushes a new scope.  
   - Return values via `return ...;`.

5. **Input/Output**:  
   - **`print`** to output values or strings.  
   - **`what? -> varName;`** reads user input into a string variable.

6. **String Operations**:  
   - Indexing and slicing: `myString[2]`, `myString[2:4]`.  
   - Length function: `len(myString)`.

## Code Examples

Below are a few simple examples demonstrating the language’s syntax.

### 1. Hello World

```c
msg = "Hello, World!";
print msg;  // prints Hello, World!
```

### 2. Basic Arithmetic

```c
x = 10;
y = 3.14;
z = x + y;
print "Result: ", z;  // Result: 13.14
```

### 3. Looping

```c
f{ i=0, i<5, i=i+1 ->
  print "Loop iteration: ", i, "\n;
}
```
This “for” loop has an initialization (`i=0`), condition (`i<5`), and update (`i=i+1`) separated by commas.

### 4. If-Else

```c
i{ x == 10 ->
  print "x is exactly 10!";
e{ ->
  print "x is not 10.";
}
```

### 5. Functions with Local Scope

```c
d{ sum(a, b) ->
  return a + b;
}
print "sum(3,7) = ", sum(3,7);  // = 10
```
Local variables `a` and `b` are confined to the function’s scope and do not overwrite global variables.

### 6. User Input

```c
what? -> username;
print "Your name is ", username;
```

### 7. Slicing and String length check
```c
str = "Hello world!"
slice = str[6 : 11];

// Prints "world!" with length of 6
print "Here's your desired slice: '", slice, "' of length: ", len(slice), "\n";
```

## Project Structure

- **lexer.l**: Lexical analyzer (flex). Defines tokens (keywords, operators, literals).  
- **parser.y**: Grammar (bison). Specifies how tokens form expressions, statements, function definitions, etc. Builds the AST.  
- **ast.c** & **ast.h**: AST structures and evaluation logic.  
- **scope.c** & **scope.h**: Manages function-level scoping with push/pop operations and symbol lookups.  
- **common_lib.h**: Shared includes or utility definitions.  
- **symtab.h**: Definitions for `SymbolNode`, `ValueType`, etc. (No longer storing a single global symbol table—migrated to scope.c).  
- **Makefile**: Builds and manages the entire project.

## Build and Run

1. **Install Dependencies**  
   Ensure you have Flex, Bison, Make, and a C compiler installed on your system (e.g., `gcc` or `clang`).

2. **Compile**  
   Simply run:
   ```bash
   make
   ```
   This should invoke `flex`, `bison`, and the C compiler, producing an executable (`BreezeCompiler`).

3. **Run**  
   ```bash
   ./BreezeCompiler myprogram.bl
   ```
   Here, `myprogram.bl` contains your source code in this language.

4. **Interact**  
   If your program uses the `what? -> var;` statement, it will prompt for user input at runtime.

## How It Works

### 1. Lexical Analysis

- **Flex** reads patterns in `lexer.l` and transforms the input stream into tokens (`INT`, `FLOAT`, `IDENTIFIER`, `PLUS`, etc.).
- Regular expressions in the `.l` file are **type-3** grammars in Chomsky hierarchy.

### 2. Parsing and AST

- **Bison** (`parser.y`) uses a **context-free grammar** to parse tokens into AST nodes.
- Grammar rules for statements, expressions, loops, and function definitions produce corresponding node types (`NODE_STMTS`, `NODE_ASSIGN`, `NODE_FUNC`, etc.).
- The AST represents the entire program in a tree-like data structure.

### 3. Evaluation and Function-Level Scoping

- **AST Evaluation**: A recursive tree walk that executes each node in order.
- **Function Calls**: When a function is invoked, a new scope is pushed. Its parameters and local variables remain isolated until the function returns, at which point the scope is popped. This mechanism supports **recursive** calls properly.

## Future Directions

1. **Block-Level Scoping**: Push/pop scopes for `{}` blocks in loops or if-statements (currently only function-level).  
2. **More Data Structures**: Arrays, dictionaries, or user-defined objects.  
3. **Modularization**: Ability to import external libraries or modules.  
4. **Static Type Checking**: Extend grammar or semantics to detect type errors at compile time.  
5. **Optimization or JIT**: Compile AST to bytecode or native code for efficiency.

## License

This project is open-source under the MIT License. See `LICENSE` for details.

### Thanks for Visiting!
_This has been my final project proposal for the "Compiler Construction" course when studying in Germany, at [OTH REGENSBURG](https://www.oth-regensburg.de/en/). 
At the moment there are no plans to resume work, if work is resumed this README will be duly updated._
