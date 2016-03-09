#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "priority_queue.h"
/*Implementation of an Array based Priority Queue for Train structs.
  The header file priority_queue.h defines the Train and p_Queue structs used*/

/*/Initialise Priority queue head,tail and size. Head and tail ==-1  means empty list*/
void init(p_Queue *pq,int size) {
	pq->size = size;
	pq->head=pq->tail=-1;
}

/*Insert by Train priority into the queue, if its the first element then, increment head and tail, otherwise just increment head. */
void insert(Train *train, p_Queue *pq) {
	if((pq->tail)>=99) {
		printf("Queue Overflow\n");
		return;
	}
	else if((pq->tail==-1)&&(pq->head==-1)) {
		pq->head++;
		pq->tail++;
		pq->queue[0]=*train;
		return;
	}
	else {
		int i; 
		int j;
		
		for(i=0;i<=pq->tail;i++) {	
			if((train->priority)>(pq->queue[i].priority)) {
				for(j=pq->tail+1;j>i;j--) {
					pq->queue[j]=pq->queue[j-1];
				}
				pq->queue[i] = *train;
				pq->tail++;
				return;
			}
		}
		pq->queue[i]=*train;
		pq->tail++;
	}
}

/*In order to meet the assignment specification, we implemented pop by id, which pops the element with specified id.
 So, its not really a queue any more, more so a partial ordering of the data*/
Train pop(p_Queue *pq, int id) {
	if((pq->tail==-1)&&(pq->head==-1)) {
		printf("Priority Queue is empty\n");
		return;
	}
	
	int flag = 0;
	int i;
	Train temp = pq->queue[0];
	for(i=0;i<=pq->tail;i++){
		if(pq->queue[i].id == id) {
			temp=pq->queue[i];
			flag =1;
		}
		if(flag==1)
			pq->queue[i]=pq->queue[i+1];
	}
	pq->tail--;
	
	if(pq->tail==-1) {
		pq->head==-1;
	}
	
	return temp;
}

/*Method used for testing which simply prints out the queue's elements*/
void printQ(p_Queue *pq) {
	int i;
	for(i=0;i<=pq->tail;i++) {
		printf("%d \n",pq->queue[i].id);
	}
}


