#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "myqueue.h"


#define SERVER_PORT 3000
#define FILE_SIZE_IN_BYTES 1979600
#define CHUNK_SIZE 1024

void receive_file(char* receive_space, int client_socket, int server_socket_fd);
void print_report(int number_of_iterations);

int main()
{
    //calculation of xor values in xor variable
    int dtaz1 = 0700;
    int dtaz2 = 2093;
    int xor = dtaz1 ^ dtaz2;

    char *receive_space = (char *)calloc((FILE_SIZE_IN_BYTES / 2), sizeof(char));
    if (receive_space == NULL)
    {
        printf("place wasn't allocated!!\n");
        exit(-1);
    }

    // creating the server socket with the function socket()
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        printf("Could not create socket: %d\n", errno);
    }

    // creating the struct with the name and the address of the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    //saving the ipv4 type and the server port using htons to convert the server port to network's order
    server_address.sin_family = AF_INET;
    //allow any IP at this port to address the server
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    int bind_res = bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    // if bind worked, bind_res == 0
    if (bind_res == -1)
    {
        printf("Bind failed with error code : %d", errno);
        // close the socket
        close(server_socket_fd);
        return -1;
    }

    int listen_value = listen(server_socket_fd, 1);
    // if listen worked, then listen_value == 0.
    if (listen_value == -1)
    {
        printf("listen didn't work with code: %d", errno);
        close(server_socket_fd);
        exit(1);
    }

    //we created an infinite loop because we want our server to work all the time and receive requests from different clients
    while(1)
    {
    printf("Waiting for incoming TCP-connections...\n");

    // declaring a new struct for the client port and ip and zero all bytes in it. accept method will fill these in the struct
    struct sockaddr_in client_address; //
    socklen_t client_address_len = sizeof(client_address);

    memset(&client_address, 0, sizeof(client_address));
    client_address_len = sizeof(client_address);
    //server will accept requests from client with accept method
    int client_socket = accept(server_socket_fd, (struct sockaddr *)&server_address, &client_address_len);
    if (client_socket == -1)
    {
        printf("accept didn't work with code: %d", errno);
        close(server_socket_fd);
        exit(1);
    }
    int iteration_number = 0;

    printf("welcome to our TCP server! :) make yourself at home\n");

    //we created a second infinite loop because we want the server to receive the files as many times as the client wishes to send it
    while (1)
    {
        //changing the cc algorithm to cubic
        if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, "cubic", 5) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }

        struct timeval start_t_cubic, end_t_cubic, tval_result_cubic; // will use them to check the timing
        struct timeval start_t_reno, end_t_reno, tval_result_reno;  
        // will use them to check the timing
        gettimeofday(&start_t_cubic, NULL); // start count for first part of the file
        //receive the file from the client using receive_file method
        receive_file(receive_space, client_socket, server_socket_fd);
        iteration_number++;
        gettimeofday(&end_t_cubic, NULL); // finish count for first part of the file
        timersub(&end_t_cubic, &start_t_cubic, &tval_result_cubic); // total time for cubic
        long int *time_elapsed_cubic = (long int *)malloc(sizeof(long int));
        *time_elapsed_cubic = tval_result_cubic.tv_sec * 1000000 + tval_result_cubic.tv_usec;
        int *iteration_number_p = (int *)malloc(sizeof(int));
        *iteration_number_p = iteration_number;
        int *cubic_param = (int *)malloc(sizeof(int));
        *cubic_param = 0;
        enqueue(time_elapsed_cubic, iteration_number_p, cubic_param);
        
        printf("algo: cubic, time: %ld.%06ld, iter num: %d\n",
               (long int)tval_result_cubic.tv_sec,
               (long int)tval_result_cubic.tv_usec,
               iteration_number);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////

        //sending the xor value to client
        int num_message = xor;

        int bytes_sent = send(client_socket, &num_message, sizeof(num_message), 0);
        if (bytes_sent == -1)
        {
            printf("send() failed with error code : %d\n", errno);
            close(server_socket_fd);
            close(client_socket);
            exit(1);
        }
        else if (bytes_sent == 0)
        {
            printf("peer has closed the TCP connection prior to send().\n");
        }
        else if (bytes_sent < sizeof(num_message))
        {
            printf("sent only %d bytes from the required %d.\n", num_message, bytes_sent);
        }
        else
        {
            printf("message was successfully sent.\n");
        }


        if (setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, "reno", 4) == -1)
        {
            printf("setsockopt() failed with error code : %d\n", errno);
            return 1;
        }


        gettimeofday(&start_t_reno, NULL); // start count for second part of the file
        //receive the file from the client using receive_file method
        receive_file(receive_space, client_socket, server_socket_fd);
        gettimeofday(&end_t_reno, NULL); // finish count for second part of the file
        timersub(&end_t_reno, &start_t_reno, &tval_result_reno); // total time for reno
        printf("algo: reno, time: %ld.%06ld, iter num: %d\n", (long int)tval_result_reno.tv_sec, (long int)tval_result_reno.tv_usec, iteration_number);
        // store the time elapsed in a variable
        long int time_elapsed_reno = tval_result_reno.tv_sec * 1000000 + tval_result_reno.tv_usec;
        long int *time_elapsed_reno_p = (long int *)malloc(sizeof(long int));
        *time_elapsed_reno_p = time_elapsed_reno;
        int *reno_param = (int *)malloc(sizeof(int));
        *reno_param = 1;
        enqueue(time_elapsed_reno_p, iteration_number_p, reno_param);
        ///////////////////////////////////////////////////////////////////////////////////////////////////


        // receive answer from client if he wants to proceed and send again of exit
        char ans;
        recv(client_socket, &ans, sizeof(char), 0);
        if (ans == 'N')
        {
            // print out the report
            printf("######################################\n");
            printf("######################################\n");
            printf("the report is \n");
            print_report(iteration_number);
            close(client_socket);
            printf("closing client socket \n");
            break;
        }
        else
        {
            continue;
        }

       }
        free(receive_space);
    }
}


