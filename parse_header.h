#ifndef __PARSE_HEADER_H__
#define __PARSE_HEADER_H__

#include "csapp.h"

/* Maxline for any buffer */
#define MAXLINE 8192

/* Function Proto-type */
void Client_Error();
int Wrong_Method(const char *method);
int Parse_StartLine(const char *string, char *method, char *uri, char *version);



#endif

