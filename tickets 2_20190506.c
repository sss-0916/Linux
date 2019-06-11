#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;

int tickets = 10;

void* thr_start(void* arg){
	while(1){
		pthread_mutex_lock(&mutex);
		if(tickets > 0){
			usleep(1000);
			printf("ticket scalper %d get a ticket[%d]!\n", (int)arg + 1, tickets);
			--tickets;
		}
		else{
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(){
	pthread_mutex_init(&mutex, NULL);

	pthread_t tid[4];
	int i = 0, ret;

	for(; i < 4; ++i){
		ret = pthread_create(&tid[i], NULL, thr_start, (void*)i);
		if(ret != 0){
			printf("pthread create error!\n");
			exit(-1);
		}
	}

	for(i = 0; i < 4; ++i){
		pthread_join(tid[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	return 0;
}
