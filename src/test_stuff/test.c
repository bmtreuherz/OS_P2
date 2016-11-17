#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
// #include <pthread.h>
//
// void *inc_x(void *arg)
// {
//   while(1){
//     printf("Looping bruh");
//     wait(1);
//   }
//   /* the function must return something - NULL will do */
//   return NULL;
// }


int main(int argc, char **argv) {
  // /* this variable is our reference to the second thread */
  // pthread_t loop_thread;
  //
  // /* create a second thread which executes inc_x(&x) */
  // if(pthread_create(&loop_thread, NULL, inc_x, NULL)) {
  //   fprintf(stderr, "Error creating thread\n");
  //   return 1;
  // }

  int mpid = fork();
  if(mpid == 0){
    while(1){
      printf("Loopin brah");
      wait(1);
    }
  }
}