void print_report(int number_of_iterations)
{
    long int avg_cubic = 0;
    long int avg_reno = 0;
    long int avg_total = 0;
    long int number_of_dequeue = 0;

    // dequeue the queue and print out the report
    while (head != NULL)
    {
        if (
            // compare the head->cubic_is_0_reno_is_1 to 0 but remember that it is a pointer
            // so you need to dereference it
            *head->cubic_is_0_reno_is_1 == 0

        )
        {
            avg_cubic += *head->time_in_micro_seconds;
        }
        else if (*head->cubic_is_0_reno_is_1 == 1)
        {
            avg_reno += *head->time_in_micro_seconds;
        }
        avg_total += *head->time_in_micro_seconds;
        dequeue();
        number_of_dequeue++;
    }

    printf("-----------------------\n");
    printf("-----------------------\n");
    printf("the average time for cubic is %ld \n", avg_cubic / number_of_iterations);
    printf("the average time for reno is %ld \n", avg_reno / number_of_iterations);
    printf("the average time for total is %ld \n", avg_total / number_of_dequeue);
}

// a method that receive the file with recv method, receives a receive_space in the size of half of the file, client socket and server socket
void receive_file(char* receive_space, int client_socket, int server_socket_fd)
{
    int bytes_counter = 0;
    int bytes_received;

    //sends file in chunks of 1024 bytes
    while (bytes_counter + CHUNK_SIZE <= FILE_SIZE_IN_BYTES / 2)
    {
        bytes_received = recv(client_socket, receive_space, CHUNK_SIZE, 0);
        if (bytes_received == -1)
        {
            printf("recv failed with error code : %d\n", errno);
            // close the sockets
            close(server_socket_fd);
            close(client_socket);
            exit(1);
        }
        bytes_counter += bytes_received;
    }

    // sends (FILE_SIZE_IN_BYTES/2) % 1024 leftover bytes
    bytes_received = recv(client_socket, receive_space, FILE_SIZE_IN_BYTES / 2 - bytes_counter, 0);
    if (bytes_received == -1)
    {
        printf("recv failed with error code : %d\n", errno);
        // close the sockets
        close(server_socket_fd);
        close(client_socket);
        exit(1);
    }

    printf("bytes received: %d\n", bytes_counter + bytes_received);
}
