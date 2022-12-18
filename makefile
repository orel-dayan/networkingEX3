all:myqueue receiver sender server

server:
	gcc -o receiver receiver.o myqueue.o -Wall 	

receiver:
	gcc receiver.c -c -Wall 

sender:
	gcc -o sender sender.c -Wall 

myqueue:
	gcc myqueue.c -c -Wall 

clean:
	rm -f *.o sender receiver

