#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>

#define BUFFER_LEN 1000

/* Thread routine that receives and prints messages from the server */
void* chat_server(void* arg_sockfd)
{
    char buffer[BUFFER_LEN];

    int sockfd = (int)arg_sockfd;
    while(1)
    {
        memset(buffer, 0, BUFFER_LEN);
        read(sockfd, buffer, BUFFER_LEN);

        printf("%s", buffer);
    }

    pthread_exit(NULL);

}

/* Usage: client hostname port */
int main(int argc, char* argv[])
{
    int sockfd;
    int portnum = htons(atoi(argv[2]));

    struct sockaddr_in server_address;
    struct hostent* server_host;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        perror("Error opening socket\n");
        exit(1);
    }

    printf("Socket created\n");

    server_host = gethostbyname(argv[1]);

    if(server_host == NULL)
    {
        perror("Error resolving host\n");
        exit(1);
    }

    printf("Host resolved\n");

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = portnum;
    memcpy((char*)&(server_address.sin_addr.s_addr), (char*)server_host->h_addr, server_host->h_length);

    printf("Connecting...\n");

    if( connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0 )
    {
        perror("Error connecting\n");
        exit(1);
    }

    printf("Connected!! Type your message!\n");

    /* Creates the thread that receives and prints messages from the server */
    pthread_t thread;
    pthread_create(&thread, NULL, chat_server, (void*)sockfd);

    const int MSG_LEN = 10000;
    char msg[MSG_LEN];
    memset(msg, 0, MSG_LEN);

    /* Read until we get EOF */
    while(fgets(msg, MSG_LEN, stdin))
    {
        write(sockfd, msg, strlen(msg));
        memset(msg, 0, MSG_LEN);
    }

    printf("Closing connection\n");
    close(sockfd);



}
