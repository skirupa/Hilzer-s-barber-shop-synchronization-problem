#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#define MAX 20
#define NO_OF_CUST 6
int customers=0;
pthread_t cust[NO_OF_CUST];
pthread_t barb[3];
sem_t mutex,chair,barber,customer,cash,reciept;

typedef struct{
sem_t leader;
sem_t follower;
}Fifo;

Fifo *standingRoom,*sofa;

Fifo* make_Fifo(int n)
{
    Fifo* F=(Fifo*)malloc(sizeof(Fifo));
    sem_init(&(F->leader),0,0);
    sem_init(&(F->follower),0,n);
    return(F);
}
void wait_Fifo(Fifo* F,int n)
{
    sem_wait(&(F->follower));
    sem_post(&(F->leader));
}
void signal_Fifo(Fifo* F)
{
    sem_wait(&(F->leader));
    sem_post(&(F->follower));
}
void* barbershop(void *arg)
{

    int n=*(int *)arg;
    //while(1)
    //{
    sem_wait(&mutex);
    if(customers>=20)
    {
        sem_post(&mutex);
        printf("exiting shop %d.. \n",n);//exit_shop();
    }
    customers+=1;
    sem_post(&mutex);
    wait_Fifo(standingRoom,n);//standing_room_wait();
    printf("Entering to standing room %d \n",n);//enter_shop();
    wait_Fifo(sofa,n);//sofa_wait();
    printf("siting in sofa %d \n",n);//sitOnSofa();
    //sleep(1);
    signal_Fifo(standingRoom);//standing_room_signal();
    sem_wait(&chair);
    printf("sit barb chair %d \n",n);//SitOnBarberChair();
    sleep(3);
    signal_Fifo(sofa);
    sem_post(&customer);

    sem_wait(&barber);
    printf("Get hair Cut %d\n",n);//getHairCut();
    sleep(2);
    printf("pay %d \n",n);//pay();
    sem_post(&cash);
    sem_wait(&reciept);
    sem_wait(&mutex);
    customers-=1;
    sem_post(&mutex);
    printf("exiting shop %d \n",n);
 //sleep(6);
 //exit_shop);

//}
}
void* cutting(void* arg)
{
    int n=*(int *)arg;
    int i;
    for(i=0;i<NO_OF_CUST;i++)
    {
        sem_wait(&customer);
        sem_post(&barber);
        printf("\tbarber %d cutting hair\n",n);
        sleep(3);
        sem_wait(&cash);
        printf("\tbarber %d accpting payment\n",n);
        sleep(1);
        sem_post(&reciept);
        sem_post(&chair);

    }
}
void main()
{
    int i,cust_id[NO_OF_CUST];
    sem_init(&mutex,0,1);
    sem_init(&chair,0,3);
    sem_init(&barber,0,0);
    sem_init(&customer,0,0);
    sem_init(&cash,0,0);
    sem_init(&reciept,0,0);
    standingRoom=make_Fifo(16);
    sofa=make_Fifo(4);
    for(i=0;i<NO_OF_CUST;i++)
    {
        cust_id[i]=i;
        pthread_create(&cust[i],0,barbershop,&cust_id[i]);
    }
    for(i=0;i<3;i++)
    {
        pthread_create(&barb[i],0,cutting,&cust_id[i]);
    }
    printf("thread creation successful\n");
    for(i=0;i<NO_OF_CUST;i++)
    {
        pthread_join(cust[i],NULL);
    }
    printf("thread joining successful\n");

printf("\nSimulation Ended!\n");
}
