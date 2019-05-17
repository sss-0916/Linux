#include <stdio.h>
#include <signal.h>

void myHandler(int signo){
	printf("hehe...\n");
}

int main(){
	struct sigaction act;

	act.sa_handler = myHandler;
	act.sa_flags = 0;

	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);
	sigaction(SIGRTMIN+1, &act, NULL);
	sigaction(SIGKILL, &act, NULL);
	sigaction(SIGSTOP, &act, NULL);

	sigset_t set;

	sigemptyset(&set);

	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGTSTP);
	sigaddset(&set, SIGRTMIN+1);
	sigaddset(&set, SIGKILL);
	sigaddset(&set, SIGSTOP);

	sigprocmask(SIG_BLOCK, &set, NULL);

	printf("Press ENTER to unblock!\n");
	getchar();

	sigset_t pending;

	sigpending(&pending);

	int i = 0;
	for(i = 1; i <= 64; ++i){
		if(sigismember(&pending, i)){
			printf("1 ");
		}
		else{
			printf("0 ");
		}
	}
	printf("\n");

	sigprocmask(SIG_UNBLOCK, &set, NULL); 

	return 0;
}
