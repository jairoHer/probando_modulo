#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/utsname.h>
#include <linux/mm.h>
#include <linux/swapfile.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/cred.h>
#define BUFSIZE  1000
struct task_struct *task;        /* Estructura definida en sched.h */
struct task_struct *task_child;  /* Estructura para iterar los procesos hijo */
struct list_head *list;          /* Estructura necesaria para iterar la lista de hijos   */

unsigned long copy_to_user(void __user *to,const void *from, unsigned long n);
unsigned long copy_from_user(void *to,const void __user *from,unsigned long n);
MODULE_LICENSE("PROCESOS");
MODULE_AUTHOR("201503750");
/*int proc_idle = 0;
int proc_running = 0;
int proc_sleep = 0;
int proc_stoped = 0;
int proc_zombie = 0;
uid_t uid;*/
static struct proc_dir_entry *ent;

static int myread (struct seq_file *buff, void *v){
    int proc_idle = 0;
    int proc_running = 0;
    int proc_sleep = 0;
    int proc_stoped = 0;
    int proc_zombie = 0;
    int num_proc = 0;
    uid_t uid;
    printk(KERN_INFO "EMPEZANDO MODULO PROCESOS\n");
    seq_printf(buff, "{%s\"procesos\":[","\n");
    for_each_process( task ){            /*    for_each_process() es un MACRO para iterar ubicado en linux\sched\signal.h    */
        uid = __kuid_val(task_uid(task));
        num_proc++;
        seq_printf(buff, "%s","{\n");
        if(task->state == 1026){
            seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\", \n\t\"CHILDS\":[\n", task->pid, task->comm, uid, "I(idle)");
            proc_idle++;
        }
        if(task->state == 0){
            seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\", \n\t\"CHILDS\":[\n", task->pid, task->comm, uid, "R(running)");
            proc_running++;
        }
        if(task->state == 1){
            seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\", \n\t\"CHILDS\":[\n", task->pid, task->comm, uid, "S(sleep)");
            proc_sleep++;
        }

        list_for_each(list, &task->children){                        /*    list_for_each MACRO para iterar task->children    */
            task_child = list_entry( list, struct task_struct, sibling );    /*    using list_entry to declare all vars in task_child struct    */
            seq_printf(buff, "%s","{\n");
            uid = __kuid_val(task_uid(task_child));
            if(task_child->state == 1026){
                seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\" ", task_child->pid, task_child->comm, uid, "I(idle)");
                proc_idle++;
            }
            if(task_child->state == 0){
                seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\"", task_child->pid, task_child->comm, uid, "R(running)");
                proc_running++;
            }
            if(task_child->state == 1){
                seq_printf(buff, "\t\t\"PID\": \"%d\",\n\t\t\"PROCESS\": \"%s\", \n\t\t\"UID\": \"%d\", \n\t\t\"STATE\": \"%s\"", task_child->pid, task_child->comm, uid, "S(sleep)");
                proc_sleep++;
            }
            seq_printf(buff, "%s","\n},\n");
        }
        seq_printf(buff, "%s","\t]\n\n},\n");
    }    
    seq_printf(buff, "%s","]}\n");
    seq_printf(buff, "%s","-------------------------------------\n");
    seq_printf(buff, "TOTAL: %d\n",proc_idle+proc_running+proc_sleep);
    seq_printf(buff, "IDLE: %d\n",proc_idle);
    seq_printf(buff, "RUNNING: %d\n",proc_running);
    seq_printf(buff, "SLEEP: %d\n",proc_sleep);
    seq_printf(buff, "STOPED: %d\n",proc_stoped);
    seq_printf(buff, "ZOMBIE: %d\n",proc_zombie);
    
    return 0;
}


static int proc_init (struct inode *inode, struct file *file){
    return single_open(file,myread,NULL);
}
    

static const struct file_operations myops ={
    .owner =THIS_MODULE,
    .read=seq_read,
    .release=single_release,
    .open=proc_init,
    .llseek=seq_lseek
};

static int simple_init(void){

    printk(KERN_INFO "EMPEZANDO MODULO PROCESOS\n");
    ent=proc_create("proc_procesos",0,NULL,&myops);
    return 0;
}

static void simple_cleanup(void)
{
    printk(KERN_INFO "TERMINANDO MODULO\n");
    proc_remove(ent);

}

module_init(simple_init);
module_exit(simple_cleanup);
