/*
 * touch.c
 * Lee Crampton
 * lee@softdev.co.uk
 * October 29th 2011
 *
 * A simple implementation of the *nix touch program for the windows command line.
 * There are version freely available on the internet but I wasn't able to locate one that
 * implements the flags and to create a file where one does not exist.
 *
 * The source code can be freely distributed and modified but as a courtesy, please leave the
 * original author details in tact.
 *
 * See man touch(1)
 *
 * (part produced here)
 * touch [OPTION]... FILE...
 * -a
 * 	change only the access time
 * -c, --no-create
 * 	do not create any files
 * -d, --date=STRING
 * 	parse STRING and use it instead of current time
 * -f
 * 	(ignored)
 * -m
 *	change only the modification time
 * -r, --reference=FILE
 *	use this file's times instead of current time
 * -t STAMP
 *	use [[CC]YY]MMDDhhmm[.ss] instead of current time
 * --time=WORD
 *	change the specified time: WORD is access, atime, or use: equivalent to -a WORD is modify or mtime: equivalent to -m
 * --help
 *	display this help and exit
 * --version
 *	output version information and exit
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/utime.h>

// int utime(const char *path, const struct utimbuf *times)

int touch (char *filename, char **argv);

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		printf ("usage: touch <file> [flags]\n");
		exit (1);
	}
	
	if (argc == 2)
	{
		touch (argv [1], NULL);
	}
	
	/*
	 * Processing the args is slightly awkward in that most (but not all) are paired, eg -t STAMP
	 * but -a -c and -d are single args, so we need to proceed with a little caution.
	 * Also since there can be multiple files we can't just rely on the last arg being the one file,
	 * eg: touch -c foo bar
	 * means touch foo and bar bit don't create the files
	 *
	 * We should sort out the file list, the single args and the arg pairs
	 */
	int i;
	for (i = 0; i < argc; i++)
	{
		printf ("arg %i = %s\n", i, argv [i]);
	
		//tStamp = checkArgs (argv, '-t');
		if (strcmp (argv [i], "-t") == 0)
		{
			char *tStamp = argv [i + 1];	
			
			printf ("timsestamp: %s\n", tStamp);	
		}
	}
	
/*
    for (i = 1; i < argc; i++)  // Skip argv[0] (program name). 
    {
        //
        // Use the 'strcmp' function to compare the argv values
        // to a string of your choice (here, it's the optional
        // argument "-q").  When strcmp returns 0, it means that the
        // two strings are identical.
        //

        if (strcmp(argv[i], "-q") == 0)  // Process optional arguments. 
        {
            quiet = 1;  // This is used as a boolean value. 
        }
        else
        {
            // Process non-optional arguments here. 
        }
    }
	
 */	
	
	exit (0);
}

/*
The utimbuf structure is used with the utime function to specify new access and modification times for a file. It contains the following members:

time_t actime
This is the access time for the file. 
time_t modtime
This is the modification time for the file.

           struct tm
           { int tm_sec;      // 0 to 59 (or 60 for occasional rare leap-seconds)
             int tm_min;      // 0 to 59
             int tm_hour;     // 0 to 23
             int tm_mday;     // 1 to 31
             int tm_mon;      // 0 to 11, stupidly 0=January, 11=December
             int tm_year;     // year-1900, so 79 means 1979, 103 means 2003
             int tm_wday;     // 0 to 6, 0=Sunday, 1=Monday, ..., 6=Saturday
             int tm_yday;     // 0 to 365, 0=1st January
             int tm_isdst;    // 0 to 1, 1=DST is in effect, 0=it isn't
             char *tm_zone;   // time zone, e.g. "PDT", "EST".
             int tm_gmtoff; } // time zone in seconds from GMT; EST=-18000, WET=3600
       tm structures are produced from by localtime and gmtime.
       tm structures are converted to strings by asctime.
       tm structures are converted to seconds by mktime and .
 
 
	int _utime( unsigned char *filename, struct _utimbuf *times );
	
	//Show file time before and after.
	system( "dir utime.c" );
	if( _utime( "utime.c", NULL ) == -1 )
      perror( "_utime failed\n" );
	else
      printf( "File time modified\n" );
	system( "dir utime.c" );
	
 */
 
int touch (char *filename, char **argv)
{
	struct utimbuf ut;

	if (argv == NULL)
	{
		printf ("Simple touch with filename: %s\n", filename); 
		
		// Show file time before
		char sys [100];
		sprintf (sys, "dir %s", filename);
		system (sys);
		
		// Does the file exist. If not, create it?
		FILE *fp = fopen (filename, "r");

		if (fp) 
		{
			// exists
			fclose (fp);

			if (_utime (filename, NULL) == -1)
				perror ("_utime failed\n");
			else
				printf ("File time modified\n");
		} 
		else 
		{
			// doesnt exist
			printf ("No such file: %s\n", filename);
			fp = fopen (filename, "wb");
			
			if (fp)
			{
				fclose (fp);
			}
			else
			{
				perror ("create file failed\n");
			}
		}		
		
		system (sys);
	}
}
