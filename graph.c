#include "graph.h"

void create_graph(Graph* G, const char* grapheFileName)
{
	FILE *fp = fopen(grapheFileName, "r");

	if (fp != NULL)
	{
		int nb_nodes, nb_arcs, u, v;
		fscanf(fp, "%d %d", &nb_nodes, &nb_arcs);
		G->nb_nodes = nb_nodes;
		G->list_successors = (Node**)malloc(nb_nodes * sizeof(Node*));

		for (int i = 0; i < nb_nodes; i++)
			G->list_successors[i] = NULL;

		for (int i = 0; i < nb_arcs; i++)
		{
			fscanf(fp, "%d %d", &u, &v);
			Node* s = (Node*)malloc(sizeof(Node));
			s->id = v;
			s->weight = 1;
			s->successor = G->list_successors[u];
			G->list_successors[u] = s;
		}
	}
	else printf("File not found.");
	fclose(fp);
}

void print_graph(Graph* G)
{
	for (int i = 0; i < G->nb_nodes; i++)
	{
		printf("Node %d : ", i);
		Node* current = G->list_successors[i];
		while (current != NULL)
		{
			printf("%d, ", current->id);
			current = current->successor;
		}
		printf("\n\n");
	}
}


/* Cette fonction initialise les paramètres de l'algorithme, on choisit un noeud source aléatoire au départ pour mettre le block genesis dans tout les noeuds 

*/

void adapted_chandy_misra_initialize(ParamsAlgoChM* paramsAlgo, Graph* G, int source)
{
	paramsAlgo->G					= G;
	ParamsAlgoChM* P = paramsAlgo;
	paramsAlgo->source = source;
	paramsAlgo->predecessor= (int*) malloc(G->nb_nodes*sizeof(int));
	paramsAlgo->tabNbUnackMessages	= (int*) malloc(G->nb_nodes * sizeof(int));
	paramsAlgo->tabblockchain= (BlockChain**) malloc(G->nb_nodes*sizeof(BlockChain*));  // on cree la liste des adresses de blockchain
	for (int i = 0; i < P->G->nb_nodes; i++)
	{
		P->predecessor[i] = UNDEFINED;  
		P->tabNbUnackMessages[i]  = 0;
		BlockChain* blockch=(BlockChain*) malloc(sizeof(BlockChain)); // on cree la blockchain
		P->tabblockchain[i]=blockch;
		InitializeChain(P->tabblockchain[i]);  // on initialise les blockchains de chaque noeud
	}
	P->predecessor[P->source]		= NONE; 
}


/* Cette fonction ajoute les messages envoyés par les blocks dans la liste de message */


int sendMessageToSuccessors(ParamsAlgoChM* paramsAlgo, MessageList* messageList, enum messageStatus status, int idNode, int* tabvisit)
{
	ParamsAlgoChM* P = paramsAlgo;
	int i = 0;
	Node* outgoing_arc = P->G->list_successors[idNode];
	while (outgoing_arc != NULL)
	{
		if (tabvisit[outgoing_arc->id]==0){
			Message *m = (Message*)malloc(sizeof(Message));
			m->idSender = idNode;
			m->idReceiver = outgoing_arc->id;
			m->status = status;

			if (status == MSG_Block){ // le cas où on envoit un block
				m->block = P->tabblockchain[idNode]->Head; 
				tabvisit[outgoing_arc->id]=1; // tabvisit permet de savoir si l'élement lui a déjà été envoyé
			}
			else
				m->block = NULL; // si le message n'est pas un block on envoit ca
			if(status!=MSG_OverQ){
				addMessageViaHead(messageList,m);
				i++;
			}
		}
		outgoing_arc = outgoing_arc->successor;
	}
	return i;
}



/* Cette fonction est l'algorithme principal, il utiliser les sous-fonctions écrites pour appliquer une version modifié de Chandy-Misra */

void adapted_chandy_misra_algorithm(Graph* G, ParamsAlgoChM* paramsAlgo,int source)
{
		ParamsAlgoChM* P = paramsAlgo;	
		P->source = source;

		for (int i = 0; i < P->G->nb_nodes; i++)
	{
		P->predecessor[i] = UNDEFINED;  // on enleve distance from source et on garde predecessor
		P->tabNbUnackMessages[i]  = 0;
	}

	P->predecessor[P->source]		= NONE; // on enleve distance from source et on garde predecessor

	
	int tabvisit[G->nb_nodes];  // si un noeud a déjà été visité on le saura grâce à ce tableau
	
	for (int h=0;h<G->nb_nodes;h++)
		tabvisit[h]=0;
	
	tabvisit[source]=1;

	MessageList msgList_phI,msgList_phII;

	messageListInitialize(&msgList_phI);
	
	messageListInitialize(&msgList_phII);
	
	paramsAlgo->tabNbUnackMessages[source] = sendMessageToSuccessors(paramsAlgo, &msgList_phI, MSG_Block, source, tabvisit);

	while (msgList_phI.nbMessages!=0) 
	{
		Message *msg	= extractRandomMessage(&msgList_phI);
		sendReceiveMessage(paramsAlgo, &msgList_phI, msg,tabvisit);
		free(msg);
	}
}


