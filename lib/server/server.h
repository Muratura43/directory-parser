#ifndef H_SERVER
#define H_SERVER

#define BUFF_SIZE 2048

#include "../parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>      // for open
#include <unistd.h>     // for close, usleep
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void socketThread(int clientSocket);

#endif