LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

all:
	c++ -std=gnu++11 -o feedreader main.cpp $(LDFLAGS) $(LDLIBS)
