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

#all : client server client2 server
all : make_waypoints show_waypoints

client.o: client.c
	${CC} ${CC_FLAGS} -DTEST_CLIENT ${INCLUDE_PATHS} -c client.c

server.o: server.c
	${CC} ${CC_FLAGS} -DCOPY_FILE ${INCLUDE_PATHS} -c server.c

test_upload_new.o: test_upload_new.c
	${CC} ${CC_FLAGS} -DCOPY_FILE ${INCLUDE_PATHS} -c test_upload_new.c

make_waypoints.o: make_waypoints.c
	${CC} ${CC_FLAGS} -c make_waypoints.c

show_waypoints.o: show_waypoints.c
	${CC} ${CC_FLAGS} -c show_waypoints.c

client: client.o
	${CC} client.o -o client

server: server.o
	${CC} server.o -o server

make_waypoints: make_waypoints.o
	${CC} make_waypoints.o -o make_waypoints

show_waypoints: show_waypoints.o
	${CC} show_waypoints.o -o show_waypoints

clean :
#	rm -f *.o *~ *# core client server client2 server test_write test_read client make_waypoints
#	rm -f *.o *~ *# client2
#	rm -f *.o *~ *# test_upload_new
	rm -f *.o *- *# make_waypoints show_waypoints

