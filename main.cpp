
#include <iostream>
#include <getopt.h>
#include <unordered_map>
#include <cstring>
#include  "openssl/bio.h"
#include  "openssl/ssl.h"
#include  "openssl/err.h"

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

		string url = argv[optind];
	}
	
	
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	
	BIO * bio;

	bio = BIO_new_connect((url + string(":80")).c_str());
	if(bio == NULL)
	{
		fprintf(stderr, "Error: Couldn't create connection");
		exit(1);		
	}



	if(BIO_do_connect(bio) <= 0)
	{
		fprintf(stderr, "Error: Couldn't connect");
		exit(1);		
	}


	char buffer[1024];	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer

	printf("Started reading\n");

        char *write_buf = "GET / HTTP/1.1\r\nHost: www.nemesisit.ro\r\n\r\n";

        if(BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
        {
                if(! BIO_should_retry(bio))
                {
                        /* Handle failed write here */
                }
            /* Do something to handle the retry */
                std::cout << "Failed write" << std::endl;
        }

        //char buf[MAX_PACKET_SIZE];

        int p;
        char r[1024];
        for(;;)
        {
                p = BIO_read(bio, r, 1023);
                if(p <= 0) break;
                r[p] = 0;
                printf("%s", r);
        }

        std::cout << "Done reading" << std::endl;

        /* To free it from memory, use this line */

        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);

	BIO_free_all(bio);

	return 0;
}
