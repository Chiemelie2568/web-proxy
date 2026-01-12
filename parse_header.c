
#include "parse_header.h"

/* Implemented HTTP Methods */
#define MAXM 8      // Number of method implemented.
static char *HTTP_METHODS[MAXM] = {
	"GET", "HEAD", "POST", "DELETE",
	"PUT", "CONNECT", "OPTIONS", "TRACE"
};

/* Function Proto-type */
static int parse_version(char *uri);
static int parse_uri(char *uri, char *method, char *hostname);
/* End */

static int parse_version(char *version)
{
	int len;
	char *hold = version;
	char word[MAXLINE];

	sscanf(hold, "%4s%n", word, &len);
	if (strcmp("HTTP", word))
		return 0;
	hold += len;

	if ('/' != (*hold++))
		return 0;
	
	if (!isdigit(*hold++))
		return 0;

	if ('.' != (*hold++))
		return 0;

	if (!isdigit(*hold++))
		return 0;
	return 1;
}

static int parse_uri(char *uri, char *method, char *hostname)
{
	int len, i=0, port;

	char *hold, word[MAXLINE];
	/* This proxy recieves a request with an absolute-form of request-target. */
	/* http://www.google.com/ */
	hold = uri;

	if (!strcmp("CONNECT", method)) {
		while ((*hold != '\0') && (*hold++ != ':'))
			hostname[i++] = *hold;
		hostname[i] = '\0';
		if ((sscanf(hold, "%3d", &port) != 1) || (port != 443))
			return 0;
		return 1;
	}

	i=0;
	sscanf(hold, "%7s%n", word, &len);
	if (strcmp("http://", word))
		return 0;

	hold += len;

	/* Read hostname utill you find a backslash, meaning a path. */
	while ((*hold != '\0') && (*hold != '/')) {
		hostname[i++] = *hold;
		++hold;
	}
	hostname[i] = '\0';
	sprintf(word, "%s", hostname);
	strcpy(hostname, word);
	return 1;
}

int Parse_StartLine(const char *string, char *method, char *uri, char *version, char *hostname)
{
	int len;
	char *hold;
	hold = string;

	/* Return error if the first character is not an alphabet */
	if (!isalpha(*hold)) {
		Client_Error();  // returns 400; bad request
		return 0;
	}

	/* Checks if the method it read, is implemented */
	sscanf(hold, "%s%n", method, &len);
	if (Wrong_Method(method)) {
		Client_Error();    // returns 501; not implemented
		return 0;
	}
	hold += len;

	/* Checks if a single space is in between */
	if ((*hold == ' ') && !isalpha(*++hold)) {
		Client_Error();    // Bad request
		return 0;
	}
	sscanf(hold, "%s%n", uri, &len);
	if (!parse_uri(uri, method, hostname)) {
		Client_Error();     // Bad uri
		return 0;
	}
	hold += len;

	/* Parsing the version */
	/* Checks if a single space is in between */
	if ((*hold == ' ') && !isalpha(*++hold)) {
		Client_Error();
		return 0;
	}
	sscanf(hold, "%s%n", version, &len);
	if (!parse_version(version)) {        /* Error when version is not compatible */
		Client_Error();
		return 0;
	}
	hold += len;

	/* Checks if start-line ends with CRLF */
	if (strcmp(hold, "\r\n")) {
		Client_Error();
		return 0;
	}
	return 1;
}

/*
void Parse_Headers()
{
}
*/

int Wrong_Method(const char *method)
{
	int i;
	for (i=0; i<MAXM; i++)
		if (!strcmp(HTTP_METHODS[i], method))
			return 0;
	return 1;
}

void Client_Error() {
	printf("BAD REQUEST\r\n400\r\n");
}


