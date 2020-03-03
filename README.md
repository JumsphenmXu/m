# Project

A language parser & tree-walking interpreter

# Language gramma

```
program: stmt*

stmt: expr_stmt | var_stmt | return_stmt | if_stmt | for_stmt | block_stmt
stmt_trailer: ';' | '\n'
expr_stmt: expr stmt_trailer
var_stmt: 'var' expr '=' expr stmt_trailer
return_stmt: 'return' expr stmt_trailer
if_stmt: 'if' '(' expr ')' block_stmt ['elif' '(' expr ')' block_stmt]* ['else' block_stmt]?
block_stmt: '{' stmt* '}' stmt_trailer
for_stmt: 'for' '(' [expr? [ ',' expr]*] ';' [expr] ';' [expr? [',' expr]*] ')' block_stmt

expr: ident_expr | literal_expr | unary_expr | binary_expr | group_expr |
      array_expr | map_expr | index_expr | func_expr | call_expr

ident_expr: [_a-zA-Z][_a-zA-Z0-9]*
literal_expr: char | bool | int | float | string
bool: 'true' | 'false'
char: \u0-\u127
int: 0 | [1-9][0-9]* | 0x[0-9a-fA-F]
float: [0 | [1-9][0-9]* | 0x[0-9a-fA-F] ] '.' [e|E [+ | -]] [0-9]*
string: "[\u0-\u127]*"

unary_expr: [uop] expr
binary_expr: expr bop expr
group_expr: '(' expr ')'
func_expr: 'func' '(' expr? [',' expr]* ')' block_stmt
index_expr: expr '[' expr [':'] [expr] ']'
call_expr: expr '(' expr? [',' expr]* ')'
array_expr: '[' expr ',' expr ',' expr ',' ... ']'
map_expr: '{' [expr ':' expr]? [',' expr ':' expr]* '}'

uop: '+' | '-' | '!' | '~'
bop: '+' | '+=' | '-' | '-=' | '&' | '&=' | '^' | '^=' | '|' | '|=' | '*' | '*=' |
    '/' | '/=' | '%' | '%=' | '=' | '==' | '>' | '>=' | '<' | '<=' | '!=' | '&&' |
    '||' | '<<' | '<<=' | '>>' | '>>='

```

# Keywords
``` 
var, if, elif, else, func, return, true, false, nil
```


# Builtin functions
`**` print `**` print object value
`**` type `**` calculate object type
`**` len `**` calculate array/map object length

# Sample

```
var fn = func(a, b, c) {
    var d = a + b * c;
    if (d < a) {
        d = a;
    } elif (d < b) {
        d = b;
    } else {
        d = c;
    }

    return d;
}

print(fn(1, 2, 3)); # eval to 3

# use function as parameters
var fn2 = func(a, b, c) {
    var x = a + b;
    x = c(x, b, 1);
    return x;
}
fn2(1, 2, fn);

var sum = func(n) {
    var result = 0;
    var i = 0;
    for (i = 0; i < n; i += 1) {
        result += i;
    }
    return result;
}
print(sum(100));

# array and map operation
var arr = [1, 'a', 'b', "a string here"];
arr[0] = {'a': "char", 1: "int"};
print(a[0]['a']); # eval to "char"
arr[0]["new key"] = "new val";

```
