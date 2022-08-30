#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define BROJ_PUTNIKA 36
#define BROJ_SEDISTA 4
#define BROJ_VAGONA 3

int putniciKojiSeUkrcavaju, putniciKojiSeIskrcavaju;
sem_t mutex1, mutex2;
sem_t redZaUkrcavanje, redZaIskrcavanje;
sem_t punVagon, prazanVagon;
sem_t ukrcavanje, iskrcavanje;

int idBrojac = 0;
int brojGotovihPutnika = 0;

void *putnik (void) {

    int id = idBrojac++;

    sem_wait(&redZaUkrcavanje);

    printf("Putnik broj %d se ukrcava\n", id);

    sem_wait(&mutex1);

    putniciKojiSeUkrcavaju++;

    if (putniciKojiSeUkrcavaju == BROJ_SEDISTA) {
        sem_post(&punVagon);
        putniciKojiSeUkrcavaju = 0;
    }

    sem_post(&mutex1);

    sem_wait(&redZaIskrcavanje);

    printf("Putnik broj %d se iskrcava\n", id);

    sem_wait(&mutex2);
    putniciKojiSeIskrcavaju++;

    if (putniciKojiSeIskrcavaju == BROJ_SEDISTA) {
        sem_post(&prazanVagon);
        putniciKojiSeIskrcavaju = 0;
    }

    sem_post(&mutex2);
    brojGotovihPutnika++;

    pthread_exit(NULL);

}

void *rolerkoster (void) {

    while(brojGotovihPutnika<BROJ_PUTNIKA) {

        for(int trenutniVagon = 1;trenutniVagon <= BROJ_VAGONA;trenutniVagon++) {

            printf ("\nOvo je %d. vagon rolerkostera\n", trenutniVagon );
            sem_wait(&ukrcavanje);
            printf ("Putnici se ukrcavaju!\n");

            for (int i = 0; i < BROJ_SEDISTA; i++)
                sem_post(&redZaUkrcavanje);

            sem_wait(&punVagon);
            sem_post(&ukrcavanje);
            sleep(2);
            printf("%d. vagon rolerkostera je pun!\n",trenutniVagon);
        }

        printf("\nRolerkoster se krece po svojoj putanji\n");
        sleep(4);
        printf("Rolerkoster je zavrsio svoju voznju\n");

        for(int trenutniVagon = 1;trenutniVagon <= BROJ_VAGONA; trenutniVagon++) {

            printf("\nPutnici se iskrcavaju iz vagona %d\n",trenutniVagon);
            sem_wait(&iskrcavanje);
            sleep(3);

            for(int i = 0; i < BROJ_SEDISTA; i++)
                sem_post(&redZaIskrcavanje);

            sem_wait(&prazanVagon);
            sem_post(&iskrcavanje);

        }

    }

    printf ("\nVoznja je gotova za danas!\n");
    pthread_exit(NULL);

}

int main () {

    pthread_t putnici[BROJ_PUTNIKA];
    pthread_t roler_koster;

    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&redZaUkrcavanje, 0, 0);
    sem_init(&redZaIskrcavanje, 0, 0);
    sem_init(&punVagon, 0, 0);
    sem_init(&prazanVagon, 0, 0);
    sem_init(&ukrcavanje, 0, 1);
    sem_init(&iskrcavanje, 0, 1);

    pthread_create(&roler_koster, NULL, rolerkoster, NULL);

    for (int i = 0; i < BROJ_PUTNIKA; i++) 
        pthread_create(&putnici[i], NULL, putnik, NULL);

    pthread_join(roler_koster, NULL);

    return 0;

}