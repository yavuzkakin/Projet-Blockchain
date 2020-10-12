#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define genesis_hash 0

typedef struct _Block {
	unsigned long index;
	char author [20];
	unsigned long timestamp; //JourMoisAnnee
	char message[20];
	unsigned long  POW;
	unsigned long hash;
	unsigned long previous_hash;
	struct _Block * next;
}Block;

typedef struct _Chain{
	Block * Head;
	int lenght;
}BlockChain;


int VerifyHash (unsigned long hash);
unsigned long HashGenerator (Block* block);
unsigned long POWGenerator ();
Block* CreateBlock(unsigned long index, char author[20],unsigned int timestamp,char message[20],unsigned long previous_hash);
void InitializeChain (BlockChain * chain);
void InsertBlock (BlockChain * chain);

void Print_Block(Block *block);
void Print_Blockchain(BlockChain * chain);
