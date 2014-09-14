AM_CXXFLAGS =

lib_LIBRARIES = libisrc.a

libisrc_a_SOURCES = AutoMachine.cpp ConfigReader.cpp Exception.cpp Log.cpp Miconv.cpp \
					./IPC/MessageQueue.cpp ./IPC/Mutex.cpp ./IPC/Semaphore.cpp \
					./Net/Socket.cpp ./Net/TcpClient.cpp ./Net/TcpListener.cpp

if ENABLE_SSL
AM_CXXFLAGS += -D__ENABLE_NET_SSL__
libisrc_a_SOURCES += ./Net/SslClient.cpp 
endif
