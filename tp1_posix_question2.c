#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define camions	9
#define voitures	5
#define vehicules  (camions+voitures)

#define en_attente 1
#define rien 2
#define traverser 3

pthread_mutex_t sectionCritique;
sem_t semaphore[vehicules];
int state[vehicules];
int bloqued_camions=0;
int limit=0; 



void wait()
{
        struct timespec delay;
        delay.tv_sec=1;
        delay.tv_nsec=0;
        nanosleep(&delay,NULL);
}

void entrer_pont(int tonnes, int id)
{
    pthread_mutex_lock(&sectionCritique);

    if(limit+tonnes <= 15)
    {
        limit=limit+tonnes;
        state[id]=traverser;
        sem_post(&semaphore[id]);
    }
    else	{
        state[id]=en_attente;
        if(tonnes==15)
            bloqued_camions++;
    }

    pthread_mutex_unlock(&sectionCritique);
    sem_wait(&semaphore[id]);
}



void quitter_pont(int tonnes, int pid)
{
    int i;

    pthread_mutex_lock(&sectionCritique);

    state[pid]=rien;
    limit=limit-tonnes;

    for(i=0; i<camions;i++)
        if ( (state[i]==en_attente) && (limit==0) )
        {
            sem_post(&semaphore[i]);
            limit=15;
            bloqued_camions--;
        }

    for(i=camions; i<vehicules;i++)
        if ( (limit<15) && (bloqued_camions==0)
             && (state[i]==en_attente) )
        {
            limit=limit+5;
            sem_post(&semaphore[i]);
        }

    pthread_mutex_unlock(&sectionCritique);
}

void* voiture(void* arg)
{
	wait();
	entrer_pont(5, *((int*)arg));
	printf(" Voiture %d : entre le pont \n",*((int*)arg)-camions + 1);
	wait();
	printf(" Voiture %d : quitte le pont \n",*((int*)arg)-camions + 1);
	quitter_pont(5, *((int*)arg));
	pthread_exit(NULL);
}

void* camion(void* arg)
{
	wait();
	entrer_pont(15,*((int*)arg));
	printf(" Camion %d : entre le pont \n",*((int*)arg) + 1);
	wait();
	printf(" Camion %d : quitte le pont \n",*((int*)arg) + 1);
	quitter_pont(15,*((int*)arg));
	pthread_exit(NULL);
}






int main(int argc, char* argv[])
{
	int i;
	pthread_t id;


	for(i=0; i<vehicules;i++)
			{
			state[i]=rien;
			sem_init(&semaphore[i],0,0);
			}

	pthread_mutex_init(&sectionCritique,0);

	for(i=0; i<vehicules;i++)
			{
			int* j=(int*)malloc(sizeof(int));
			*j=i;
			if (i<camions)
				pthread_create(&id,NULL,camion,j);
			else	pthread_create(&id,NULL,voiture,j);
			}


	pthread_exit(NULL);
};
