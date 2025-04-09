#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // Pour la fonction sleep

#define BUFFER_SIZE 5

// Tampon circulaire
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

// Sémaphores et mutex
sem_t empty;  // Compte les emplacements vides
sem_t full;   // Compte les emplacements pleins
pthread_mutex_t mutex; // Protège l'accès au tampon

// Fonction du producteur
void *producer(void *arg) {
    int n = *(int *)arg; // Nombre d'éléments à produire
    for (int i = 0; i < n; i++) {
        int item = rand() % 100; // Générer un élément aléatoire

        sem_wait(&empty); // Attendre un emplacement vide
        pthread_mutex_lock(&mutex); // Verrouiller l'accès au tampon

        // Ajouter l'élément au tampon
        buffer[in] = item;
        printf("Producteur a produit : %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); // Déverrouiller l'accès au tampon
        sem_post(&full); // Signaler qu'un élément est disponible

        sleep(1); // Simulation de production lente
    }
    pthread_exit(NULL);
}

// Fonction du consommateur
void *consumer(void *arg) {
    int n = *(int *)arg; // Nombre d'éléments à consommer
    for (int i = 0; i < n; i++) {
        sem_wait(&full); // Attendre un élément disponible
        pthread_mutex_lock(&mutex); // Verrouiller l'accès au tampon

        // Retirer un élément du tampon
        int item = buffer[out];
        printf("Consommateur a consommé : %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); // Déverrouiller l'accès au tampon
        sem_post(&empty); // Signaler un emplacement vide

        sleep(1); // Simulation de consommation lente
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod_thread, cons_thread;
    int n = 10; // Nombre d'éléments à produire et consommer

    // Initialisation des sémaphores et du mutex
    sem_init(&empty, 0, BUFFER_SIZE); // Tampon commence vide
    sem_init(&full, 0, 0);            // Aucun élément disponible au début
    pthread_mutex_init(&mutex, NULL);

    // Création des threads
    pthread_create(&prod_thread, NULL, producer, &n);
    pthread_create(&cons_thread, NULL, consumer, &n);

    // Attente de la fin des threads
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Destruction des sémaphores et du mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
