# gcc hbg*.c -o hbg
# gcc -DUSE_DICE -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -o engine engine.c
gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine0.c
gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine1.c
gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine2.c
gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine3.c
gcc -Wall -Wstrict-prototypes -ansi -pedantic -o engine engine.c engine0.o engine1.o engine2.o engine3.o
