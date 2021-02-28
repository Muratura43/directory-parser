#include "client.h"

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

    // //Read the message from the server into the buffer
    // if (recv(clientSocket, buffer, 1024, 0) < 0)
    // {
    //     perror("Receive failed\n");
    //     exit(1);
    // }

    //Now receive full data
    int total_recv = recv_timeout(clientSocket, 4);

    printf("\n\nDone. Received a total of %d bytes\n\n", total_recv);

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
