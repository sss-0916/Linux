#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>
using std::cout;
using std::endl;

const int n = 4;

int resources = 0;

class BlockQueue{
	public:
		BlockQueue(int cap = 10) : _capacity(cap){
			pthread_mutex_init(&_mutex, NULL);
			pthread_cond_init(&_cond_productor, NULL);
			pthread_cond_init(&_cond_consumer, NULL);
		}

		~BlockQueue(){
			pthread_mutex_destroy(&_mutex);
			pthread_cond_destroy(&_cond_productor);
			pthread_cond_destroy(&_cond_consumer);
		}

		bool BlockQueuePush(int data){
			BlockQueueLock();
			while(BlockQueueFull()){
				ProductorWait();
			}
			_queue.push(data);
			ConsumerWakeUp();
			BlockQueueUnLock();
			return true;
		}

		bool BlockQueuePop(int* data){
			BlockQueueLock();
			while(BlockQueueEmpty()){
				ConsumerWait();
			}
			*data = _queue.front();
			_queue.pop();
			ProductorWakeUp();
			BlockQueueUnLock();
			return true;
		}

	private:
		void BlockQueueLock(){
			pthread_mutex_lock(&_mutex);
		}

		void BlockQueueUnLock(){
			pthread_mutex_unlock(&_mutex);
		}

		bool BlockQueueFull(){
			return (_queue.size() == _capacity);
		}

		bool BlockQueueEmpty(){
			return _queue.empty();
		}

		void ProductorWait(){
			pthread_cond_wait(&_cond_productor, &_mutex);
		}

		void ProductorWakeUp(){
			pthread_cond_signal(&_cond_productor);
		}

		void ConsumerWait(){
			pthread_cond_wait(&_cond_consumer, &_mutex);
		}

		void ConsumerWakeUp(){
			pthread_cond_signal(&_cond_consumer);
		}

	private:
		std::queue<int> _queue;
		int _capacity;
		pthread_mutex_t _mutex;
		pthread_cond_t _cond_productor;
		pthread_cond_t _cond_consumer;
};

void* thr_productor(void* arg){
	BlockQueue* bq = (BlockQueue*)arg;

	while(1){
		cout << "productor put resources: " << resources << endl;
		bq->BlockQueuePush(resources++);
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
		}
	}

	for(i = 0; i < n; ++i){
		ret = pthread_create(&ctid[i], NULL, thr_consumer, (void*)&bq);
		if(ret != 0){
			cout << "consumer thread" << i << " create failed!" << endl;
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
