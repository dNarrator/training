#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
//#include<stdlib.h>
//#include<unistd.h>
//#include<dirent.h>
//#include<sys/stat.h>
//#include<sys/types.h>
//#include<fcntl.h>
//#include<string.h>



#define HELLO_MAJOR 234
#define INIT_SIZE 128
#define GROWTH_FACT 2
#define MAX_LOAD_FACT 1
#define MULT 37

struct LinkedList {
	char ch;
	struct LinkedList *next;
};

typedef struct ele{
	struct ele* next;
	char *key;
	int value;
	int perm_level;
	int maxUser;
	int currNumUser;
}Ele;

typedef struct table{
	int total;
	int size;
	struct ele **list;
}Table;

typedef struct LinkedList LinkedList;

static LinkedList *head = NULL;
static LinkedList *tail = NULL;
struct kmem_cache *list_cache;
struct semaphore rsm,wsm;
static DEFINE_MUTEX(mutex);

static int debug_enable = 0;
module_param(debug_enable, int, 0);
MODULE_PARM_DESC(debug_enable,"Enable module debug mode.");

static int driver_count = 0;
module_param(driver_count,int,0);
MODULE_PARM_DESC(driver_count,"Number of times driver is used");


static int count1 = 0;
module_param(count1, int, 0);
MODULE_PARM_DESC(count1, "stores number of entries");



struct file_operations hello_fops;

static Table *t=NULL;

//int insert(Table *t,const char* key, int val,int perm_level,int maxUser);
int insert(const char* key, int val,int perm_level,int maxUser);
int create();
//Table* create()
int create()
{
	//Table *t;
	int i,fd,level,max_users;
	fd=0;
	level=0;
	max_users=0;
	char devname[255];
	DIR *dp=NULL;
	struct dirent *dirptr=NULL;

	t=(Table*)malloc(sizeof(t));
	if(t==NULL)
		return NULL;
	t->size=INIT_SIZE;
	t->total=0;
	t->list=(Ele**)malloc(sizeof(Ele*)*t->size);
	if(t->list==NULL)
	{
		t=NULL;
		return 1;
	}
	for(i=0;i<t->size;i++)
	{
		t->list[i]=0;
//		printf("%d \t %x\n",i,t->list[i]);
	}
	if(NULL==(dp=opendir("/dev")))
	{
		printk("\n cannot open /dev\n");
		return 1;
	}
	while(NULL != (dirptr = readdir(dp)))
	{	
		if(!strcmp(dirptr->d_name,".") || !strcmp(dirptr->d_name,".."))
			continue;
		//printf("%s\n",dirptr->d_name);
		sprintf(devname,"/dev/%s",dirptr->d_name);
		devname[strlen(dirptr->d_name)+5]='\0';
		fd=open(devname,O_RDWR);
		if(fd<0)
			continue;
		//insert(t,devname,fd,(level++)%2,(max_users++)%3);
		insert(devname,fd,(level++)%2,(max_users++)%3);
		printk("%s\t %d\n",devname,fd);
	}
	closedir(dp);
//	printf("Created\n");
	return 0;
}

static unsigned long hash_func(const char *s)
{
	unsigned const char *us;
	unsigned long h=0;
	h=0;
	for(us = (unsigned const char*)s;*us;us++)
	{
//		printf("us %d h %ld",*us,h);
		h=(h*MULT) + (*us);
	}
	return h;
}

//int insert(Table *t,const char* key, int val,int perm_level,int maxUser)
int insert(const char* key, int val,int perm_level,int maxUser)
{
	Ele *elem;
	unsigned long hash;

	if(!key || !val)
	{
		printk("Invalid parameter\n");
		return 1;
	}

	elem=malloc(sizeof(Ele));

	if(!elem)
	{
		printk("Not enough mem\n");
		return 2;
	}
	elem->key=strdup(key);
	elem->value=val;
	elem->perm_level=perm_level;
	elem->maxUser=maxUser;
	elem->currNumUser=maxUser;

	hash=hash_func(key) % t->size;
//	printf("hash %ld\t size %d\n",hash,t->size);

	if(t->list[hash]==0)
		elem->next=0;	

	elem->next = t->list[hash];
	
	t->list[hash]= elem;

	t->total++;
//printf("inserted\n");
}

