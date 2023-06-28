#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>
int max_lenght = 20;

void* funcion_hilo(void*);
/*declarando las estructuras de datos*/
/*esta estructura se le enviara al hilo, para que sepa donde comienza y termina su rango*/
struct values{
    int id;
};
/*variable con la cantidad de hilos*/
int nE;
int nCars;
int *espacios;
int times;
int nTimes=0;
int pista;

pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t mybarrier;
int main(int args, char ** argsv){
    FILE *file;
    file = fopen("mantenimientoConfig.txt","rt");
    char *line = (char*) malloc( max_lenght*sizeof(char));
    fgets(line, max_lenght*sizeof(char), file);
    nCars = atoi(line);
    fgets(line, max_lenght*sizeof(char), file);
    nE = atoi(line);
    fgets(line, max_lenght*sizeof(char), file);
    int espacio = atoi(line);

    printf("El numero de carros es [%d]. \n",nCars);
    
    printf("El numero de estaciones es [%d]. \n",nE);

    printf("El espacio es [%d]. \n",espacio);

    /*declarando el vector de espacios*/

    espacios = (int*) malloc(nE*sizeof(int));
    for(int i  = 0; i<nE;i++){
        espacios[i] = espacio;
    }
    
    /*creando los hilos*/
    struct values *datos =  (struct values *) malloc(nCars*sizeof(struct values));
    pthread_t ids[nCars];
    for(int i = 0; i<nCars;i++){
        datos[i].id= i;
        pthread_create(&ids[i],NULL,funcion_hilo,&datos[i]);
    }
    for(int i = 0;i<nCars;i++){
        pthread_join(ids[i],NULL);
    }
    printf("Todos los vehiculos han terminado su mantenimiento y estan listos para volver a la carretera. \n");

    printf("Liberando los recursos. \n");
    free(line);
    free(datos);
    free(espacios);
    printf("Terminando el padre. \n");
    return 0;
}
int disponible(){
    for(int i =0;i<nE;i++){
        if(espacios[i]!=0){
            return i;
        }
    }
    return -1;
}

void* funcion_hilo(void* arg){
    /*extrayendo el dato de los argumentos*/
    struct values* data = (struct values*)arg;
    int id = data->id;

    pthread_mutex_lock(&mutex);
    int estacion = disponible();
    while(estacion == -1){
        printf("El vehiculo [%d] esta esperando. \n",data->id+1);
        pthread_cond_wait(&cond, &mutex);
        estacion = disponible();
    }	
    espacios[estacion] =espacios[estacion]-1;
    pthread_mutex_unlock(&mutex);
    printf("El vehiculo [%d] ha ingresado a la estacion [%d]. \n",id+1,estacion+1);
    srand (time(NULL));  
    sleep(2);
    printf("El vehiculo [%d] ha comenzado el mantenimiento de bateria en la estacion [%d]. \n",id+1,estacion+1);
    printf("El vehiculo [%d] ha comenzado el mantenimiento de motor en la estacion [%d]. \n",id+1,estacion+1);
    printf("El vehiculo [%d] ha comenzado el mantenimiento de direccion en la estacion [%d]. \n",id+1,estacion+1);
    printf("El vehiculo [%d] ha comenzado el mantenimiento de sistema de navegacion en la estacion [%d]. \n",id+1,estacion+1);
    printf("El vehiculo [%d] ha salido de la estacion [%d]. \n",id+1,estacion+1);
    pthread_mutex_lock(&mutex);
    espacios[estacion]++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}