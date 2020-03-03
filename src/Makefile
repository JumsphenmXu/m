objs=util.o lex.o program.o var_stmt.o \
	 return_stmt.o expr.o parser.o main.o \
	 output.o stmt.o literal.o block_stmt.o \
	 if_stmt.o for_stmt.o func.o token_list.o \
	 expr_list.o call_expr.o index_expr.o \
	 array_expr.o map_expr.o iobject.o hash.o \
	 builtin.o treewalk_interpreter.o

util.o: util/util.c include/m.h
	gcc -c util/util.c -o util.o

hash.o: util/hash.c include/m.h
	gcc -c util/hash.c -o hash.o

lex.o: parser/lex.c include/m.h
	gcc -c parser/lex.c -o lex.o

program.o: parser/program.c include/m.h
	gcc -c parser/program.c -o program.o

var_stmt.o: parser/var_stmt.c include/m.h
	gcc -c parser/var_stmt.c -o var_stmt.o

return_stmt.o: parser/return_stmt.c include/m.h
	gcc -c parser/return_stmt.c -o return_stmt.o

expr.o: parser/expr.c include/m.h
	gcc -c parser/expr.c -o expr.o

array_expr.o: parser/array_expr.c include/m.h
	gcc -c parser/array_expr.c -o array_expr.o

map_expr.o: parser/map_expr.c include/m.h
	gcc -c parser/map_expr.c -o map_expr.o

parser.o: parser/parser.c include/m.h
	gcc -c parser/parser.c -o parser.o

output.o: parser/output.c include/m.h
	gcc -c parser/output.c -o output.o

stmt.o: parser/stmt.c include/m.h
	gcc -c parser/stmt.c -o stmt.o

block_stmt.o: parser/block_stmt.c include/m.h
	gcc -c parser/block_stmt.c -o block_stmt.o

if_stmt.o: parser/if_stmt.c include/m.h
	gcc -c parser/if_stmt.c -o if_stmt.o

for_stmt.o: parser/for_stmt.c include/m.h
	gcc -c parser/for_stmt.c -o for_stmt.o

literal.o: parser/literal.c include/m.h
	gcc -c parser/literal.c -o literal.o

token_list.o: parser/token_list.c include/m.h
	gcc -c parser/token_list.c -o token_list.o

expr_list.o: parser/expr_list.c include/m.h
	gcc -c parser/expr_list.c -o expr_list.o

call_expr.o: parser/call_expr.c include/m.h
	gcc -c parser/call_expr.c -o call_expr.o

index_expr.o: parser/index_expr.c include/m.h
	gcc -c parser/index_expr.c -o index_expr.o

func.o: parser/func.c include/m.h
	gcc -c parser/func.c -o func.o

iobject.o: objects/iobject.c include/m.h
	gcc -c objects/iobject.c -o iobject.o

builtin.o: interpreter/builtin.c include/m.h
	gcc -c interpreter/builtin.c -o builtin.o

treewalk_interpreter.o: interpreter/treewalk_interpreter.c include/m.h
	gcc -c interpreter/treewalk_interpreter.c -o treewalk_interpreter.o

main.o: main.c include/parser.h
	gcc -c main.c -o main.o

.PHONY: main
main: $(objs)
	gcc -o main $(objs)

.PHONY: clean
clean:
	rm *.o
