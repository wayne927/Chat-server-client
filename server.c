#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>

#define BUFFER_LEN 1000
#define MAX_CONNECTIONS 10

int sockfds[MAX_CONNECTIONS];
pthread_t threads[MAX_CONNECTIONS];

/* Sent messages to all open sockets (ie. all clients that are connected) */
void broadcast(char* msg)
{
    int i;
    for(i=0; i<MAX_CONNECTIONS; i++)
    {
        if(sockfds[i] > 0)
            write(sockfds[i], msg, strlen(msg));
    }
}

/* Thread routine that receives messages from clients and broadcasts those messages out */
void* chat_client(void* arg_cid)
{
    int cid = (int)arg_cid;
    int sockfd = sockfds[cid];

    printf("Connection accepted! Now chatting with sockfd=%d\n", sockfd);

    char buffer[BUFFER_LEN];
    char bcast_msg[BUFFER_LEN];

    while(1)
    {
        memset(buffer, 0, BUFFER_LEN);
        read(sockfd, buffer, BUFFER_LEN);

        /* Client sent an EOF. Breaks loop and closes connection. */
        if(buffer[0] == 0)
            break;
        else
        {
            memset(bcast_msg, 0, BUFFER_LEN);
            sprintf(bcast_msg, "sockfd=%d> %s", sockfd, buffer);
            broadcast(bcast_msg);   
            // printf("sockfd=%d> %s", sockfd, buffer);
        }
    }

    printf("Done chatting with sockfd=%d. Closing connection\n", sockfd);
    sockfds[cid] = -1;  /* Open socket file descriptors are positive */

    close(sockfd);

    pthread_exit(NULL);
}

/* Usage: server port */
int main(int argc, char* argv[])
{
    int portnum = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in server_address, client_address;
    int client_address_length;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        perror("Error opening socket\n");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portnum);

    if( bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0 )
    {
        perror("Error binding socket\n");
        exit(1);
    }

    listen(sockfd, MAX_CONNECTIONS);

    /* Initialize the empty list of client sockets to -1. Open sockets should be positive. */
    int i;
    for(i=0; i<MAX_CONNECTIONS; i++)
        sockfds[i] = -1;

    char bcast_msg[BUFFER_LEN];

    int cid = 0;
    while(1)
    {        
        // printf("Listening for clients on port %d...\n", portnum);

        int this_sockfd = accept(sockfd, (struct sockaddr*)&client_address, &client_address_length);

        if(this_sockfd < 0)
        {
            perror("Error accepting\n");
            exit(1);
        }

        sockfds[cid] = this_sockfd;

        /* The server creates one dedicated thread to communicate with each connected client. */
        pthread_create(&threads[cid], NULL, chat_client, (void*)cid);

        /* Announces a new client has joined */
        memset(bcast_msg, 0, BUFFER_LEN);
        sprintf(bcast_msg, "Server> sockfd=%d has joined!!\n", sockfds[cid]);
        broadcast(bcast_msg);

        cid++;
    }
    
    /* Server never quits until it's killed. It keeps waiting for new clients to join.
       I probably need to add some more code to stop new clients from joining when
       MAX_CONNECTIONS is reached */
    

    return 0;
}