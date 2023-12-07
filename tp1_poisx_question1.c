#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

sem_t sem_compteur;
pthread_mutex_t sc_camion;

#define camions	5
#define voitures	5
#define vehicules  (camions+voitures)
void wait()
{
        struct timespec tempsAttente;
        tempsAttente.tv_sec=1;
        tempsAttente.tv_nsec=0;
        nanosleep(&tempsAttente,NULL);
}

void entrer_pont(int tonnes)
{
    if (tonnes==15)
    {
        pthread_mutex_lock(&sc_camion);
        while(tonnes>0)
        {
            sem_wait(&sem_compteur);
            tonnes=tonnes-5;
        };
        pthread_mutex_unlock(&sc_camion);
    }
    else  sem_wait(&sem_compteur);
}
void quitter_pont(int tonnes)
{
    while(tonnes>0)
    {
        sem_post(&sem_compteur);
        tonnes=tonnes-5;
    };
}
void* voiture(void* arg)
{
	wait();
    entrer_pont(5);
	printf(" Voiture %d : entre le pont \n",*((int*)arg)-camions);
	wait();
	printf(" Voiture %d : quitte le pont \n",*((int*)arg)-camions);
    quitter_pont(5);
	pthread_exit(NULL);
}
void* camion(void* arg)
{
	wait();
    entrer_pont(15);
	printf(" Camion %d : entre le pont \n",*((int*)arg));
	wait();
	printf(" Camion %d : quitte le pont \n",*((int*)arg));
    quitter_pont(15);
	pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
	int i;
	pthread_t id;

	sem_init(&sem_compteur, 0, 3);

	for(i=0; i<vehicules;i++){

		int* j=(int*)malloc(sizeof(int));
		*j=i;
		if (i<camions)
		pthread_create(&id,NULL,camion,j);
		else	
		pthread_create(&id,NULL,voiture,j);
	}
	pthread_exit(NULL);
};
