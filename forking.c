/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * @param   sfd         Server socket file descriptor.
 * @return  Exit status of server (EXIT_SUCCESS).
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
int forking_server(int sfd) {
    //puts("forking serv");
    Request * request;
    pid_t pid;

    signal(SIGCHLD, SIG_IGN);
    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        request = accept_request(sfd);
        if(request == NULL){continue;}
        pid=fork();
        if(pid == -1){
            close(sfd);
            free_request(request);
            //return(EXIT_FAILURE);
            continue;
        }
        else if(pid == 0){
            close(sfd);
            handle_request(request);
            free_request(request);
            exit(EXIT_SUCCESS);
        }
        else{
            //close(sfd);
            free_request(request);
            //return(EXIT_SUCCESS);
        }

	/* Fork off child process to handle request */
    }

    /* Close server socket */
    close(sfd);
    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
