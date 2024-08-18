#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define EXIT_SUCCESS 0;

typedef struct {
  int percentage;
  pthread_mutex_t lock;
  bool ongoing;
  time_t started;
  time_t finished;
  bool is_finished;
} LoadingContext;

typedef struct {
  int frame_rate;
  char *runes;
  LoadingContext *context;
} Spinner;

void clear_screen() {
  printf("\033[2J");
  printf("\033[H");
  fflush(stdout);
}

LoadingContext newContext() {

  time_t start = time(NULL);

  LoadingContext context;
  context.percentage = 0;
  context.ongoing = false;
  context.is_finished = false;
  context.started = start;

  return context;
}

Spinner newSpinner(char *runes, int frame_rate, LoadingContext *context) {
  Spinner s;
  s.context = context;
  s.runes = runes;
  s.frame_rate = frame_rate;

  return s;
}

void *run_spinner(void *spinner) {

  Spinner *spin = (Spinner *)spinner;
  if (spin->context->ongoing) {
    return NULL;
  }

  int interval = 1000000 / spin->frame_rate;
  int i;

  int array_length = 4;
  printf("array length: %d", array_length);
  while (!spin->context->is_finished) {
    for (i = 0; i < array_length; i++) {
      char rune = spin->runes[i];
      clear_screen();
      printf("loading at percentage at :%d%% %c\n", spin->context->percentage,
             rune);
      fflush(stdout);

      usleep(interval);
    }
  }
  return NULL;
}

void start(Spinner *spinner) {
  pthread_t pid;
  if (pthread_mutex_init(&spinner->context->lock, NULL) != 0) {
    printf("Mutex init has failed \n");
    return;
  }
  printf("starting spinner at %ld \n", spinner->context->started);
  if (pthread_create(&pid, NULL, &run_spinner, (void *)spinner) != 0) {
    perror("Failed to create thread");
    return;
  }
  pthread_detach(pid);
}

void change_percentage(int new_per, Spinner *spinner) {
  spinner->context->percentage = new_per;
  return;
}

void stop(Spinner *spinner) {
  if (!spinner->context->ongoing) {
    return;
  } else {
    spinner->context->is_finished = true;
    spinner->context->finished = time(NULL);
    spinner->context->ongoing = false;
    return;
  }
}

int main(int argc, char *argv[]) {
  int percentage = 0;
  char runes[4] = {'|', '/', '-', '\\'};
  LoadingContext context = newContext(&percentage);
  Spinner spinner = newSpinner(runes, 5, &context);
  start(&spinner);
  int i;
  int p = 1;
  for (i = 0; i < 100; i++) {
    change_percentage(i, &spinner);
    sleep(1);
  }
  return EXIT_SUCCESS;
}
// spinner
/*
 * structure:
 * int frame rate
 * char[] charaters
 * context spinnerContext
 *
 *
 * methods:
 * start(*Spinner spinner)
 * stop(*Spinner spinner)
 *
 * typers of spinner
 *
 */

/*
 * LoadingBar
 * Structure:
 * char barrier
 * LoadingContext context
 * *char[] loadingBar
 *
 */

/*
 * LoadingContext
 *
 * structure:
 * int percentage
 * bool  ongoing
 * char[] started
 * char[] finished
 * bool is_finished
 *
 * methods:
 * start()
 * stop()
 */
