/*Train struct holds direction of the train, id of the train, priority, loading and crossing time*/
struct Train {
	char direction;
	int id;
	int priority;
	int load;
	int cross;

};
typedef struct Train Train;

/*p_Queue struct holds head, tail, size and of course the queue itself*/
struct p_Queue {
	int head;
	int tail;
	int size;
	Train queue[100];

};
typedef struct p_Queue p_Queue;

/*Function Prototypes*/
extern void init(p_Queue *pq, int size);
extern void insert(Train *train, p_Queue *pq);
extern Train pop(p_Queue *pq,int id);
extern void printQ(p_Queue *pq);