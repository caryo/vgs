#! /bin/sh
BUILD_FLAGS=${BUILD_FLAGS:-}

gcc ${BUILD_FLAGS} -Wall -Wstrict-prototypes -ansi -pedantic -c engine0.c
gcc ${BUILD_FLAGS} -Wall -Wstrict-prototypes -ansi -pedantic -c engine1.c
gcc ${BUILD_FLAGS} -Wall -Wstrict-prototypes -ansi -pedantic -c engine2.c
gcc ${BUILD_FLAGS} -Wall -Wstrict-prototypes -ansi -pedantic -o engine engine.c engine0.o engine1.o engine2.o
