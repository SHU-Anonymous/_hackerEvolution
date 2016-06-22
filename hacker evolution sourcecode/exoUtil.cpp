#include "exoUtil.h"
#include <string.h>

// ===========================================================================================================================
// counts the number of word (separated by spaces) in a string of chars
int eu_wordcount(char* string)
{
	int   count = 0,i;

	i = 0;

	// empty string case
	if(strlen(string) == 0) return count;

	// skip any preceding spaces
	while((string[i] == ' ')) i++;

	while(string[i] != 0)
	{
		while((string[i] != ' ') && (string[i] != 0)) i++;
		count++;

		// skip spaces
		while(string[i] == ' ') i++;
	}

	return count;
}
// ===========================================================================================================================
// checks if [string] ends with [with]
int eu_endswith(char* string, char *with)
{
	char* tmp;

	tmp = &(string[strlen(string) - strlen(with)]);
	if(strcmp(tmp,with) == 0)
		return 1;
		else
		return 0;
}
