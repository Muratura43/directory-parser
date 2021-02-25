#include "client.h"

void *clientThread(void *arg)
{
    short port = 8080;
    const char *serverIp = "127.0.0.1";

    // Send message to the client socket
    pthread_mutex_lock(&lock);
    printf("In thread\n");
    char message[1000];
    char buffer[1024];
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // Create the socket.
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Could not create socket");
        exit(1);
    }

    //Configure settings of the server address

    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);

    // Address family is Internet
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function
    serverAddr.sin_port = htons(port);

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;

    if ((connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size)) == -1)
    {
        perror("Connection error");
        exit(1);
    }

    strcpy(message, ".");

    if (send(clientSocket, message, strlen(message), 0) < 0)
    {
        perror("Send failed\n");
        exit(1);
    }

    //Read the message from the server into the buffer
    if (recv(clientSocket, buffer, 1024, 0) < 0)
    {
        perror("Receive failed\n");
        exit(1);
    }

    //Print the received message
    printf("Data received: %s\n", buffer);
    pthread_mutex_unlock(&lock);
    close(clientSocket);
    pthread_exit(NULL);
}

int main()
{

    int i = 0;
    pthread_t tid[3];
    while (i < 3)
    {
        if (pthread_create(&tid[i], NULL, clientThread, NULL) != 0)
            printf("Failed to create thread\n");
        i++;
    }
    sleep(2);
    i = 0;
    while (i < 3)
    {
        pthread_join(tid[i++], NULL);
        printf("Thread %d dead \n", i);
    }
    return 0;
}
