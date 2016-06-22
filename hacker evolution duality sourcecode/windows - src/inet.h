/*

INET.H

Header containing InternetGetFile and a wrapper function, InternetDownloadFile which also makes the Internet connection.

*/

#ifndef INET_H
#define INET_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include <wininet.h>
#include <mmsystem.h>

enum
{
	INTERNET_ERROR_OPENURL=1,
	INTERNET_ERROR_FILEOPEN,
	INTERNET_ERROR_READFILE,
	INTERNET_ERROR_OPEN
};

UINT InternetGetFile (HINTERNET IN hOpen, // Handle from InternetOpen()
                 char *szUrl,        // Full URL
                 char *szFileName
				 );

int InternetDownloadFile(char *URL, char *FileDest);

#endif /*INET_H*/