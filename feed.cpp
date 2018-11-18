#include "feed.hpp"

Feed::Feed(Program *program, string url)
{
	this->program = program;

	this->determinePort(url);
	this->determineHost(url);
	this->determinePath(url);
}

bool Feed::determinePort(string url)
{
	if(regex_match(url, regex("^http://.*$")))
		this->port = 80;
	else if(regex_match(url, regex("^https://.*$")))
		this->port = 443;
	else
		return false;

	return true;
}

bool Feed::determineHost(string url)
{
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)([^/]+)")))
	{
        if(matches.size() == 2)
        	this->host = matches[1];
    	else
    		return false;
	}
	else
		return false;

	return true;
}

bool Feed::determinePath(string url)
{
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)[^/]*/(.+)")))
	{
        if(matches.size() == 2)
        	this->path = matches[1];
    	else
    	{
    		cerr << "aaa";
    		return false;
    	}
	}

	return true;
}

void Feed::dumpInfo()
{
	cout << this->host << "\n";
	cout << this->port << "\n";
	cout << this->path << "\n";
}

void Feed::read()
{
	BIO *bio;

	this->connectHost(&bio);
	this->sendRequest(&bio);
	string response = this->readResponse(&bio);
	string content = this->discardHeader(response);

    ERR_print_errors_fp(stderr);
    BIO_free_all(bio);

    this->parse(content, this->program->flags);
}


void Feed::connectHost(BIO **bio)
{
	if(this->port == 80)
	{
		this->connectHttpHost(bio);
	}
	else if(this->port == 443)
	{
		this->connectHttpsHost(bio);
	}
}


void Feed::connectHttpHost(BIO **bio)
{
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	*bio = BIO_new_connect(string(this->host+":"+to_string(this->port)).c_str());
	if(*bio == NULL)
	{
		fprintf(stderr, "Error: Couldn't create connection");
		exit(1);		
	}

	if(BIO_do_connect(*bio) <= 0)
	{
		fprintf(stderr, "Error: Couldn't connect");
		exit(1);		
	}
}


void Feed::connectHttpsHost(BIO **bio)
{
	SSL *ssl;
    SSL_CTX *ctx;


	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL)
    {
            std::cout << "Ctx is null" << std::endl;
            ERR_print_errors_fp(stderr);
    }

    this->setupCertificates(&ctx);

    *bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(*bio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    //replace with your own test server
	BIO_set_conn_hostname(*bio, string(this->host+":"+to_string(this->port)).c_str());

    if(BIO_do_connect(*bio) <= 0)
    {
            std::cout<<"Failed connection" << std::endl;
            /* Handle failed connection */
    } else {
            //std::cout<<"Connected" << std::endl;
    }

    if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        /* Handle the failed verification */
        std::cout << "Failed get verify result " << std::endl;

        fprintf(stderr, "Certificate verification error: %i\n", SSL_get_verify_result(ssl));
    //do not exit here (but some more verification would not hurt) because if you are using a self-signed certificate you will receive 18
    //18 X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT which is not an error
    }
}

void Feed::setupCertificates(SSL_CTX **ctx)
{
    /*
    if(!SSL_CTX_load_verify_locations(ctx, "/tmp/openssl-0.9.8e/certs/vsign1.pem", NULL))
    {
        // Handle failed load here
            std::cout << "Faild load verify locations" << std::endl;

    }	@see https://www.openssl.org/docs/man1.1.0/ssl/SSL_CTX_set_default_verify_paths.html
    */
   
	bool certFileUsed = (this->program->flags.count('c') != 0);
	bool certFolderUsed = (this->program->flags.count('C') != 0);

	if(!certFileUsed && !certFolderUsed)
		SSL_CTX_set_default_verify_paths(*ctx);
}

void Feed::sendRequest(BIO **bio)
{
	
	string httpRequest("GET /"+this->path+" HTTP/1.0\r\n"
					"HOST: "+this->host+"\r\n"
					"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"
					"Connection: close\r\n\r\n");

    const char *httpRequestC = httpRequest.c_str();

    if(BIO_write(*bio, httpRequestC, strlen(httpRequestC)) <= 0)
    {
            if(!BIO_should_retry(*bio))
            {
                // Handle failed write here
            	std::cerr << "Failed write" << std::endl;
            }
        	// Do something to handle the retry 
            std::cerr << "Failed write (retrying)" << std::endl;
    }
    
}


string Feed::readResponse(BIO **bio)
{
	
	char buffer[65535];	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
    int len;
    string response;

    while(true)
    {
        len = BIO_read(*bio, buffer, sizeof(buffer)-1);
        if(len <= 0)
        	break;
        buffer[len] = 0;

        response += buffer;
    }

    return response;
}


string Feed::discardHeader(string content)
{
	size_t pos;

	pos = content.find("\r\n\r\n");
	if(pos != string::npos)
		pos += 4;
	else
	{
		pos = content.find("\n\n");
		if(pos != string::npos)
			pos += 2;
		else
		{
			cerr << "Couldn't find header in response" << endl;
			exit(1);
		}
	}   

	return content.substr(pos);
}

void Feed::parse(string content, unordered_multimap<int, string> flags)
{
	Parser parser(content);

	cout << parser.toString(flags);
}