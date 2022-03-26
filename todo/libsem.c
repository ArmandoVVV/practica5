#include <pthread_utils.h>
#include <libsem.h>

int g = 0;
int g2 = 0;

// Macro que incluye el código de la instrucción máquina xchg
#define atomic_xchg(A,B) 	__asm__ __volatile__(	\
							"   lock xchg %1,%0 ;\n"	\
							: "=ir"  (A)			\
							: "m"  (B), "ir" (A)		\
							);




void initsem(SEMAPHORE *s,int val)
{
	initqueue(&s->queue);
	s->count = val;
}


void waitsem(SEMAPHORE *s){
	int k = 1; 

	while(k != 0){
		atomic_xchg(k, g);
	}

	if(s->count > 0){
		s->count --; 
	}else{
		enqueue(&s->queue,pthread_self());
		s->count--;
		block_thread();
	}
	g = 0;
}

void signalsem(SEMAPHORE *s){
	int k = 1;

	while(k != 0){
		atomic_xchg(k, g2);
	}

	if(s->queue.exit == s->queue.entry){
		s->count++;
		g2 = 0;
		return;
	}

	if(s->count <= 0)
	{
		s->count++;
		unblock_thread(dequeue(&s->queue));
	}
	g2 = 0;
}

