#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <stdio.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"
#define FILE_SIZE_IN_BYTES 1979600

void send_message_to_server(char *half_file, int socket_fd);
void receive_message_from_server(int socket_fd);

int main()
{
    int dtaz1 = 0700;
    int dtaz2 = 2093;
    int xor = dtaz1 ^ dtaz2;
    char file_again;

    // create the socket file descriptor -> an integer that will represent the connection
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // if the socket file descriptor equals to -1 then the socket wasn't created
    if (socket_fd == -1)
    {
        printf("Could not create socket: %d", errno);
    }

    // create the server_address struct that contains the ip protocol and the server port
    struct sockaddr_in server_address;
    // zero all bites of the server_address spot in memory
    memset(&server_address, 0, sizeof(server_address));

    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &server_address.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() failed: %d", errno);
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        printf("connect() failed with error code: %d", -1);
    }

    printf("connected to server\n");

    FILE *myFile;
    myFile = fopen("butt.txt", "r");

    // read file into array

    char first_half_message[FILE_SIZE_IN_BYTES / 2];
    for (int i = 0; i < FILE_SIZE_IN_BYTES; i++)
    {
        fscanf(myFile, "%c", &first_half_message[i]);
    }

    char second_half_message[FILE_SIZE_IN_BYTES - FILE_SIZE_IN_BYTES / 2];
    for (int i = FILE_SIZE_IN_BYTES - FILE_SIZE_IN_BYTES / 2; i < FILE_SIZE_IN_BYTES; i++)
    {
        fscanf(myFile, "%c", &second_half_message[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    while (1)
    {
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, "cubic", 5) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }

        printf("sender: sending the first half of the file\n");
        send_message_to_server(first_half_message, socket_fd);
        // receive_message_from_server(socket_fd);

        int result = 0;
        int bytes_received = recv(socket_fd, &result, sizeof(result), 0);
        if (bytes_received == -1)
        {
            printf("recv() failed with error code : %d", errno);
        }
        else if (bytes_received == 0)
        {
            printf("peer has closed the TCP connection prior to recv().\n");
        }
        else
        {
            printf("received %d bytes from server: %d\n", bytes_received, result);
        }


        if (xor != result)
        {
            printf("FRAUD!!!!\n");
            close(socket_fd);
            exit(1);
        }

        sleep(1);

        if (setsockopt(socket_fd, IPPROTO_TCP, TCP_CONGESTION, "reno", 4) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }

        printf("sending the second part of the file\n");
        send_message_to_server(second_half_message, socket_fd);


        printf("Send the file again? (y/n): ");
         scanf(" %c", &file_again);
         if (file_again == 'n')
        {
            send(socket_fd, "N", 1, 0);
            close(socket_fd);
            file_again=0;
            exit(1);

        }
        else
        {
            send(socket_fd, "Y", 1, 0);
        }
        
    }
    sleep(3);
    close(socket_fd);
    return 0;
}

void send_message_to_server(char *half_file, int socket_fd)
{
    int bytes_sent = send(socket_fd, half_file, FILE_SIZE_IN_BYTES / 2, 0);
    if (bytes_sent == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(socket_fd);
        exit(1);
    }
    else if (bytes_sent == 0)
    {
        printf("peer has closed the TCP connection prior to send().\n");
    }
    else if (bytes_sent < FILE_SIZE_IN_BYTES / 2)
    {
        printf("sent only %d bytes from the required %d.\n", FILE_SIZE_IN_BYTES / 2, bytes_sent);
    }
    else
    {
        printf("message was successfully sent.\n");
    }
}

void receive_message_from_server(int socket_fd)
{
    char *receive_space = (char *)calloc(sizeof(char), 1000);
    if (receive_space == NULL)
    {
        printf("space wasn't allocated, screw you");
        close(socket_fd);
        exit(1);
    }
    int bytes_received = recv(socket_fd, receive_space, 1000, 0);
    if (bytes_received == -1)
    {
        printf("recv() failed with error code : %d", errno);
    }
    else if (bytes_received == 0)
    {
        printf("peer has closed the TCP connection prior to recv().\n");
    }
    else
    {
        printf("received %d bytes from server: %s\n", bytes_received, receive_space);
    }
}