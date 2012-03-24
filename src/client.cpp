# include <iostream>
# include "../lib/libinetsocket.c"
# include <vector>
# include <stdlib.h>
# include <unistd.h>

struct err
{
	err(const char* e) { errmsg = e; }
	const char* errmsg;
};

int main(int argc, char** argv)
{
	int number = 1, buf = 0;
	char type = 0; // i, b
	char base = 10; //
	bool verbose = false, fileio = false;
	const char* outfile;
	
	while ( -1 != (buf = getopt(argc,argv,"b:o:rin:v")) )
	{
		try {
			switch ( buf )
			{
				case 'i' : 
					if ( type != 0 )
					{
						throw err("Please specify only one type!");
					} else
					{
						type = 'i';
					}
					break;
				case 'r' :
					if ( type != 0 )
					{
						throw err("Please specify only one type!");
					} else
					{
						type = 'r';
					}
					break;
				case 'n' :
					number = strtoul(optarg,0,10);
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
			}
		} catch (err error)
		{
			std::cout << "Exception raised: " << error.errmsg << std::endl;
			abort();
		}
	}
	
	return 0;
}
