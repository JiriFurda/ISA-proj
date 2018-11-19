LDFLAGS = -L/usr/local/ssl/lib -I/usr/include/libxml2/
LDLIBS = -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`

SRCS = src/main.cpp src/feed.cpp src/parser.cpp src/entry.cpp src/program.cpp

all:
	c++ -std=gnu++11 -o feedreader $(SRCS) $(LDFLAGS) $(LDLIBS)
