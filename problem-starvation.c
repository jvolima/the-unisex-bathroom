// bibliotecas

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

// macros

#define MAX_ERROR_LENGTH 128

// declarar estruturas globais

sem_t s_male, s_female;
pthread_cond_t cond_male, cond_female;
pthread_mutex_t mutex;

void *male () {
  int val;

  sleep(rand() % 10);

  // verificando se há mulheres no banheiro

  pthread_mutex_lock(&mutex);
    sem_getvalue(&s_female, &val);
    
    if (val < 3) {
      pthread_cond_wait(&cond_male, &mutex);
    }
  pthread_mutex_unlock(&mutex);

  // tentar entrar no banheiro

  sem_wait(&s_male);
    sleep(rand() % 10);
    printf("Male used the bathroom.\n"); 
    fflush(stdout);
  sem_post(&s_male);

  // liberar banheiro para mulheres

  pthread_mutex_lock(&mutex);
    sem_getvalue(&s_male, &val);
    if (val == 3) {
      pthread_cond_broadcast(&cond_female);
      printf("Male released for female.\n\n"); 
    }
  pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void *female () {
  int val;

  sleep(rand() % 10);

  // verificando se há homens no banheiro

  pthread_mutex_lock(&mutex);
    sem_getvalue(&s_male, &val);
    if (val < 3) {
      pthread_cond_wait(&cond_female, &mutex);
    }
  pthread_mutex_unlock(&mutex);

  // tentar entrar no banheiro

  sem_wait(&s_female);
    sleep(rand() % 10);
    printf("Female used the bathroom.\n"); 
    fflush(stdout);
  sem_post(&s_female);

  // liberar banheiro para homens

  pthread_mutex_lock(&mutex);
    sem_getvalue(&s_female, &val);
    if (val == 3) {
      pthread_cond_broadcast(&cond_male);
      printf("Female released for male.\n\n"); 
    }
  pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int main() {
  // declarar variáveis

  srandom(getpid());
  
  int max_male = 20;
  int max_female = 20;

	pthread_t *th_males = (pthread_t*) malloc(max_male * sizeof(pthread_t));
  pthread_t *th_females = (pthread_t*) malloc(max_female * sizeof(pthread_t));
	char err_msg[MAX_ERROR_LENGTH];
  
  // iniciar estruturas

  if (pthread_cond_init(&cond_male, NULL) == -1) {
    strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
		printf("Erro em cond_init: %s\n", err_msg);
		exit(1);    
  }

  if (pthread_cond_init(&cond_female, NULL) == -1) {
    strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
		printf("Erro em cond_init: %s\n", err_msg);
		exit(1);    
  }

  if (pthread_mutex_init(&mutex, NULL) == -1) {
    strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
		printf("Erro em mutex_init: %s\n", err_msg);
		exit(1);    
  }

	if (sem_init(&s_male, 0, 3) < 0) {
		strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
		printf("Erro em sem_init: %s\n", err_msg);
		exit(1);
	}

	if (sem_init(&s_female, 0, 3) < 0) {
		strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
		printf("Erro em sem_init: %s\n", err_msg);
		exit(1);
	}

  // criar threads

  for (int i = 0; i < max_male; i++) {
    if (pthread_create(&th_males[i], NULL, male, NULL) == -1) {
      strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
      printf("Erro em pthread_create: %s\n", err_msg);
      exit(1);
	  }
  }

  for (int i = 0; i < max_female; i++) {
    if (pthread_create(&th_females[i], NULL, female, NULL) == -1) {
      strerror_r(errno, err_msg, MAX_ERROR_LENGTH);
      printf("Erro em pthread_create: %s\n", err_msg);
      exit(1);
	  }
  }

  // esperar threads acabarem
  
  for (int i = 0; i < max_male; i++) {
    pthread_join(th_males[i], NULL);
  }

  for (int i = 0; i < max_female; i++) {
    pthread_join(th_females[i], NULL);
  }

  // destruir estruturas
  
  free(th_males);
  free(th_females);
	sem_destroy(&s_male);
	sem_destroy(&s_female);
  pthread_cond_destroy(&cond_male);
  pthread_cond_destroy(&cond_female);
  pthread_mutex_destroy(&mutex);

	return(0);
}