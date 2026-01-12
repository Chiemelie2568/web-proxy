//#include <stdio.h>
#include "csapp.h"
#include "sbuf.h"
#include "parse_header.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* Maxs number of threads */
#define NTHREADS 6
/* Sbuf package buffer size */
#define SBUFSIZE 36
#define MAXLINE 8192


/* Function Proto-type */
void RRequest(int connfd);
void *thread(void *vargp);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

sbuf_t sbuf;  /* Shared buffer of connected descriptors */

int main(int argc, char **argv)
{
	int i, listenfd, connfd;
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	pthread_t tid;

	/* Check command line args */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	listenfd = Open_listenfd(argv[1]);

	sbuf_init(&sbuf, SBUFSIZE);
	for (i=0; i<NTHREADS; i++)     /* Create worker threads */
		Pthread_create(&tid, NULL, thread, NULL);

	while(1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
		Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s, %s)\n", hostname, port);

		sbuf_insert(&sbuf, connfd);    /* Insert connfd in buffer */
	}

	printf("%s", user_agent_hdr);
	return 0;
}

/* GetRequest Begins. */
void *thread(void *vargp)
{
	Pthread_detach(pthread_self());
	while (1) {
		int connfd = sbuf_remove(&sbuf);      /* Remove connfd from buffer */
		RRequest(connfd);

		Close(connfd);
	}
}

/* Recieve request from client */
void RRequest(int connfd)
{
	rio_t rio;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];

	Rio_readinitb(&rio, connfd);
	Rio_readlineb(&rio, buf, MAXLINE);

	printf("%s", buf);
	if (!Parse_StartLine(buf, method, uri, version))  // Bad request
		return;

	//printf("%s %s %s\r\n\r\n", method, uri, version);
	/* Building header */
}



