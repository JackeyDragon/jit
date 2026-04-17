# JIT Language Specification

## Overview

JIT is a simple interpreted programming language with support for variables, functions, control flow, and basic arithmetic and logical operations.

## Capabilities

### Data Types
- `int` - Integer type (32-bit)
- `float` - Floating point type

### Literals
- Integer literals: `42`, `0`, `-10`
- Float literals: `3.14`, `0.5`, `-2.71`

### Operators

**Arithmetic:**
- `+` Addition
- `-` Subtraction
- `*` Multiplication
- `/` Division

**Logical:**
- `&&` Logical AND
- `||` Logical OR

**Comparison:**
- `==` Equal
- `!=` Not equal

### Statements

1. **Variable Declaration**
   ```jit
   int x = 5;
   float pi = 3.14;
   ```

2. **Variable Assignment**
   ```jit
   x = 10;
   ```

3. **If Statement**
   ```jit
   if (condition) 
   ```

  only the next statement is being executed.

4. **Goto (with labels)**
   ```jit
   label_name:
   goto label_name;
   ```

5. **Function Declaration**
   ```jit
   int add(int a, int b) {
     return a + b;
   }
   ```

6. **Return Statement**
   ```jit
   return value;
   ```

### Special Commands
- `list` - Print all stored statements

---

## Grammar

### Program Structure

```
program        → statement*
```

### Statements

```
statement      → declaration
               | assignment
               | if_statement
               | function_declaration
               | goto_statement
               | label
               | expression
               | block

block          → "{" statement* "}"
```

### Declarations

```
declaration    → type identifier "=" expression ";"
type           → "int" | "float"
```

### Assignments

```
assignment     → identifier "=" expression ";"
```

### If Statements

```
if_statement   → "if" "(" expression ")" statement
```

Note: Currently no `else` clause support.

### Function Declarations

```
function_declaration → type identifier "(" parameters? ")" block
parameters     → parameter ("," parameter)*
parameter      → type identifier
```

### Goto and Labels

```
goto_statement → "goto" identifier ";"
label          → identifier ":"
```

### Expressions

Using precedence climbing (binding power):

```
expression     → logical_or
logical_or     → logical_and (("||") logical_and)*
logical_and    → equality (("&&") equality)*
equality       → comparison (("==" | "!=") comparison)*
comparison     → term (("+" | "-") term)*
term           → factor (("*" | "/") factor)*
factor         → primary
primary        → INTEGER | FLOAT | identifier | "(" expression ")"
```

### Operator Precedence (lowest to highest)

| Precedence | Operators       |
|------------|-----------------|
| 1          | `||`            |
| 2          | `&&`            |
| 3          | `==`, `!=`      |
| 4          | `+`, `-`        |
| 5          | `*`, `/`        |

---

## Type System

- Static typing with `int` and `float`
- Type checking enforced at compile-time (during parsing/execution)
- Cannot mix `int` and `float` in expressions
- Cannot assign wrong type to variable

---

## Implementation Details

### Modules

- **lexer/tokenizer** (`lexer.c`, `tokenizer.h`): Tokenizes input string into tokens
- **parser** (`parser.c`): Builds AST from tokens using precedence climbing
- **ast** (`ast.h`, `ast.c`): AST node definitions
- **symbol table** (`symbol_table.c`, `symbol_table.h`): Manages variables and functions
- **emiter** (`emiter.c`): Executes AST nodes (interpreter)
- **code** (`code.c`, `code.h`): Stores executed statements for listing

### Execution Model

1. Lexer tokenizes input
2. Parser builds AST
3. Emiter executes AST nodes
4. Results stored in symbol table
