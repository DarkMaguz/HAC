bin_PROGRAMS = hacd
hacd_SOURCES = main.cpp hacd.cpp Server.cpp User.cpp
hacd_CPPFLAGS = -I"/usr/include/mysql++" -I"/usr/include/mysql" -I"../misc"
#$(MYSQLPP_CFLAGS) -I"../misc"
hacd_LDADD = ../misc/libhac.la -L/usr/lib/x86_64-linux-gnu -lmariadbclient -lpthread -lz -lm -ldl -lmysqlpp -lmysqlcppconn
#-lmysqlcppconn $(MYSQLPP_LIBS) $(PTHREAD_LIBS)
