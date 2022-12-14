all: receiver sender

receiver: 
	gcc -o receiver receiver.c -Wall

sender: 
	gcc -o sender sender.c -Wall

clean:
	rm -f receiver sender