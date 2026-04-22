# Language Documentation

## Overview

This is a line-by-line interpreter with expression evaluation. When you enter code, it's stored and executed sequentially. The REPL provides a `list` command to view stored code.

## Variables

### Declaration

```c
int x = 5;
float y = 3.14;
```

Variables must be declared with an initializer:

```c
int x = 5;      // must have = value
float y = 3.14; // must have = value
```

### Array Declaration

```c
int x[3] = {1, 2, 3};     // array of 3 ints
float y[2] = {1.0, 2.0};  // array of 2 floats
```

Once declared, a variable's type cannot change. You can only assign values of the same type.

Note: Variables must always be initialized:
```c
int x = 5;      // must have = value
float y = 3.14; // must have = value
int z[3] = {1, 2, 3}; // must have = {...} values
```

### Assignment

```c
x = 10;
y = 2.71;
```

Variables must be declared with an initializer:

```c
int x = 5;      // must have = value
float y = 3.14; // must have = value
```

Variables must be declared before use.

## Expressions

### Arithmetic Operators

```c
+   // addition
-   // subtraction
*   // multiplication
/   // division
```

Examples:
```c
int x = 5 + 3;    // x = 8
int y = 10 - 4;    // y = 6
int z = 3 * 4;     // z = 12
int w = 15 / 3;     // w = 5
```

### Logical Operators

```c
&&  // logical AND
||  // logical OR
```

Examples:
```c
int x = 1 && 1;    // x = 1 (true)
int y = 1 && 0;    // y = 0 (false)
int z = 0 || 1;    // z = 1 (true)
```

### Equality Operators

```c
==  // equal
!=  // not equal
```

Examples:
```c
int x = 5 == 5;     // x = 1 (true)
int y = 5 != 3;     // y = 1 (true)
int z = 2 == 5;     // z = 0 (false)
```

### Operator Precedence (highest to lowest)

1. Parentheses: `(expr)`
2. Multiplicative: `* /`
3. Additive: `+ -`
4. Equality: `== !=`
5. Logical AND: `&&`
6. Logical OR: `||`

## If Statements

### Syntax

```c
if(expr) statement;
```

The key insight: the `statement` after the `if(expr)` IS the body. It's not a block. The interpreter uses a skip-ahead mechanism:

**When condition is TRUE:**
1. Condition evaluates to non-zero
2. Skip mechanism does NOT advance (condition was true)
3. Main loop advances to next statement → executes the if_body ✓

**When condition is FALSE:**
1. Condition evaluates to zero
2. Skip mechanism advances past the if_body
3. Main loop advances again → if_body is skipped ✓

Examples:
```c
int x = 1;
if(x == 1) int y = 2;      // y = 2 (condition true, body executed)

int a = 0;
if(a == 1) int b = 2;     // b not declared (condition false, body skipped)
```

Note: The if_body is always the single statement immediately following the if.

## Goto and Labels

### Syntax

```c
label_name: statement;        // label definition
goto label_name;            // goto statement
```

Labels must be defined before goto use, and there must be a statement after the label.

Example:
```c
int x = 0;
loop:
x = x + 1;
if(x < 5) goto loop;
```

## Functions

### Function Declaration

```c
int add(int a, int b) {
    return a + b;
}
```

Syntax: `return_type function_name(param1_type param1, ...) { body }`

- Return type: `int`, `float`, or `void`
- Parameters must have type and name
- Use `return expression;` to return a value

### Function Call

```c
int result = add(1, 2);
```

Arguments are evaluated left-to-right and must match parameter types.

### Functions with No Parameters

```c
int getFive() {
    return 5;
}
```

### Functions with No Return Value

```c
void printHello() {
    // no return statement needed
}
```

---

# Grammar Reference

## Statements

```
S → DECLARE
  | ASSIGN
  | IF_CONDITION
  | GOTO
  | LABEL
  | FUNCTION_DECLARATION
  | FUNCTION_CALL
```

## Variable Declaration

```
DECLARE → TYPE IDENTIFIER "=" EXPRESSION ";"

TYPE  → "int" | "float"
```

## Assignment

```
ASSIGN → IDENTIFIER "=" EXPRESSION ";"
```

## If Statement

```
IF_CONDITION → "if" "(" EXPRESSION ")" STATEMENT
```

## Goto Statement

```
GOTO → "goto" IDENTIFIER ";"

LABEL → IDENTIFIER ":"
```

## Function Declaration

```
FUNCTION_DECLARATION → TYPE IDENTIFIER "(" PARAMETERS ")" BLOCK

PARAMETERS      → PARAMETER_LIST | void
PARAMETER_LIST  → PARAMETER ("," PARAMETER)*
PARAMETER     → TYPE IDENTIFIER
```

## Expressions

```
EXPR        → LOGIC_OR

LOGIC_OR    → LOGIC_AND ( "||" LOGIC_AND )*
LOGIC_AND   → EQUALITY ( "&&" EQUALITY )*
EQUALITY    → ADD ( ("==" | "!=") ADD )*

ADD         → MUL ( ("+" | "-") MUL )*
MUL         → PRIMARY ( ("*" | "/") PRIMARY )*

PRIMARY     → NUMBER
            | IDENTIFIER
            | TRUE
            | FALSE
            | "(" EXPR ")"
```

## Tokens

```
IDENTIFIER    → [a-zA-Z_][a-zA-Z0-9_]*
NUMBER     → [0-9]+ ("." [0-9]+)?
TRUE       → "true"
FALSE      → "false"
INT        → "int"
FLOAT      → "float"
VOID       → "void"
IF         → "if"
GOTO       → "goto"
RETURN     → "return"
```

---

# REPL Commands

- `list` - View all stored code
- Empty line or Ctrl+D - Exit REPL

# Type System

- **int**: 32-bit signed integer
- **float**: 32-bit floating point
- **void**: No return type (functions only)
- **error**: Type error indicator

Type mixing is not allowed (e.g., cannot add int and float).