/*Cette fonction retire un message de la liste de message et l'envoie à un récepteur*/


void sendReceiveMessage(ParamsAlgoChM* paramsAlgo, MessageList *messageList, Message* message, int* tabvisit)
{
	ParamsAlgoChM* P = paramsAlgo;

	int u = message->idSender;
	int t = message->idReceiver;

		if (t != P->source)
		{
			if (P->tabNbUnackMessages[t] > 0)
			{
				Message mAck;
				mAck.idSender	= t;
				mAck.idReceiver = P->predecessor[message->idReceiver];
				sendReceiveAck(P, &mAck);
			}
			message->block->index=P->tabblockchain[t]->Head->index+1; // ici on ajoute un block à la blockchain du noeud
			message->block->next=P->tabblockchain[t]->Head;
			P->tabblockchain[t]->Head=message->block;
			P->predecessor[t]	=	u;
			P->tabNbUnackMessages[t] +=	sendMessageToSuccessors(P, messageList, MSG_Block, t,tabvisit);

			if (P->tabNbUnackMessages[t] == 0) 
			{
				Message mAck;
				mAck.idSender		= t;
				mAck.idReceiver	= P->predecessor[t];
				sendReceiveAck(P, &mAck);
			}		
		}
		Message mAck;
		mAck.idSender = t;
		mAck.idReceiver = u;
		sendReceiveAck(P, &mAck);
}


/* Cette fonction sert à envoyer un message d'acknowledgment à ses récepteurs */

void sendReceiveAck(ParamsAlgoChM* paramsAlgo, Message* message)
{
	ParamsAlgoChM* P = paramsAlgo;
	P->tabNbUnackMessages[message->idReceiver]--;

	if (P->tabNbUnackMessages[message->idReceiver] == 0 && message->idReceiver != P->source)
	{
		Message m;
		m.idSender = message->idReceiver;
		m.idReceiver = P->predecessor[message->idReceiver];
		sendReceiveAck(P, &m);
	}
}


/*Cette fonction permet d'extraire un message aléatoirement */

Message* extractRandomMessage(MessageList *messageList)
{
	if (messageList->head != NULL)
	{
		Message *current, *previous;
		previous = NULL;
		current  = messageList->head;

		int k = generateRandomInRange(1, messageList->nbMessages);
		int i = 1;

		while (i < k)
		{
			previous = current;
			current = current->next;
			i++;
		}
		if (previous != NULL && current != NULL)
			previous->next = current->next;
		else
			messageList->head = current->next;
		messageList->nbMessages--;
		return current;
	}
	return NULL;
}


/* Cette fonction initialise les listes de messages*/

void messageListInitialize(MessageList *messageList)
{
	messageList->head = NULL;
	messageList->nbMessages	= 0;
}



void addMessageViaHead(MessageList *messageList, Message* newMessage)
{
		newMessage->next = messageList->head;
		messageList->head = newMessage;
		messageList->nbMessages++;
}

int generateRandomInRange(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}


void InsertNewBlock(ParamsAlgoChM* paramsAlgo,int source){
	InsertBlock(paramsAlgo->tabblockchain[source]);
	}
	
int random_node(Graph * G){
	srand(time(NULL)); 
	int r = rand() %G->nb_nodes; 
	return r ;
}


// Cette fonction permet d'avoir une interface pour représenter notre travail*/

void program()
{
  bool quitter = false;
  int choix;
  int i;
  Graph G;
  ParamsAlgoChM paramsAlgo;
  create_graph(&G,"graph.txt");
  int source = random_node(&G);
  adapted_chandy_misra_initialize(&paramsAlgo,&G,source);
  while (!quitter)
  {
    printf("\n");
    printf("****************MENU****************\n\n");
    printf("1: inserez un block\n");
    printf("2: observer la chaine d'un noeud\n");
    printf("3: voir le graph\n");
    printf("4: observer toutes les chaines\n");
    printf("5: quitter\n");
    printf("\n\n");
    printf("choix : ");
    scanf("%d", &choix);
    switch(choix)
    {
      case 1 :
	source = random_node(&G);
        InsertNewBlock(&paramsAlgo,source);
	adapted_chandy_misra_algorithm(&G,&paramsAlgo,source);
        break;
      case 2 :
	printf("choisir un noeud entre 0 et %d\n",G.nb_nodes - 1);
	scanf("%d",&i);
        Print_Blockchain(paramsAlgo.tabblockchain[i]);
        break;
      case 3 :
        print_graph(&G);
        break;
      case 5 :
        quitter = true;
        break;
      case 4 :
      	for (int i = 0; i<G.nb_nodes;i++){
		printf("la chaine du noeud %d est :\n\n",i);
		Print_Blockchain(paramsAlgo.tabblockchain[i]);
		printf("******************************************** \n\n");
	}
      
    }
  }
}




