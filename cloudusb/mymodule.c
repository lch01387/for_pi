#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

//#include <sys/types.h>
#include <linux/signal.h>
//#include <linux/unistd.h>
#include <linux/wait.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/pid_namespace.h>
#include <asm/siginfo.h>    //siginfo
#include <linux/rcupdate.h> //rcu_read_lock

#define INIT 0
#define RETURN_FILE 1

MODULE_LICENSE("GPL");

typedef struct FILES{
    int pid;
    int *block_require_p;
} fileS;

/********************************************************/

static int cloud_major = -1;
static int cloud_open(struct inode *inode, struct file *file);
static long cloud_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations cloud_operations =
{
    .unlocked_ioctl = 	cloud_ioctl,
    .open = 		cloud_open,
};
static int cloud_open(struct inode *inode, struct file *file)
{
    printk(KERN_ALERT "CloudUSB file open function called\n");
    return 1;
}
static long cloud_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    extern int test_variable;
    struct task_struct *t;
    struct siginfo info;
    fileS *files;
    int user_pid;
    int ret;
    
    printk(KERN_ALERT "CloudUSB ioctl called\n");
    switch (cmd)
    {
        case INIT:
            printk(KERN_ALERT "CloudUSB ioctl get INIT\n");
            printk(KERN_ALERT "CloudUSB arg %lu\n", arg);
            printk(KERN_ALERT "CloudUSB arg %p\n", (fileS *)(arg));
            files = (fileS *)(arg);
            printk(KERN_ALERT "CloudUSB pid:%d, require:%d\n", files->pid, *files->block_require_p);
            *files->block_require_p = test_variable; // test_variable = 150
            user_pid = files->pid;
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
            ret = send_sig_info(SIGCONT, &info, t);
            break;
        case RETURN_FILE:
            printk(KERN_ALERT "CloudUSB ioctl get RETURN_FILE\n");
            break;
    }
    return 0;
}

/**********************************************************/

int init_module(void)
{
    unregister_chrdev(231, "CloudUSB");
    cloud_major = register_chrdev(235, "CloudUSB", &cloud_operations);
    if (cloud_major < 0) {
        printk(KERN_ERR "CloudUSB - cannot register device\n");
        return cloud_major;
    }else{
        printk(KERN_ERR "Cloud chdev successfully registerd, major num\n");
    }
    printk("<1>Hello World 1.\n");
    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(235, "CloudUSB");
    printk(KERN_ALERT "Goodbye world 1.\n");
}


