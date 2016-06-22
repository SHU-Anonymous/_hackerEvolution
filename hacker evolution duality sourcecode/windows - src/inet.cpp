#include "inet.h"

UINT InternetGetFile (HINTERNET IN hOpen, // Handle from InternetOpen()
                 char *szUrl,        // Full URL
                 char *szFileName
				 )
{
       DWORD dwSize;
       CHAR   szHead[] = "Accept: */*\r\n\r\n";
       VOID* szTemp[16384];
       HINTERNET  hConnect;
       FILE * pFile;

    if ( !(hConnect = InternetOpenUrlA ( hOpen, szUrl, szHead,
             lstrlenA (szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
    {
         return INTERNET_ERROR_OPENURL;
	}

    pFile = fopen (szFileName, "wb" );


	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

        if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                      (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	    {
		dwByteToRead = 0;
	}
       do
       {
          // Keep coping in 16 KB chunks, while file has any data left.
          // Note: bigger buffer will greatly improve performance.
          if (!InternetReadFile (hConnect, szTemp, 16,  &dwSize) )
          {
			  fflush(pFile);
              fclose (pFile);
              return INTERNET_ERROR_READFILE;
          }
          if (!dwSize)
              break;  // Condition of dwSize=0 indicate EOF. Stop.
          else
             fwrite(szTemp, sizeof (char), dwSize , pFile);
	dwBytes += dwSize;
    }   // do
      while (TRUE);
      fflush (pFile);
      fclose (pFile);
      return 0;
}

int InternetDownloadFile(char *URL, char *FileDest)
{
DWORD dwFlags;
DWORD dwResult = INTERNET_ERROR_OPEN;
InternetGetConnectedState(&dwFlags, 0);
CHAR strAgent[64];
sprintf(strAgent, "Agent%ld", timeGetTime());

HINTERNET hOpen;
if(!(dwFlags & INTERNET_CONNECTION_PROXY))
hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
else
hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
if(hOpen)
{
	dwResult = InternetGetFile(hOpen, URL, FileDest);
	InternetCloseHandle(hOpen);
}
else return INTERNET_ERROR_OPEN;
return dwResult;
}
