

#ifndef GRAPH_H

#define GRAPH_H
#define INFINITY 100  

#include "blockchain.h"
#define UNDEFINED -1
#define NONE -2

enum messageStatus {MSG_Block, MSG_Ack,MSG_OverQ}; // on change MSG_Length en MSG_Block

typedef struct _node
{
	int	id;
	int	weight; 
	struct	_node* successor;
} Node;

typedef struct
{
	int	nb_nodes;
	Node** list_successors;
} Graph;


// on a pas besoin de cette structure

//typedef struct
//{
//	int	distanceFromSource;
//	int predecessor;
//} ShortPath;

typedef struct
{
	int source;
	Graph*	G;
	int*	tabNbUnackMessages;
	BlockChain** 	tabblockchain; // on ajoute les blockchain de chaque element ici
	int*	predecessor; // on enlève tabshortpath et on garde que la table des prédécesseurs
} ParamsAlgoChM;

typedef struct _message
{
	int idSender;
	int idReceiver;
	Block*	block; // on enlève data et on met un block
	enum messageStatus	status;
	struct _message *next;
} Message;

typedef struct
{
	int		nbMessages;
	Message *head;
} MessageList;

/******************************************************************************
Ces fonctions sont créer et lire les graphes
******************************************************************************/
void create_graph(Graph* G, const char* grapheFileName);
void print_graph(Graph* G);

/******************************************************************************
Ces fonction correspondent à l'application de la version modifié de Chandy-Misra pour mettre en place le réseau.
******************************************************************************/
void adapted_chandy_misra_algorithm(Graph* G,ParamsAlgoChM* paramsAlgo, int source);
void adapted_chandy_misra_initialize(ParamsAlgoChM* paramsAlg, Graph* G, int source);
void messageListInitialize(MessageList* list);
void addMessageViaHead(MessageList *messageList, Message* newMessage);
void sendReceiveMessage(ParamsAlgoChM* paramsAlgo, MessageList *messageList, Message* message, int* tabvisit);
int sendMessageToSuccessors(ParamsAlgoChM* paramsAlgo, MessageList* messageList,enum messageStatus status, int idNode, int* tabvisit);
void sendReceiveAck(ParamsAlgoChM* paramsAlgo,  Message* message);
void sendReceiveOverMinus(ParamsAlgoChM* paramsAlgo, MessageList *messageList, Message* message);
void sendReceiveOverQ(ParamsAlgoChM* paramsAlgo, MessageList *messageList, Message* message);
Message* extractRandomMessage(MessageList *messageList);
int generateRandomInRange(int lower, int upper);
void InsertNewBlock(ParamsAlgoChM* paramsAlgo,int source);
int random_node(Graph * G);
void program();
#endif 
