blockchain_network : main.o graph.o blockchain.o
	gcc main.o graph.o blockchain.o -o blockchain_network

main.o : main.c graph.h blockchain.h
	gcc -c main.c -o $@

graph.o : graph.c blockchain.c graph.h
	gcc -c graph.c -o $@

blockchain.o : blockchain.h blockchain.c
	gcc -c blockchain.c -o $@

.PHONY := run
run: blockchain_network
	./$<












