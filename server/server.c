#include "server.h"

void socketThread(int clientSocket)
{
    char client_message[2000];
    char buffer[1024];
    int newSocket = clientSocket;
    recv(newSocket, client_message, 2000, 0);

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

        int pid_c = 0;
        if ((pid_c = fork()) == 0)
        {
            // printf("Client is sending work...\n");
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