#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/seq_file.h>	

#define procfs_name "leakSecretByteAddress"
static struct  proc_dir_entry *procFIle;
static unsigned char * secret = NULL;

MODULE_AUTHOR("D3fa1t");
MODULE_DESCRIPTION("A simple Linux Driver for testing spectre V1");
MODULE_VERSION("1");
MODULE_LICENSE("GPL");

//more on seq file can be found at https://linux.die.net/lkmpg/x861.html

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	if ( *pos == 0 )
	{	
	
		return &secret;
	}
	else
	{
	
		*pos = 0;
		return NULL;
	}
}

char temp;

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    int i= 0;
    for( i = 0; i < 10; i++)
    {
        temp = *((char*)secret+1);
        
    }
    
    
	return NULL;
}


static void my_seq_stop(struct seq_file *s, void *v)
{

}


static int my_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	char temp  = *((char*)secret+1);
	seq_printf(s, "%016p\n", (void*)secret+1);
	return 0;
}


static struct seq_operations my_seq_ops={

    .start  = my_seq_start ,
    .next   = my_seq_next  ,
    .stop   = my_seq_stop  ,
    .show   = my_seq_show  
};

static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_ops);
};

static struct file_operations ops =
{
	.read   = seq_read,
    .open   = my_open,
    .owner  = THIS_MODULE,
    .llseek = seq_lseek,
    .release= seq_release

};


int init_module(void){


    secret = kmalloc(PAGE_SIZE,GFP_KERNEL);
    strcpy(secret+1,"Hack me?"); // Hack me? is the secret string and 
                                //'H' is the secret Byte in kernel space which we are going to extract from user space 
    printk(KERN_ALERT "spectreKernalModuleInit and secret is at %p \n",secret+1 );
    procFIle = proc_create(procfs_name,0660,NULL,&ops);
    if(procFIle == NULL){
        proc_remove(procFIle);
        printk(KERN_ALERT "Error:Could not initialize /proc/%s\n",procfs_name);
        return -ENOMEM;
    }
    return 0 ;
}


void cleanup_module(void){
    proc_remove(procFIle);
    printk(KERN_ALERT "spectreKernelModuleDestroy \n");
}