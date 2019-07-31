###########################################################
# makefile for myserver
###########################################################
# list/makefile
#
# makefile for the example programs 
#
INCLUDE_PATHS = -I.
# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAGS = -non_shared
T64LDFLAGS_T = ${T64LDFLAGS} -pthread

# GCC/Linux compile flags
GNUCC = gcc
#GNUCFLAGS = -g -ansi -Wstrict-prototypes
GNUCFLAGS = -static
GNULDFLAGS = -static
CC_FLAGS = ${GNUCFLAGS}

GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}

#all : client server client2 server2
#all : client2
all : server

client.o: client.c
	${CC} ${CC_FLAGS} -DTEST_CLIENT ${INCLUDE_PATHS} -c client.c

server.o: server.c
#	${CC} ${CC_FLAGS} -DTEST_SEQUENCE ${INCLUDE_PATHS} -c server.c
	${CC} ${CC_FLAGS} -DSIMPLE_TEST ${INCLUDE_PATHS} -c server.c

test_upload_new.o: test_upload_new.c
	${CC} ${CC_FLAGS} -DCOPY_FILE ${INCLUDE_PATHS} -c test_upload_new.c

client: client.o
	${CC} client.o -o client

server2: server.o
	${CC} server.o -o server

client3.o: client.c
	${CC} ${CC_FLAGS} -DTEST_CLIENT ${INCLUDE_PATHS} -c client.c

server2.o: server.c
	${CC} ${CC_FLAGS} -DCOPY_FILE ${INCLUDE_PATHS} -c server.c

client3: client3.o
	${CC} client.o -o client2

server2: server.o
	${CC} server.o -o server2

test_upload_new: test_upload_new.o
	${CC} test_upload_new.o -o test_upload_new

clean :
	rm -f *.o *~ *# core client server client2 server2 test_write test_read client
#	rm -f *.o *~ *# client2
#	rm -f *.o *~ *# test_upload_new

