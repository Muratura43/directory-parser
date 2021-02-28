#include "server.h"

#define CHUNK_SIZE 512

//Receive data in multiple chunks by checking a non-blocking socket
//Timeout in seconds
int recv_timeout(int s, int timeout)
{
    int size_recv, total_size = 0;
    struct timeval begin;
    struct timeval now;
    char chunk[CHUNK_SIZE];
    double timediff;

    //make socket non blocking
    fcntl(s, F_SETFL, O_NONBLOCK);

    //beginning time
    gettimeofday(&begin, NULL);

    while (1)
    {
        gettimeofday(&now, NULL);

        //time elapsed in seconds
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

        //if you got some data, then break after timeout
        if (total_size > 0 && timediff > timeout)
        {
            break;
        }

        //if you got no data at all, wait a little longer, twice the timeout
        else if (timediff > timeout * 2)
        {
            break;
        }

        memset(chunk, 0, CHUNK_SIZE); //clear the variable
        if ((size_recv = recv(s, chunk, CHUNK_SIZE, 0)) < 0)
        {
            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
            usleep(100000);
        }
        else
        {
            total_size += size_recv;
            printf("%s", chunk);
            //reset beginning time
            gettimeofday(&begin, NULL);
        }
    }
    return total_size;
}

void socketThread(int clientSocket)
{
    char client_message[2000];
    char buffer[1024];
    int newSocket = clientSocket;

    //Receive a reply from the client
    if (recv(newSocket, client_message, 2000, 0) < 0)
    {
        perror("Recv failed");
        exit(1);
    }

    // Send message to the client socket
    pthread_mutex_lock(&lock);
    print_directory_tree(".");

    char *message = malloc(sizeof(client_message) + 20);
    strcpy(message, "Hello Client : ");
    strcat(message, client_message);
    strcat(message, "\n");
    strcpy(buffer, message);
    free(message);
    pthread_mutex_unlock(&lock);
    sleep(1);
    send(newSocket, buffer, 13, 0);
    printf("Exit socketThread \n");
    close(newSocket);
}

int main()
{
    short serverPort = 8080;
    const char *serverIp = "127.0.0.1";

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    pid_t pid[50];

    //Create the socket
    // 1) Internet domain
    // 2) Stream socket
    // 3) Default protocol (TCP)
    if (-1 == (serverSocket = socket(AF_INET, SOCK_STREAM, 0)))
    {
        perror("Could not create socket");
        exit(1);
    }

    // Configure settings of the server address struct

    // Address family = Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(serverPort);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Bind the address struct to the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Failed to bind socket");
        exit(1);
    }

    //Listen on the socket, with 10 max connection requests queued
    if (listen(serverSocket, 10) != 0)
    {
        perror("Server cannot listen");
        exit(1);
    }
    else
    {
        printf("Server is listening...\n");
    }

    pthread_t tid[60];
    int i = 0;

    while (1)
    {
        /*---- Accept call creates a new socket for the incoming connection ----*/
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
        printf("Connection accepted from %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

        int pid_c = 0;
        if ((pid_c = fork()) == 0)
        {
            socketThread(newSocket);
        }
        else
        {
            pid[i++] = pid_c;
            if (i >= 49)
            {
                i = 0;
                while (i < 50)
                    waitpid(pid[i++], NULL, 0);
                i = 0;
            }
        }
    }
    return 0;
}