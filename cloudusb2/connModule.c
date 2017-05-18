#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>


//#include <sys/types.h>
#include <linux/signal.h>
//#include <linux/unistd.h>
#include <linux/wait.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/pid_namespace.h>
#include <asm/siginfo.h>    //siginfo
#include <linux/sched.h> // timeout
#include <linux/rcupdate.h> //rcu_read_lock

#define INIT 0
#define RETURN_FILE 1

struct module_init{
    int pid;
    unsigned int amount;
    loff_t file_offset;
};

struct return_file{
    unsigned char *buf;
    int nread;
};

MODULE_LICENSE("GPL");

/********************************************************/

static int cloud_open(struct inode *inode, struct file *file);
static long cloud_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int cloud_release(struct inode *inode, struct file *filp);
//static int send_signals(void);


const struct file_operations cloud_operations =
{
    open: 		cloud_open,
    unlocked_ioctl: 	cloud_ioctl,
    release: cloud_release
};
int cloud_open(struct inode *inode, struct file *file)
{
    printk(KERN_ALERT "CloudUSB file open function called\n");
    return 0;
}


//extern loff_t file_offset;
//extern unsigned int amount;
//extern int cloud_flag;
//extern char *buf;
//extern ssize_t nread;

//struct block_request request;

int cloud_flag = 1;
unsigned int		amount = 0;
loff_t			file_offset = 0;
ssize_t			nread = 0;

char buff[16384];

EXPORT_SYMBOL(amount);
EXPORT_SYMBOL(file_offset);
EXPORT_SYMBOL(nread);
EXPORT_SYMBOL(cloud_flag);
EXPORT_SYMBOL(buff);

//EXPORT_SYMBOL(send_signals);

struct module_init *inits;
struct return_file *files;

struct task_struct *t;
struct siginfo info;
int user_pid;

long cloud_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk(KERN_ALERT "CloudUSB ioctl called\n");
    switch (cmd)
    {
        case INIT:
            printk(KERN_ALERT "CloudUSB ioctl get INIT\n");
            inits = (struct module_init *)(arg);
            user_pid = inits->pid;
            memset(&info, 0 ,sizeof(struct siginfo));
            info.si_signo = SIGCONT;
            info.si_code = SI_QUEUE;
            rcu_read_lock();
            //t = find_task_by_vpid(user_pid);
            t = pid_task(find_vpid(user_pid), PIDTYPE_PID);
            rcu_read_unlock();
            if(t == NULL){
                printk(KERN_ALERT "CloudUSB no such pid\n");
                return -ENODEV;
            }
            printk(KERN_ALERT "CloudUSB init success\n");
            cloud_flag = 0;
//          send_sig_info(SIGCONT, &info, t);
            break;
        case RETURN_FILE:
            printk(KERN_ALERT "CloudUSB ioctl get RETURN_FILE\n");
            files = (struct return_file *)(arg);
            //buff = files->buf;
            printk(KERN_ALERT "CloudUSB RETURN_FILE files->buf: ");
            int i;
            for(i=0;i<nread;i++){
                printk(KERN_CONT "%02x ", files->buf[i]);
            }
            printk(KERN_ALERT "\n");
            strncpy(buff, files->buf, files->nread);
            printk(KERN_ALERT "CloudUSB ioctl get RETURN_FILE1\n");
            nread = files->nread;
            printk(KERN_ALERT "CloudUSB ioctl get RETURN_FILE2\n");
            cloud_flag = 0;
            break;
    }
    printk(KERN_ALERT "CloudUSB before loop\n");
    while(!cloud_flag){schedule_timeout_uninterruptible(0.001*HZ);} // 블록요청 들어올때까지 기다림.
    printk(KERN_ALERT "CloudUSB after loop1\n");
    inits->amount = amount;
    printk(KERN_ALERT "CloudUSB after loop2\n");
    inits->file_offset = file_offset;
    send_sig_info(SIGCONT, &info, t); // 필요한정보 구조체에 넣은후 블록요청 받았다고 유저프로그램에 알려주었다.
    return 0;
}

//static int send_signals(void){
//    inits->amount = amount;
//    inits->file_offset = file_offset;
//    send_sig_info(SIGCONT, &info, t); // 필요한정보 구조체에 넣은후 블록요청 받았다고 유저프로그램에 알려주었다.
//    return 0;
//}

int cloud_release(struct inode *inode, struct file *filp) {
    printk (KERN_ALERT "Inside close \n");
    return 0;
}

/**********************************************************/
static struct cdev *cloud_cdev;
static dev_t dev;
int init_modules(void)
{
    int ret;
    dev = MKDEV(235, 0);
    ret = register_chrdev_region(dev, 1, "CloudUSB");
    cloud_cdev = cdev_alloc();
    cloud_cdev->ops = &cloud_operations;
    cloud_cdev->owner = THIS_MODULE;
    cdev_init(cloud_cdev, &cloud_operations);
    cdev_add(cloud_cdev, dev, 1);
    
    return 0;
}

void cleanup_modules(void)
{
    cdev_del(cloud_cdev);
    unregister_chrdev_region(235, 1);
    printk(KERN_ALERT "CloudUSB cleanup\n");
}

module_init(init_modules);
module_exit(cleanup_modules);

