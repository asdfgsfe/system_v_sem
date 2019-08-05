#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h> 

#include <sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>

#define ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	} while(0)

union semun 
{
	int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

int semid;

int semCreat(key_t key)
{
	int semid;
	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if(semid < 0)
		ERR_EXIT("semid_creat");

	return semid;
}

int semOpen(key_t key)
{
	int semid;
	semid = semget(key, 0, 0);
	if(semid < 0)
		ERR_EXIT("semid_open");		

	return semid;
}

int semRm(int semid)
{
	int ret = semctl(semid, 0, IPC_RMID, 0);
	if(ret < 0)
		ERR_EXIT("semRm");
	
	return 0;
}

int semSetVal(int semid, int val)
{
	union semun su;
	su.val = val;

	int ret = semctl(semid, 0, SETVAL, su);
	if(ret < 0)
		ERR_EXIT("semsetVal");
	
	return 0;
}

int semGetVal(int semid)
{
        int ret = semctl(semid, 0, GETVAL, 0);
        if(ret < 0)
                ERR_EXIT("semGetVal");

        return ret;
}

int sem_p(int semid)
{
	struct sembuf sb_p = {0, -1, 0};
	int ret;
	ret = semop(semid, &sb_p, 1);
	if(ret < 0)
		ERR_EXIT("sem_p");

	return ret;
}
	
int sem_v(int semid)
{
        struct sembuf sb_v = {0, 1, 0};
        int ret;
        ret = semop(semid, &sb_v, 1);
        if(ret < 0)
                ERR_EXIT("sem_v");

        return ret;
}

void print(char op_char)
{
	int pause_time;
	srand(getpid());
	
	int i;
	for(i=0; i<10; i++)
	{
		sem_p(semid);

		printf("%c", op_char);
		fflush(stdout);
		pause_time = rand() % 3;
		sleep(pause_time);
		printf("%c", op_char);
		fflush(stdout);

		sem_v(semid);
		
		pause_time = rand() % 2;
		sleep(pause_time);
	}
}

int main(int argc, char *argv[])
{
	semid = semCreat(IPC_PRIVATE);
	semSetVal(semid, 0);

	pid_t pid;
	pid = fork();
	if(pid < 0)
		ERR_EXIT("fork");
	
	if(pid > 0)
	{
		semSetVal(semid, 1);	
		print('o');
		wait(NULL);
		semRm(semid);
	}
	else
		print('x');

	return 0;
}

¾ºÕù½á¹û£º
gw@ubuntu:~/linux_net_code/ipc/31ipc$ ./print 
ooxxooxxooooxxooxxooxxooxxooxxooxx
gw@ubuntu:~/linux_net_code/ipc/31ipc$ ./print 
ooxooxoxxxoxooxoxooxxxooxoxxxoxooxxoooxx


