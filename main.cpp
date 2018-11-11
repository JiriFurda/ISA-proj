
#include <iostream>
#include <getopt.h>
#include <unordered_map>
#include <cstring>

#include "feed.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	int opt;
	std::unordered_map<int, int> flags;

	while((opt = getopt(argc, argv, "f:c:C:Tau")) != -1)
	{
		switch(opt)
		{
			case 'T':
			case 'a':
			case 'u':
				flags[opt] = true;
				break;
			case 'f':
			case 'c':
			case 'C':
				flags[opt] = true;
				//flags[opt] = optarg;
				break;
			default: // '?' 
				fprintf(stderr, "Usage: %s <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]\n", argv[0]);
				exit(1);
	   }
	}

	string url;

	if(optind < argc)	// has URL parameter
	{
		if(flags['f'])
		{
			fprintf(stderr, "Error: Cannot combine URL parameter with -f option");
			exit(1);			
		}

		url = argv[optind];
	}

	std::cout << url << "\n";

	Feed feed(url);

	feed.read();


	return 0;
}
