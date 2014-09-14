CC = g++

ORACLE_HOME = /opt/oracle/instantclient

INCLUDE = -I../../inc -I$(ORACLE_HOME)/sdk/include

LIB = -L$(ORACLE_HOME) -L../../lib

CFLAGS = -Wall -O -DDAEMON -g
LFLAGS = -L/usr/lib/mysql -lmysqlclient -lpthread -lpcap -lisrc -lclntsh -ldl -lm -lz

TARGET = a.out

SOUCE_FILES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOUCE_FILES))

%.o : %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)

$(TARGET): $(OBJS) ../../lib/libisrc.a
#make -C ../../
	$(CC) $^ -o $@ $(LIB) $(LFLAGS)

.PHONY:clean
clean:
	$(RM) $(TARGET) $(OBJS)
install:
	cp -rf $(TARGET) /usr/local/bin/
	cp -rf inms.conf /etc/inms/
