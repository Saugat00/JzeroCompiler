# Joseph Coston & Saugat Sharma
# CSE423
# HW6
CC= gcc
CFLAGS= -g -Wall
OBJFLAGS= -g -Wall -c

assignment= HW6

primary= tree.o hash.o symt.o termargs.o builtins.o type.o tac.o icode.o
secondary= j0.tab.o lex.yy.o
testdir= ./unitTests/*.java ./jefferyTests4/*.java

all: j0

j0: $(secondary) $(primary) main.c
	$(CC) $(CFLAGS) $^ -o j0

$(primary): %.o: %.c %.h
	$(CC) $(OBJFLAGS) $<


lexer: parser lex.yy.o

lex.yy.o: lex.yy.c j0.tab.h
	$(CC) $(OBJFLAGS) lex.yy.c

lex.yy.c: j0.l
	flex j0.l


parser: j0.tab.o

j0.tab.o: j0.tab.c j0.tab.h
	$(CC) $(OBJFLAGS) j0.tab.c

j0.tab.c: j0.tab.h
j0.tab.h: j0.y
	bison -d j0.y


# run this to display shift/reduce and reduce/reduce errors
parser-debug: j0.y
	bison -v -Wcounterexamples j0.y

clean:
	rm -f j0 *.o ./*/*.ic j0.tab* lex.yy* sharma_coston_$(assignment).zip

package: clean
	zip -r sharma_coston_$(assignment).zip .

testall: $(testdir)

$(testdir): all
	-./j0 $@ -v