//Ele* search(Table *t,const char* key)
Ele* search(const char* key)
{
	Ele *elem;
	for(elem=t->list[hash_func(key) % t->size];elem!=0;elem=elem->next)
	{
	//	printf("%s\n",elem->value);
		if(!strcmp(elem->key,key))
		{
	//		printf("found\n");
			return elem;
		}
	}
	return 0;
}
	
//void listall(Table *t)
void listall()
{
	int i;
	Ele *elem;
	printk("The devices that can be accessed are listed below\n");
	for(i=0;i<t->size;i++)
	{
		for(elem=t->list[i];elem!=0;elem=elem->next)
		{
			printk("%s \t %d \n",elem->key,elem->perm_level);
		}
	}
}

//int devAccess(Table *t,const char *key,int perm_level)
int devAccess(const char *key,int perm_level)
{
	Ele *temp;
	//temp=search(t,key);
	temp=search(key);
	if(perm_level<temp->perm_level)
	{
		printk("Cannot be accessed by you\n");
		return -1;
	}
	return temp->value;
}


static int hello_read_proc(char *buf,char **start,off_t offset,int count,int *eof)
{
	int cnt = 0;
	cnt+=sprintf(buf,"Number of device reads:%d\n",driver_count);
	*eof=1;
	cnt+= sprintf(buf+cnt, "Number of entries in storage: %d", count1);

	return cnt;
}

static int hello_open(struct inode *inode, struct file *file){
	printk("hello_open: successful\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file){
	printk("hello_release: successful\n");
	return 0;
}

static ssize_t hello_read(struct file *file, char *buf, size_t count, loff_t *ptr)
{

	down_interruptible(&rsm);
	mutex_lock(&mutex);
	LinkedList *temp;
	driver_count++;
	temp = tail;
	count += sprintf(buf+count, "%c ", tail->ch);
	tail = tail->next;
	count1--;
	kmem_cache_free(list_cache,temp);

	printk("hello_read:driver reads %d times\n",driver_count);
	printk("hello_read: returning zero bytes\n");
	mutex_unlock(&mutex);
	up(&wsm);
	return 0;
}

static ssize_t hello_write(struct file *file, const char *buf, size_t count, loff_t *ppos){
	down_interruptible(&rsm);
	mutex_lock(&mutex);
     LinkedList *item;
     
	item = kmem_cache_alloc(list_cache, GFP_KERNEL);
     item->ch = *buf;
     if(head==NULL && tail ==NULL){
          head=item;
          tail=item;
          item->next=NULL;
     }  
     else{
          head->next=item;
          head=item;
          item->next=NULL;
        }

	count1++;
	driver_count++;
	printk("hello_write: accepting %c bytes\n",item->ch);
	mutex_unlock(&mutex);
	up(&rsm);	
	return 0;
}

static int hello_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("hello_ioctl: cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}

static int __init hello_init(void)
{
	int ret;
	struct proc_dir_entry *dir;

	printk("Hello Example Init - debug mode is %s\n", debug_enable ? "enabled" :"disabled");
	ret = register_chrdev(HELLO_MAJOR, "hello", &hello_fops);
	if(ret < 0){
		printk("Error registering hello device\n");
		goto hello_fail1;
	}
	printk("Hello: registered module succesfully! \n");
	printk("Hello Init: Proc is creating\n");
	dir = create_proc_read_entry	("hello",0,NULL,hello_read_proc,NULL);
	list_cache=kmem_cache_create("cache",sizeof(Element),0,SLAB_HWCACHE_ALIGN,NULL);
	sema_init(&wsm,1);
	sema_init(&rsm,1);
	create();
	return 0;

	hello_fail1:
	return ret;
}

static void __exit hello_exit(void)
{
	if(list_cache){
 		kmem_cache_destroy(list_cache);
	}
	printk("Hello Example\n");
	printk("Removing proc entries\n");
	remove_proc_entry("hello",NULL);
	printk("Successfully removed proc\n");
	unregister_chrdev(HELLO_MAJOR,"hello");
}

struct file_operations hello_fops = {
owner:	THIS_MODULE,
read:	hello_read,
write:	hello_write,
open:	hello_open,
release:	hello_release,
listall:	listall,
devAccess:	devAccess,
};


module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("Varun Joshi");
MODULE_DESCRIPTION("Hello World Example");
MODULE_LICENSE("GPL");
