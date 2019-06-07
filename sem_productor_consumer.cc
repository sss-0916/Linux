#include <iostream>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
using std::cout;
using std::endl;

const int n = 4;
int resources = 0;

class BlockQueue{
	public:
		BlockQueue(int cap = 10):_queue(10), _capacity(cap),
		_write_step(0), _read_step(0){
			sem_init(&_sem_data, 0, 0);
			sem_init(&_sem_idle, 0, cap);
			sem_init(&_sem_lock, 0, 1);
		}

		~BlockQueue(){
			sem_destroy(&_sem_data);
			sem_destroy(&_sem_idle);
			sem_destroy(&_sem_lock);
		}

		bool BlockQueuePush(int data){
			ProductorWait();
			BlockQueueLock();
			_queue[_write_step] = data;
			_write_step = (_write_step + 1) % _capacity;
			BlockQueueUnLock();
			ConsumerWakeUp();

			return true;
		}

		bool BlockQueuePop(int* data){
			ConsumerWait();
			BlockQueueLock();
			*data = _queue[_read_step];
			_read_step = (_read_step + 1) % _capacity;
			BlockQueueUnLock();
			ProductorWakeUp();

			return true;
		}

	private:
		void BlockQueueLock(){
			sem_wait(&_sem_lock);
		}

		void BlockQueueUnLock(){
			sem_post(&_sem_lock);
		}

		void ProductorWait(){
			sem_wait(&_sem_idle);
		}

		void ProductorWakeUp(){
			sem_post(&_sem_idle);
		}

		void ConsumerWait(){
			sem_wait(&_sem_data);
		}

		void ConsumerWakeUp(){
			sem_post(&_sem_data);
		}

	private:
		std::vector<int> _queue;
		int _capacity;
		int _write_step;
		int _read_step;
		sem_t _sem_data;
		sem_t _sem_idle;
		sem_t _sem_lock;
};

void* thr_productor(void* arg){
	BlockQueue* bq = (BlockQueue*)arg;

	while(1){
		bq->BlockQueuePush(resources);
		cout << "productor put data: " << resources << endl;
		++resources;
		sleep(1);
	}

	return NULL;
}

void* thr_consumer(void* arg){
	BlockQueue* bq = (BlockQueue*)arg;

	while(1){
		int data;
		bq->BlockQueuePop(&data);
		cout << "consumer get data: " << data << endl;
		sleep(1);
	}

	return NULL;
}

int main(){
	pthread_t ptid[n], ctid[n];
	BlockQueue bq;
	int i, ret;

	for(i = 0; i < n; ++i){
		ret = pthread_create(&ptid[i], NULL, thr_productor, (void*)&bq);
		if(ret != 0){
			cout << "productor thread" << i << " create failed!" << endl;
			exit(-1);
		}
	}

	for(i = 0; i < n; ++i){
		ret = pthread_create(&ctid[i], NULL, thr_consumer, (void*)&bq);
		if(ret != 0){
			cout << "consumer thread" << i << " create failed!" << endl;
			exit(-1);
		}
	}

	for(i = 0; i < n; ++i){
		pthread_join(ptid[i], NULL);
	}

	for(i = 0; i < n; ++i){
		pthread_join(ctid[i], NULL);
	}

	return 0;
}
