/*
 * touch.c
 * Lee Crampton
 * sdclee@gmail.com
 * Original date : October 29th 2011
 * Last change   : December 16th 2001
 *
 * A simple implementation of the *nix touch program for the windows command line.
 * There are version freely available on the internet but I wasn't able to locate one that
 * implements all of the the flags and to create a file where one does not exist.
 *
 * The source code can be freely distributed and modified but as a courtesy, please leave the
 * original author details in tact.
 * License GPLv3+: GNU GPL version 3 or later
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#ifdef __linux__
#include <utime.h>
#else
#include <sys/utime.h>
#endif

#define FALSE 0;
#define TRUE  1;

int touch (const char *filename, struct utimbuf *timestamp, short int bCreate);

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		printf ("usage: touch [flags] <files>\n");
		printf ("       touch --help\n");
		exit (0);
	}
	
	short int bTimeStamp = FALSE;
	short int bCreate = TRUE;
	struct tm tma = {0}, tmm = {0};
	struct utimbuf ut;
	struct utimbuf *pnull = NULL;

	if (argc == 2)
	{
		if (0 == strcmp (argv [1], "--version"))
		{
			printf ("touch (SoftDev version 0.02)\n");
			printf ("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
			printf ("This is free software: you are free to change and redistribute it.\n");
			printf ("There is NO WARRANTY, to the extent permitted by law.\n");
			printf ("SoftDev version written by Lee Crampton sdclee@gmail.com\n");
			exit (0);
		}

		if (0 == strcmp (argv [1], "--help"))
		{
			printf ("How about man touch (linux) or googling for man touch in you are on a windows box?\n");
			exit (0);
		}
		
		touch (argv [1], pnull, bCreate);
		exit (0);
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
	int iFilePos = 0;

	for (i = 0; i < argc; i++)
	{
		//DEBUG
		//printf ("arg %i = %s\n", i, argv [i]);

		if (strcmp (argv [i], "-c") == 0)
		{
			bCreate = FALSE;
		}
		
		if (strcmp (argv [i], "-t") == 0)
		{
			bTimeStamp = 1;
			iFilePos = i + 2;	// where the file(s) potentially start if there are no other flags
			// no safety check here yet !
			
			if (i == argc)
				perror ("No timestamp following -t flag!");
				
			char *tStamp = argv [i + 1];	
			
			//  [[CC]YY]MMDDhhmm[.ss] 
			tma.tm_sec = tmm.tm_sec = tma.tm_isdst = tmm.tm_isdst = 0;
			
			char szSec [3];
			strcpy (szSec, "00");
			
			int tStampLen = strlen (tStamp);
			
			if (tStamp [tStampLen - 3] == '.')
			{
				strncpy (szSec, &tStamp [tStampLen - 2], 2);
				
				// then just lop off the end with a judiciously placed null to make the right hand end of the string
				// work for MMDDhhmm
				tStamp [tStampLen - 3] = '\0';
				tStampLen = strlen (tStamp);
			}
			
			char szMin [3];
			strncpy (szMin, &tStamp [tStampLen - 2], 2);
			szMin [2] = '\0';

			char szHour [3];
			strncpy (szHour, &tStamp [tStampLen - 4], 2);
			szHour [2] = '\0';

			char szDay [3];
			strncpy (szDay, &tStamp [tStampLen - 6], 2);
			szDay [2] = '\0';

			char szMon [3];
			strncpy (szMon, &tStamp [tStampLen - 8], 2);
			szMon [2] = '\0';

			tma.tm_sec  = tmm.tm_sec   = atoi (szSec); 
			tma.tm_min  = tmm.tm_min   = atoi (szMin); 
			tma.tm_hour = tmm.tm_hour  = atoi (szHour); 
			tma.tm_mday = tmm.tm_mday  = atoi (szDay); 
			tma.tm_mon  = tmm.tm_mon   = atoi (szMon) - 1;	// 0 = January 

			char szYear [21];
			memset (szYear, '\0', sizeof (szYear));
			
			// We could have 8, 10 or 12 chars left in the string from which to determine the year (and century)
			if (strlen (tStamp) == 8)
			{
				// no CCYY specified		
				// Get the current time (just need the year part)
				time_t currtime;
				struct tm *loctime;
				currtime = time (NULL);
				loctime = localtime (&currtime);
				strftime (szYear, sizeof (szYear), "%Y", loctime); 
				tma.tm_year = tmm.tm_year = atoi (szYear) - 1900;	
			}
			else if (strlen (tStamp) == 10)
			{
				// just YY specified		
				strncpy (szYear, tStamp, 2);
				tma.tm_year = tmm.tm_year = atoi (szYear);	
			}
			else if (strlen (tStamp) == 12)
			{
				// CCYY specified
				strncpy (szYear, tStamp, 4);
				tma.tm_year = tmm.tm_year = atoi (szYear) - 1900;	
			}			
			else
			{
				perror ("Invalid timestamp - [[CC]YY]MMDDhhmm[.ss]");
			}
			//printf ("year = %s month = %s day = %s hours = %s minutes = %s seconds = %s\n", szYear, szMon, szDay, szHour , szMin, szSec);		
			//printf ("tmyear = %d tmmonth = %d tmday = %d tmhours = %d tmminutes = %d tmseconds = %d\n", tmm.tm_year, tmm.tm_mon, tmm.tm_mday, tmm.tm_hour , tmm.tm_min, tmm.tm_sec);		
			
			// Convert tm to time_t
			ut.actime  = mktime (&tma);
			ut.modtime = mktime (&tmm);
			
			// So we need to perform a _utime () after the file(s) have been determined
			if (iFilePos <= argc)
			{
				//printf ("args=%d file(s) start at offset %d\n", argc, iFilePos);
				int j;
				
				for (j = iFilePos; j < argc; j++)
				{
					touch (argv [j], &ut, bCreate);
				}
			}
		}
		else
		{
			// no special args to deal with
			int j;
				
			for (j = iFilePos; j < argc; j++)
			{
				touch (argv [j], pnull, bCreate);
			}
		}
	}
		
	exit (0);
}
 
int touch (const char *filename, struct utimbuf *timestamp, short int bCreate)
{
	int res = 0;
		
	// Does the file exist. If not, create it?
	FILE *fp = fopen (filename, "r");

	if (fp) 
	{
		// exists
		fclose (fp);

		if (-1 == utime (filename, timestamp))
			perror ("utime failed\n");
	} 
	else if (bCreate)
	{
		// doesn't exist
		printf ("No such file: %s\n", filename);
		fp = fopen (filename, "wb");
		
		if (fp)
		{
			fclose (fp);

			if (-1 == utime (filename, timestamp))
				perror ("utime failed\n");
		}
		else
		{
			perror ("create file failed\n");
		}
	}		
	
	//system (sys);
}
