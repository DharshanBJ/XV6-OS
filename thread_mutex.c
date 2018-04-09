#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"


struct thread_mutex {
    uint locked;       // Is the lock held?
    
    // For debugging:
    char *name;        // Name of lock.
    struct cpu *cpu;   // The cpu holding the lock.
    uint pcs[10];      // The call stack (an array of program counters)
    // that locked the lock.
};


struct thread_mutex lock;

struct balance {
    char name[32];
    int amount;
};

volatile int total_balance = 0;

volatile unsigned int delay (unsigned int d) {
    unsigned int i;
    for (i = 0; i < d; i++) {
        __asm volatile( "nop" ::: );
    }
    
    return i;
}
void
thread_mutex_init(struct thread_mutex *m){
    
    m->locked = 0;
    m->cpu = 0;
}

void
thread_mutex_lock(struct thread_mutex *m)
{
    while(xchg(&m->locked, 1) != 0){
        sleep(1);
    }
    __sync_synchronize();
}

void
thread_mutex_unlock(struct thread_mutex *m)
{
    m->pcs[0] = 0;
    m->cpu = 0;
    __sync_synchronize();
    asm volatile("movl $0, %0" : "+m" (m->locked) : );
}


void do_work(void *arg){
    int i;
    int old;
    
    struct balance *b = (struct balance*) arg;
    printf(1, "Starting do_work: s:%s\n", b->name);
    
    for (i = 0; i < b->amount; i++) {
        
        thread_mutex_lock(&lock);
        old = total_balance;
        delay(100000);
        total_balance = old + 1;
        thread_mutex_unlock(&lock);
    }
    
    printf(1, "Done s:%x\n", b->name);
    
    thread_exit();
    return;
}

int main(int argc, char *argv[]) {
    
    struct balance b1 = {"b1", 3200};
    struct balance b2 = {"b2", 2800};
    
    void *s1, *s2;
    int t1, t2, r1, r2;
    
    s1 = malloc(4096);
    s2 = malloc(4096);
    
    t1 = thread_create(do_work, (void*)&b1, s1);
    t2 = thread_create(do_work, (void*)&b2, s2);
    
    r1 = thread_join();
    r2 = thread_join();
    
    printf(1, "Threads finished: (%d):%d, (%d):%d, shared balance:%d\n",
           t1, r1, t2, r2, total_balance);
    
    exit();
}



