zip -u -r thread.zip sched
zip -u -r thread.zip thread_io/ioports.c
zip -u -r thread.zip thread_io/Makefile
zip -u -r thread.zip thread_io/sched.c
zip -u -r thread.zip thread_io/ioports.h
zip -u -r thread.zip thread_io/tasks.c
zip -u -r thread.zip thread_io/tasks.h
zip -u -r thread.zip thread_io/lcd_func.c
zip -u -r thread.zip thread_io/lcd_func.h
zip -u -r thread.zip thread_io/queue/rdwr.h
zip -u -r thread.zip thread_io/queue/rdwr.c
zip -u -r thread.zip thread_io/queue/illist_threads_rw.h
zip -u -r thread.zip thread_io/queue/illist_threads_rw.c
zip -u -r thread.zip thread_io/queue/ollist_threads_rw.h
zip -u -r thread.zip thread_io/queue/ollist_threads_rw.c
zip -u -r thread.zip thread_io/queue/rt_llist_threads_rw.h
zip -u -r thread.zip thread_io/queue/rt_llist_threads_rw.c
zip -u -r thread.zip thread_io/ncurses/client.c
zip -u -r thread.zip thread_io/ncurses/client.h
zip -u -r thread.zip thread_io/serial_io.c
zip -u -r thread.zip thread_io/serial_io.h
zip -u -r thread.zip thread_io/idata.csv
zip -u -r thread.zip thread_io/odata.csv
zip -u -r thread.zip thread_io/ncurses/demo_menus2.c
zip -u -r thread.zip thread_io/ncurses/menu_scroll2.c
zip -u -r thread.zip thread_io/ncurses/menu_scroll3.c
zip -u -r thread.zip thread_io/ncurses/Makefile
zip -u -r thread.zip thread_io/ncurses/init_db.c
zip -u -r thread.zip thread_io/ncurses/list_db.c
zip -u -r thread.zip thread_io/ncurses/conv_db.c
zip -u -r thread.zip thread_io/ncurses/setiodata.h
zip -u -r thread.zip thread_io/ncurses/tcp_win.c
zip -u -r thread.zip thread_io/ncurses/tcp_win2.c
zip -u -r thread.zip thread_io/ncurses/tcp_win_show.c
zip -u -r thread.zip thread_io/ncurses/config_file.c
zip -u -r thread.zip thread_io/ncurses/config_file.h
zip -u -r thread.zip thread_io/ncurses/file_win.c
zip -u -r thread.zip thread_io/ncurses/file_menu2.c
zip -u -r thread.zip thread_io/ncurses/demo_forms4.c
zip -u -r thread.zip thread_io/ncurses/form_supplement.c
zip -u -r thread.zip thread_io/ncurses/form_sup.h
zip -u -r thread.zip thread_io/ncurses/forms.sh
zip -u -r thread.zip thread_io/ncurses/test.priv.h
zip -u -r thread.zip thread_io/ncurses/text_entry.c
zip -u -r thread.zip thread_io/ncurses/text_entry.h
zip -u -r thread.zip thread_io/ncurses/ncurses_cfg.h
zip -u -r thread.zip thread_io/thread_io_notes.txt
zip -u -r thread.zip mytypes.h
unzip -l thread.zip
cp thread.zip /media/dan/USB_DISK/thread.zip
umount /media/dan/USB_DISK
echo "USB_DISK unmounted"
echo ""
