#include <thread>

#include <iostream>
#include "guard.cpp"
#include <string.h>

int c;
void* print_message_function( void *ptr )
{
    my_guard g(&locker);
    char* s = strtok("hay you"," ");
    printf("%s : %d\n",(char*) ptr,c);
     
}
int main(int argc, char const *argv[])
{
     pthread_t thread1, thread2, thread3, thread4, thread5;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     char *message3 = "Thread 3";
     char *message4 = "Thread 4";
     char *message5 = "Thread 5";
     int  iret1, iret2;

    
    /* Create independent threads each ofw which will execute function */
     /* args:
     1.thread id
     2.attributes to the thread (joinable, scheduling priority, stack address)
     3.pointer to execute function (similar to run in java)
     4.arguments to the function, if needed multiple use a struct
     */
   pthread_create( &thread1, NULL, print_message_function, (void*) message1);
   pthread_create( &thread2, NULL, print_message_function, (void*) message2);
   pthread_create( &thread3, NULL, print_message_function, (void*) message3);
   pthread_create( &thread4, NULL, print_message_function, (void*) message4);
   pthread_create( &thread5, NULL, print_message_function, (void*) message5);
     
    //  printf("%d\n",iret2);
     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL);
     pthread_join( thread3, NULL); 
     pthread_join( thread4, NULL); 
     pthread_join( thread5, NULL);  

    //  printf("Thread 1 returns: %d\n",thread1);
    //  printf("Thread 2 returns: %d\n",thread2);
     exit(0);
}
