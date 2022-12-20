#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <stdio.h>

#define SERVER_PORT 3000
#define SERVER_IP_ADDRESS "127.0.0.1"
#define FILE_SIZE_IN_BYTES 1979600

void send_message_to_server(char *half_file, int socket_fd);

int main()
{
    //calculation of xor values in xor variable
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

    //transferring the ip from string to binary form
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &server_address.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() failed: %d", errno);
        return -1;
    }

    //saving the ipv4 type and the server port using htons to convert the server port to network's order
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    //connect to server
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        printf("connect() failed with error code: %d", -1);
    }

    printf("connected to server\n");

    //open the file
    FILE *myFile;
    myFile = fopen("butt.txt", "r");

    // read the first half of the file into array
    char first_half_message[FILE_SIZE_IN_BYTES / 2];
    for (int i = 0; i < FILE_SIZE_IN_BYTES; i++)
    {
        fscanf(myFile, "%c", &first_half_message[i]);
    }

    // read the second half of the file into array
    char second_half_message[FILE_SIZE_IN_BYTES - FILE_SIZE_IN_BYTES / 2];
    for (int i = FILE_SIZE_IN_BYTES - FILE_SIZE_IN_BYTES / 2; i < FILE_SIZE_IN_BYTES; i++)
    {
        fscanf(myFile, "%c", &second_half_message[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    //infinet loop in order to send the file many times as we want
    while (1)
    {
        //changing the cc algorithm to cubic
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, "cubic", 5) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }

        //sending the first half of the file using send_message_to_server method
        printf("sender: sending the first half of the file\n");
        send_message_to_server(first_half_message, socket_fd);

        //receiving the xor value in result from the server using the recv method
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

        //if the sender's calculation of xor doesn't equal to the server calculation of xor close the socket, exit and let the user know
        if (xor != result)
        {
            printf("FRAUD!!!!\n");
            close(socket_fd);
            exit(1);
        }

        //change the cc algorithm to reno
        if (setsockopt(socket_fd, IPPROTO_TCP, TCP_CONGESTION, "reno", 4) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }

        //sending the second half of the file using send_message_to_server method
        printf("sending the second part of the file\n");
        send_message_to_server(second_half_message, socket_fd);

        //telling the sender to sleep for 15 seconds in order to give the server more time to accept all data
        //15 seconds is not always enough but average according to our research
        sleep(15);

        //user interface loop - asking the user if he wants to send the file again or exit the program
        free_will:
        printf("Send the file again? (y/n): ");
         scanf(" %c", &file_again);
         if (file_again == 'n')
        {
            //sending "N" to the server, equivalent to "EXIT" message from instructions
            int exit_send = send(socket_fd, "N", 1, 0);
            if(exit_send == -1){
                printf("exit message has failed to send\n");
            }
            close(socket_fd);
            file_again=0;

            //recommended and nice user interface :)
            printf("thank you for using our project.\n");
            printf("If you would like to leave input on our project, press 1.\n");
            printf("else, if you want to exit, press 0\n");
            int ans;
            scanf("%d", &ans);
            if(ans == 1){
                char opinion[1000];
                printf("please insert your opinion (up to 1000 bytes)\n");

                scanf("%s", opinion);

                printf("thank you so much! It is very important for us to improve, we will now transfer your input to the relevant sources..\n");
                printf("///////////////////////////////////////////////////////////////////\n");
                printf("                                                                   \n");
                printf("                                       ////////////////////////////\n");
                printf("                                     //                        // |\n");
                printf("                                   //                        //   |\n");
                printf("                                 //          /.../         //     |\n");
                printf("                               /////////////////////////////      |\n");
                printf("                               |                           |      |\n");
                printf("   |||||||||||                 |                           |      |\n");
                printf("   |         |                 |                           |      |\n");
                printf("   | opinion |  -------->      |       //trash-can//       |      |\n");
                printf("   |         |                 |                           |      |\n");
                printf("   |||||||||||                 |                           |    // \n");
                printf("                               |                           |  //   \n");
                printf("                               |                           |//     \n");
                printf("                               |////////////////////////////       \n");
                printf("                                                                   \n");
                printf("///////////////////////////////////////////////////////////////////\n");
            }
            exit(1);

        }
        else if(file_again == 'y')
        {
            //send to server "Y", equivalent to telling the server "run again"
            send(socket_fd, "Y", 1, 0);
        }
        else
        {
            //takes care if user enters another char besides y or n
            printf("so what do you want??\n");
            goto free_will;
        }
        
    }
}

//function that sends half of the file to the receiver using send method
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

