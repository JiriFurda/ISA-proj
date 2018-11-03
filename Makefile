LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

SRCS = main.cpp feed.cpp

all:
	c++ -std=gnu++11 -o feedreader $(SRCS) $(LDFLAGS) $(LDLIBS)
