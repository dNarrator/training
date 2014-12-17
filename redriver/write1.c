#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<pthread.h>
#include<string.h>
#include<dirent.h>

pthread_t tid[5];
char rd_buf[20];
int fd;
int rc = 0;

void* writefunction();
void* readfunction();
//void* access(void *arg);
//void* listdevices();

typedef struct ele{
	struct ele* next;
	char *key;
	int value;
	int perm_level;
	int maxUser;
}Ele;

typedef struct readele{
	char devname[25];
	int fd;
	int owner;
	int perm_level;
}ReadEle;

int main(int argc, char **argv)
{
	/* Our file descriptor */
	int ref1 = 0;
	int i=0,j=0;
	fd = open("/dev/hello1", O_RDWR);
	if (fd == -1) {
		printf("open failed");
		rc = fd;
		exit(-1);
	}
	else
	{
		printf("opened successfully");
	}


	printf("%s: entered\n", argv[0]);
	/* Open the device */
	Ele *temp;
	//char s[6] = {'h','e','l','l','o'};
//     ref1 = pthread_create(&(tid[i]),NULL,readfunction,NULL);
 //    sleep(5);
/*	while (i < 5)
	{
		ref1 = pthread_create(&(tid[i]), NULL, writefunction,(void *)&s[i]);
        	i++;
        	if(ref1)
		{
			exit(EXIT_FAILURE);
		}
	}	
	for(j=0;j<5;j++)
	{
 		pthread_join(tid[j],NULL);
	}*/
	ref1 = pthread_create(&(tid[0]), NULL, writefunction,(void*)temp);
	sleep(1);
//	ref1 = pthread_create(&(tid[1]),NULL,readfunction,NULL);
	pthread_join(tid[0],NULL);
//	pthread_join(tid[1],NULL);
//	sleep(5);
	close(fd);
	return 0;
}

void* writefunction(void *arg)
{
	char *p; int fdesc=0,ret=0;
	int level,max_users;
	char devname[255];
	char entries[80];
	DIR *dp=NULL;
	int count;
	struct dirent *dirptr=NULL;
	printf("check1\n");
	if(NULL==(dp=opendir("/dev")))
	{
		printf("\n cannot open /dev\n");
		return NULL;
	}
	printf("/dev opened\n");
	while(NULL != (dirptr = readdir(dp)))
	{
		if(!strcmp(dirptr->d_name,".") || !strcmp(dirptr->d_name,".."))
			continue;
		printf("%s\n",dirptr->d_name);
		sprintf(devname,"/dev/%s",dirptr->d_name);
		devname[strlen(dirptr->d_name)+5]='\0';
		fdesc=open(devname,O_RDWR);
		if(fd<0)
			continue;
		//insert(t,devname,fd,(level++)%2,(max_users++)%3);
		Ele elem;
		elem.key=strdup(devname);
		elem.value=fdesc;
		elem.perm_level=(level++)%2;
		elem.maxUser=(max_users++)%3;
		printf("%s\n",elem.key);
		rc = write(fd, &elem, sizeof(elem));
		if (rc == -1) {
			printf("write failed\n");
			close(fd);
			exit(-1);
		}
//		insert(devname,fdesc,(level++)%2,(max_users++)%3);
		printf("%s\t %d\n",devname,fd);
	}
	closedir(dp);
     //p = (char *)arg;
	
	
	//return NULL;
	
}

void* readfunction()
{
	int rc2;
	ReadEle re;
	strcpy(re.devname,"/dev/zero");
	re.fd=0;
	re.owner=4;
	re.perm_level=1;
	rc2 = read(fd, &re ,1);

     if(rc2 == -1){
     		perror("read failed");
 	     close(fd);
     		exit(-1);
     }
        if(re.fd<0)
	{
		printf("The device %s hasn't been added yet, dear %d\n",re.devname,re.owner);
		exit(-1);
	}
	printf("The device has been opened and the file desc is %d, dear %d",re.fd,re.owner);
	
//    	printf("%s: read: stored read entries:%s\n",rd_buf);   	        
}
