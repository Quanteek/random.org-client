# include <iostream>
# include "../lib/libinetsocket.hpp"
# include <vector>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sstream>
# include <sys/socket.h>
# include <string>

bool verbose = false;

std::string help_string = "Help for random.org client\n\
\n\
Options:\n\
	-b	Which base should we use? 2 8 10 16\n\
	-B	Specify bottom (lower bound) -1E9 -- 1E9\n\
	-h	Display this help\n\
	-n	How many numbers do you want? max. 1000 \n\
	-o	Specify a file to write in. The output is appended. Else, stdout is used\n\
	-T	Specify top (higher bound) -1E9 -- 1E9\n\
	-v	Be verbose\n";

struct err
{
	err(const char* e) { errmsg = e; }
	const char* errmsg;
};

void cut_header(int sock)
{
	char buffer;
	int brcount = 0;

	// Cut the HTTP header
	while ( 0 < read(sock,&buffer,1) )
	{
		if ( buffer == 0x0d || buffer == 0x0a)
		{
			++brcount;
			if ( brcount == 4 )
				break;
		} else
		{
			brcount = 0;
		}
	}
}

int request(int number, long long bottom, long long top, char base)
{
	std::ostringstream request_string;
	
	int sock = libsock::create_isocket("random.org","80",TCP,IPv4,0);
	
	request_string << "GET /integers/?num=" << number << "&min=" << bottom << "&max=" << top << "&col=1&base=" << static_cast<int>(base) << "&format=plain HTTP/1.1\n"
		<< "Host: www.random.org\n" << "User-agent: random.org client\n\n";

	if ( verbose == true)
		std::cout << "DBG: Request string: " << request_string.str() << "\n";

	write(sock,request_string.str().data(),request_string.str().length());
	
	shutdown(sock,SHUT_WR);

	return sock;
}

void get_data(int sock, int fileout)
{
	char buffer[32];
	int sizeread;

	while ( 0 < (sizeread = read(sock,buffer,32)) )
			write(fileout,buffer,sizeread);
}

int open_outfile(const char* outfile)
{
	int fileout;
	if ( verbose == true )
		std::cout << "DBG: Opening " << outfile << " for writing (append)\n\n";
		
	fileout = open(outfile,O_WRONLY|O_APPEND|O_CREAT,0644);

	if ( fileout < 0 )
		exit(1);

	return fileout;
}

int main(int argc, char** argv)
{
	int buf = 0, fileout = 1, sock;
	unsigned short number = 1;
	char base = 10; 
	bool fileio = false;
	const char* outfile;
	long long top = 100, bottom = 0;

	while ( -1 != (buf = getopt(argc,argv,"b:ho:n:vT:B:")) )
	{
		try {
			switch ( buf )
			{
				case 'n' :
					number = static_cast<short>(strtoul(optarg,0,10));
					break;
				case 'b' :
					base = strtoul(optarg,0,10);
					if ( base != 2 && base != 8 && base != 10 && base != 16 )
						throw err("The specified base is not valid for random.org!");

					break;
				case 'v' :
					verbose = true;
					break;
				case 'o' :
					fileio = true;
					outfile = optarg;
					break;
				case 'T' :
					top = strtoull(optarg,0,10);
					break;
				case 'B' :
					bottom = strtoull(optarg,0,10);
					break;
				case 'h' :
					std::cout << help_string;
					exit(0);
			}

			if ( top <= bottom )
				throw err("Invalid range!");

			if ( top < -1E9 || top > 1E9 )
				throw err("top is too big or too small");

			if ( bottom < -1E9 || bottom > 1E9 )
				throw err("bottom is too big or too small"); 
			if ( number > 1000 )
				throw err("Number too high");

		} catch (err error)
		{
			std::cout << "Exception raised: " << error.errmsg << std::endl;
			exit(1);
		}
	}

	if ( verbose == true )
		std::cout << "DBG: Done with processing arguments\n";
	
	if ( fileio == true )
	{
		fileout = open_outfile(outfile);
	}

	sock = request(number,bottom,top,base);
	
	cut_header(sock);

	get_data(sock,fileout);

	return 0;
}
