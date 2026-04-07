# Initial goals
Define variable
Print variable
Assign variable some value

# Grama

S -> DECLARE | ASSIGN
DECLARE -> KEYWORD_VAR IDENTIFIER TOKEN_ASSIGN NUMBER SEMICOLON
ASSIGN -> IDENTIFIER TOKEN_ASSIGN NUMBER
