CC = g++

ORACLE_HOME = /opt/oracle/instantclient

HOME = ../../..

INCLUDE = -I$(HOME)/inc

LIB = -L$(HOME)/lib

CFLAGS = -Wall -O -g
LFLAGS = -lisrc -lssl

TARGET = echoclient

SOUCE_FILES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOUCE_FILES))

%.o : %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)

$(TARGET): $(OBJS) 
	$(CC) $^ -o $@ $(LIB) $(LFLAGS)

.PHONY:clean
clean:
	$(RM) $(TARGET) $(OBJS)
