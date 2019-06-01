#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int noodles = 1;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* thr_restaurant(void* arg){
	while(1){
		pthread_mutex_lock(&mutex);
		if(noodles == 1){
			pthread_cond_wait(&cond, &mutex);
		}
		sleep(1);
		printf("noodles sell out!\n");
		++noodles;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* thr_customer(void* arg){
	while(1){
		pthread_mutex_lock(&mutex);
		if(noodles == 0){
			pthread_cond_wait(&cond, &mutex);
		}
		sleep(1);
		printf("eatting noodles!\n");
		--noodles;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
	}
	return NULL;
}

int main(){
	pthread_t tid1, tid2;
	int ret;

	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	ret = pthread_create(&tid1, NULL, thr_restaurant, NULL);
	if(ret != 0){
		printf("restaurant error!");
		exit(-1);
	}

	ret = pthread_create(&tid2, NULL, thr_customer, NULL);
	if(ret != 0){
		printf("customer error!\n");
		exit(-1);
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	return 0;
}
