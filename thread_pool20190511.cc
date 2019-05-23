#include <iostream>
#include <queue>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
using std::cout;
using std::endl;

#define MAX_THR 5
#define MAX_QUE 10
#define TASK_NUM 10

typedef bool (*task_callback)(int data);

bool deal_data(int data){
	srand(time(NULL));
	int n = rand() % 5;
	cout << "thread: " << pthread_self() << ". deal data: " 
		<< data << ". sleep: " << n << "secs" << endl;
	sleep(n);
	return true;
}

class Task{
	public:
		void SetTask(int data, task_callback handler){
			_data = data;
			_handler = handler;
		}

		bool Run(){
			return _handler(_data);
		}
	private:
		int _data;
		task_callback _handler;
};

class ThreadPool{
	public:
		ThreadPool(){
		}

		~ThreadPool(){
			pthread_mutex_destroy(&_mutex);
			pthread_cond_destroy(&_cond_productor);
			pthread_cond_destroy(&_cond_consumer);
		}

		bool ThreadPoolInit(int max_thr = MAX_THR, int max_que = MAX_QUE){
			_quit_flag = false;
			_max_thr = max_thr;
			_cur_thr = max_thr;
			_capacity = max_que;

			pthread_mutex_init(&_mutex, NULL);
			pthread_cond_init(&_cond_productor, NULL);
			pthread_cond_init(&_cond_consumer, NULL);

			int i, ret;
			pthread_t tid;
			for(i = 0; i < _max_thr; ++i){
				ret = pthread_create(&tid, NULL, thr_start, (void*)this);
				if(ret != 0){
					cout << "thread create failed!" << endl;
					return false;
				}
				pthread_detach(tid);
			}

			return true;
		}

		bool PushTask(Task& task){
			if(_quit_flag == true){
				return false;
			}
			QueueLock();
			while(QueueFull()){
				ProductorWait();
			}
			_queue.push(task);
			ConsumerWakeUp();
			QueueUnLock();
			return true;
		}

		void ThreadPoolQuit(){
			if(_quit_flag != true){
				_quit_flag = true;
			}
			while(_cur_thr > 0){
				ConsumerWakeUpAll();
				sleep(1);
			}
			return;
		}

		void PopTask(Task* task){
			*task = _queue.front();
			_queue.pop();
		}

	private:
		void QueueLock(){
			pthread_mutex_lock(&_mutex);
		}

		void QueueUnLock(){
			pthread_mutex_unlock(&_mutex);
		}

		void ConsumerWait(){
			if(_quit_flag == true){
				pthread_mutex_unlock(&_mutex);
				cout << "thread: " << pthread_self() << endl;
				--_cur_thr;
				pthread_exit(NULL);
			}
			pthread_cond_wait(&_cond_consumer, &_mutex);
		}

		void ConsumerWakeUp(){
			pthread_cond_signal(&_cond_consumer);
		}

		void ConsumerWakeUpAll(){
			cout << "Wake Up" << endl;
			pthread_cond_broadcast(&_cond_consumer);
		}

		void ProductorWait(){
			pthread_cond_wait(&_cond_productor, &_mutex);
		}

		void ProductorWakeUp(){
			pthread_cond_signal(&_cond_productor);
		}

		bool QueueFull(){
			return _capacity == _queue.size();
		}

		bool QueueEmpty(){
			return _queue.empty();
		}

		static void* thr_start(void* arg){
			ThreadPool* pool = (ThreadPool*)arg;
			while(1){
				pool->QueueLock();
				while(pool->QueueEmpty()){
					pool->ConsumerWait();
				}
				Task task;
				pool->PopTask(&task);
				pool->ProductorWakeUp();
				pool->QueueUnLock();
				task.Run();
			}
			return NULL;
		}

	private:
		int _max_thr;
		int _cur_thr;
		int _quit_flag;
		int _capacity;
		std::queue<Task> _queue;
		pthread_mutex_t _mutex;
		pthread_cond_t _cond_productor;
		pthread_cond_t _cond_consumer;
};

int main(){
	ThreadPool pool;

	pool.ThreadPoolInit();

	Task task[TASK_NUM];
	int i = 0;

	for(; i < TASK_NUM; ++i){
		task[i].SetTask(i, deal_data);
		pool.PushTask(task[i]);
	}

	pool.ThreadPoolQuit();

	return 0;
}
