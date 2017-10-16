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
CC_FLAGS = -DNOMAIN ${GNUCFLAGS}

GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}

all : client init_db list_db

menu_scroll2.o: menu_scroll2.c
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c menu_scroll2.c

demo_menus2.o: demo_menus2.c test.priv.h ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h
	${CC}  ${CC_FLAGS} ${INCLUDE_PATHS} -c demo_menus2.c

form_supplement.o: form_supplement.c  test.priv.h ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h form_sup.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c form_supplement.c

demo_forms4.o: demo_forms4.c  test.priv.h ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h form_sup.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c demo_forms4.c

tcp_win.o: tcp_win.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h ../mytypes.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c tcp_win.c

file_win.o: file_win.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h ../mytypes.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c file_win.c

file_menu2.o: file_menu2.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c file_menu2.c

config_file.o: config_file.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h
	${CC} -DCONFIG_FILE ${CC_FLAGS} ${INCLUDE_PATHS} -c config_file.c

init_db.o: init_db.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c init_db.c

list_db.o: list_db.c ../queue/illist_threads_rw.h ../queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c list_db.c

client.o: client.c client.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} -c client.c

illist_threads_rw.o: ../queue/illist_threads_rw.c ../queue/illist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} ${CC_FLAGS} -c ../queue/illist_threads_rw.c

ollist_threads_rw.o: ../queue/ollist_threads_rw.c ../queue/ollist_threads_rw.h
	${CC} ${CC_FLAGS} ${INCLUDE_PATHS} ${CC_FLAGS} -c ../queue/ollist_threads_rw.c

rdwr.o: ../queue/rdwr.c ../queue/rdwr.h
	${CC} ${INCLUDE_PATHS} ${CC_FLAGS} -c ../queue/rdwr.c

client: demo_menus2.o menu_scroll2.o demo_forms4.o form_supplement.o client.o tcp_win.o file_win.o file_menu2.o\
 config_file.o illist_threads_rw.o ollist_threads_rw.o rdwr.o 
	${CC} menu_scroll2.o demo_menus2.o demo_forms4.o form_supplement.o client.o tcp_win.o file_win.o file_menu2.o config_file.o  illist_threads_rw.o ollist_threads_rw.o rdwr.o  -o client -lmenu -lform -lcurses

init_db: config_file.o init_db.o
	${CC} init_db.o config_file.o -o init_db

list_db: config_file.o list_db.o
	${CC} list_db.o config_file.o -o list_db

clean :
	rm -f *.o *~ *# core client init_db list_db
