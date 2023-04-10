# Computer Networking Course Assigment 3
University assignment 3 for Networking

## For Computer Science B.S.c Ariel University

## Authors
- Orel Dayan
- Shahar Zaidel

## Description
In this project, we compared two tcp congestion control algorithms - Reno and Cubic. We examined their coping in different network conditions. We have simulated different network conditions by using packet loss software.
with the packet loss software, we examined several percentages of packet losses: 0%, 5%, 10%, 15%, 20%.

The code includes a server and a client. The client sends a 2 mega file to the server. He sends the file in two halves. One half uses the Reno algorithm, and the other half uses the Cubic algorithm. Our conclusions are presented in the PDF file. 

The client will be able to send the file as many times as he wants, which will help base conclusions on the performance of each algorithm.

## requierments
the program runs on linux os only - can be on virtual machine or on a full linux enviroment only. 
GCC Compiler
iproute - for simulating packet loss 

## running the program 
compile all files with "make all"
in 2 different terminals, run ./server and ./client

## how to use iprute
a command for setting a packet-loss rate: 
sudo tc qdisc add dev lo root netem loss XX%

a command for changing the packet-loss rate:
sudo tc qdisc change dev lo root netem loss XX%

a command for disabling the packet-loss rate: 
sudo tc qdisc del dev lo root netem
