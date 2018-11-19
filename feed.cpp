#include "feed.hpp"

Feed::Feed(Program *program, string url)
{
	// Save pointer to the program (to have access to flags)
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

bool Feed::read()
{
	BIO *bio;

	if(!this->connectHost(&bio))
		return this->closeBio(&bio);

	this->sendRequest(&bio);


	string response = this->readResponse(&bio);

	if(!this->checkHeader(response))
		return this->closeBio(&bio);

	string content = this->discardHeader(response);

    BIO_free_all(bio);

    this->parse(content, this->program->flags);

    return true;
}

bool Feed::closeBio(BIO **bio)
{
	BIO_free_all(*bio);
	return false;	
}


bool Feed::connectHost(BIO **bio)
{
	if(this->port == 80)
	{
		return this->connectHttpHost(bio);
	}
	else if(this->port == 443)
	{
		return this->connectHttpsHost(bio);
	}
}


bool Feed::connectHttpHost(BIO **bio)
{
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	*bio = BIO_new_connect(string(this->host+":"+to_string(this->port)).c_str());
	if(*bio == NULL)
	{
		cerr << "ERROR: Couldn't create connection\n";
		return false;		
	}

	if(BIO_do_connect(*bio) <= 0)
	{
		cerr << "ERROR: Connection failed\n";
		return false;		
	}

	return true;
}


bool Feed::connectHttpsHost(BIO **bio)
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
        cerr << "ERROR: Couldn't create context strucutre\n";
        return false;
    }

    this->setupCertificates(&ctx);

    *bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(*bio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    //replace with your own test server
	BIO_set_conn_hostname(*bio, string(this->host+":"+to_string(this->port)).c_str());

    if(BIO_do_connect(*bio) <= 0)
    {
        cerr << "ERROR: Connection failed\n";
        return false;
    }

    long int verifyResult = SSL_get_verify_result(ssl);
    if(verifyResult != X509_V_OK &&
    	verifyResult != X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT)
    {
    	cerr << "ERROR: Certificate verification failed\n";
    	return false;
    }

    return true;
}

void Feed::setupCertificates(SSL_CTX **ctx)
{
	// Check if certification parameteres were used
	bool certFileUsed = (this->program->flags.count('c') != 0);
	bool certFolderUsed = (this->program->flags.count('C') != 0);

	// Set default location when certificates weren't used
	if(!certFileUsed && !certFolderUsed)
	{
		SSL_CTX_set_default_verify_paths(*ctx);
		return;
	}

	// Loop through used parameters
	for(auto const& flag: this->program->flags)
	{
		// Process parameter -c
		if(flag.first == 'c')
		{
			if(!SSL_CTX_load_verify_locations(*ctx, flag.second.c_str(), NULL))
			{
				cerr << "ERROR: File specified in -c parameter failed to load\n";
			}
		}
		// Process parameter -C
		else if(flag.first == 'C')
		{
			if(!SSL_CTX_load_verify_locations(*ctx, NULL, flag.second.c_str()))
			{
				cerr << "ERROR: Location specified in -C parameter failed to load\n";
			}
		}
	}

}

void Feed::sendRequest(BIO **bio)
{
	// Prepare HTTP request
	string httpRequest("GET /"+this->path+" HTTP/1.0\r\n"
					"HOST: "+this->host+"\r\n"
					"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"
					"Connection: close\r\n\r\n");
    const char *httpRequestC = httpRequest.c_str();

    // Send HTTP request
    if(BIO_write(*bio, httpRequestC, strlen(httpRequestC)) <= 0)
    {
            if(!BIO_should_retry(*bio))
            {
                // Handle failed write here
            	cerr << "Failed write" << std::endl;
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


bool Feed::checkHeader(string content)
{
	string responseCode(content.substr(9, 3)); 
	if(responseCode != "200")
	{
		cerr << "ERROR: Unexpected HTTP response code " << responseCode << "\n";
	    return false;			
	}

	return true;
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