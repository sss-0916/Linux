#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	// 创建一个子进程
	pid_t pid = fork();

	if(pid < 0){
		perror("fork");
		return 1;
	} else if(pid > 0){ 
		// 父进程
		printf("parent[%d] is sleeping...\n", getpid());
		// 睡眠60s
		sleep(120);
	} else{
		// 子进程
		printf("child[%d] is begin Z...", getpid());
		// 睡眠5s
		sleep(60);
		return 0;
	}

	return 0;
}
