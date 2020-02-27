# Project

A language parser & tree-walking interpreter

# Language gramma

```
expr: ident_expr
expr: literal_expr
expr: unary_expr
expr: binary_expr
expr: group_expr
expr: array_expr
expr: map_expr
expr: index_expr
expr: func_expr
expr: call_expr

ident_expr: [_a-zA-Z][_a-zA-Z0-9]*

literal_expr: bool
literal_expr: int
literal_expr: float
literal_expr: char
literal_expr: string
// e.g. 'true', 'false', '1', '1.01','\'c\'', '"string"''

unary_expr: [op] expr

binary_expr: expr op expr

group_expr: '(' expr ')'

func_expr: 'func' '(' ident_expr? [',' ident_expr]* ')' block_stmt

index_expr: expr '[' expr [':'] [expr] ']'
// e.g. array index [1, 2, 3, 4][2], map index {"a": "b", "c": 1}["a"]

call_expr: expr '(' expr? [',' expr]* ')'

array_expr: '[' expr ',' expr ',' expr ',' ... ']'

map_expr: '{' [expr ':' expr]? [',' expr ':' expr]* '}'


stmt_trailer: ';'
stmt_trailer: '\n'

stmt: expr_stmt
stmt: var_stmt
stmt: return_stmt
stmt: if_stmt
stmt: for_stmt
stmt: block_stmt

expr_stmt: expr stmt_trailer

var_stmt: 'var' expr '=' expr stmt_trailer

return_stmt: 'return' expr stmt_trailer

if_stmt: 'if' '(' expr ')' block_stmt ['elif' '(' expr ')' block_stmt]* ['else' block_stmt]?

block_stmt: '{' stmt* '}' stmt_trailer

for_stmt: 'for' '(' [expr_list]; [expr]; [expr_list] ')' block_stmt
```
