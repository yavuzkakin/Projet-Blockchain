#include "blockchain.h"


int VerifyHash (unsigned long hash){ 
	int a=99;// ce sont deux valeurs aléatoires que l'on choisit pour le départ
	int b=98;
	int resu=0;
	while((hash!=0)){
		b=a;
		a=hash%10;
		hash=hash/10;
		if ((a==b)&&(a==9))
			resu=1;
	}
	return resu;
}



unsigned long HashGenerator (Block* block){
	return (strlen(block->author)*128*128*128*128 + strlen(block->message)*128*128*128 + block->timestamp*128*128 + block->POW*128 + block->previous_hash*1);
}

unsigned long POWGenerator (){
	srand(time(NULL));
	unsigned long r = rand() % 1000000;
	return (r);
}


Block* CreateBlock(unsigned long index, char author[20],unsigned int timestamp,char message[20],unsigned long previous_hash){
	Block* block= (Block*) malloc(sizeof(Block));
	block->index=index;
	strcpy(block->author,author);
	block->timestamp=timestamp;
	strcpy(block->message,message);
	block->POW=POWGenerator();
	block->previous_hash=previous_hash;
	block->next=NULL;
	block->hash=HashGenerator(block);
	while (VerifyHash(block -> hash) == 0){
		block->POW=POWGenerator();
		block->hash=HashGenerator(block);
	}
	
	return block;
}

void InitializeChain (BlockChain * chain){
	chain -> lenght = 1;
	Block* h;
	h=CreateBlock(1,"GENESIS BLOCK",0,"GENESIS BLOCK",0);
	chain->Head=h;
}

void InsertBlock (BlockChain * chain){
	unsigned int timestamp;
	char author[20];
	char message[20];
	printf("entrer le nom de votre auteur (20 caracteres max y inclus les espaces !) :\n");
	scanf("%s",author);
	printf("\nentrer votre message (20 caracteres max y inclus les espaces !) :\n");
	scanf("%s",message);
	printf("entrer la date sous la forme JJMMAAAA (tout attacher):\n\n");
	scanf("%d",&timestamp);
	Block * block = CreateBlock(chain->lenght + 1,author,timestamp,message,chain -> Head -> hash);
	chain -> lenght = chain -> lenght + 1;
	block->next=chain->Head;
	chain -> Head = block;
	
}

void Print_Block(Block *block){
	printf("\n index du message: %ld \n",block->index);
	printf("le nom de l'auteur est : %s\n",block->author);
	printf("timestamp: %ld\n",block->timestamp);
	printf("le message est : %s \n",block->message);
	printf("POW:%ld\n",block->POW);
	printf("hash:%ld\n",block->hash);
	printf("previous hash: %ld \n\n", block->previous_hash);
}

void Print_Blockchain(BlockChain * chain){
	Block * current = chain -> Head;
	while (current !=NULL){
		Print_Block(current);
		current=current->next;
	}
}


