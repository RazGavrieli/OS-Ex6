#include <pthread.h>

#include <iostream>
#include "guard.cpp"
#include <string.h>
#include <unistd.h>
pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER; 

const int loop = 10000000;
int c;
void* print_message_function( void *ptr )
{
    my_guard g(&locker);
    for (size_t i = 0; i < loop; i++)
    {
      c+=1;
    }
    
    return NULL;
     
}
int main(int argc, char const *argv[])
{
  c = 0;
  pthread_t thread1, thread2, thread3, thread4, thread5;

  pthread_create( &thread1, NULL, print_message_function, NULL);
  pthread_create( &thread2, NULL, print_message_function, NULL);
  pthread_create( &thread3, NULL, print_message_function, NULL);
  pthread_create( &thread4, NULL, print_message_function, NULL);
  pthread_create( &thread5, NULL, print_message_function, NULL);

  printf("incrementing c variable on the thread functions..\n");

  pthread_join( thread1, NULL);
  pthread_join( thread2, NULL);
  pthread_join( thread3, NULL); 
  pthread_join( thread4, NULL); 
  pthread_join( thread5, NULL);  

  printf("c=%d\n", c);
  if (c!=5*10000000) {
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  } 
    printf("Test passed!\n");
    exit(EXIT_SUCCESS);
    //  printf("Thread 1 returns: %d\n",thread1);
    //  printf("Thread 2 returns: %d\n",thread2);
}
