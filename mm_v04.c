/*****************************************
Para su debida ejuecucion:gcc mv_v01.c 
./a.out

* Fecha: 11-04-24
* Autor: Juan Esteban Paez Alfonso
* Materia: Sistemas Operativos
* Tema: Concurrencia
* Objetivo: Hacer una metodologia para la implementacion de la multiplicacion de matrices.

*****************************************/


// libreras estandar de C, pthread para el manejo de hilos y sys/time para medir el tiempo
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Definimos una constante para reservar memoria para las matrices
#define RESERVA (1024 * 128 * 64 * 8)

// Reserva de memoria estatica para las matrices
static double MEM_CHUNK[RESERVA];

// Estructura para almacenar los datos necesarios para cada hilo
struct datosMM
{
    int N;
    int H;
    int idH;
};
double *mA;
double *mB;
double *mC;

// Funcin para inicializar las matrices con valores aleatorios
void iniMatrices(int n, double *m1, double *m2, double *m3)
{
    for (int i = 0; i < n * n; i++)
    {
        m1[i] = i * 1.1; // Inicializamos la matriz 1
        m2[i] = i * 2.2; // Inicializamos la matriz 2
        m3[i] = 0.0;     // Inicializamos la matriz 3 con ceros
    }
};

// Funcion para imprimir las matrices
void impreMatrices(int n, double *m1)
{
    if (n < 9)
    {
        for (int i = 0; i < n * n; i++)
        {
            if (i % n == 0)
                printf("\n");    
            printf(" %f", m1[i]); // Imprimimos el elemento actual
        }
        printf("\n###########################################\n ");
    }
    else
    {
        printf("\n###########################################\n ");
    }
};

void *MulMatrices(void *argMM)
{
    // Descomprimir los datos
    struct datosMM *datos = (struct datosMM *)argMM;
    int n = datos->N;
    int h = datos->H;
    int idH = datos->idH;

    // Imprimimos la cantidad de hilos y el ID del hilo actual
    printf("Cantidad de hilos: %d, ID del hilo actual: %d\n", h, idH);

    // Accede a las matrices globales directamente
    double *m1 = mA;
    double *m2 = mB;
    double *m3 = mC;

    // Calcula el rango de filas que este hilo debe procesar
    int inicio = (n / h) * idH;
    int fin = (idH == h - 1) ? n : inicio + (n / h);

    // Realiza el algoritmo clqsico de multiplicación de matrices
    for (int i = inicio; i < fin; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double sumaTemp = 0.0;
            double *pA = m1 + i * n;
            double *pB = m2 + j;
            for (int k = 0; k < n; k++, pA++, pB += n)
            {
                sumaTemp += (*pA) * (*pB);
            }
            m3[i * n + j] = sumaTemp;
        }
    }
    return 0;
};

int main(int argc, char *argv[])
{
    if (argc <= 2)
    { // Verifica que se hayan ingresado los argumentos necesarios
        printf("Error: Ingrese la dimensión de la matriz y el número de hilos\n");
        printf("\n\t $ejecutable.exe DIM NumHilos\n");
        return -1;
    }

    // Convierte los argumentos de la línea de comandos a enteros
    int N = (int)atof(argv[1]);
    int H = (int)atof(argv[2]);

    // Creaa un array de hilos
    pthread_t hilos[H];

    // Asignación de memoria
    mA = (double *)malloc(N * N * sizeof(double));
    mB = (double *)malloc(N * N * sizeof(double));
    mC = (double *)malloc(N * N * sizeof(double)); // Asignación de memoria para mC

    // Inicializa las matrices
    iniMatrices(N, mA, mB, mC);

    printf("\n++++++++++++++++++++++++++++++++++++++\n");

    // Creamos los hilos y los iniciamos
    for (int h = 0; h < H; h++)
    {
        struct datosMM *valoresMM = (struct datosMM *)malloc(sizeof(struct datosMM));
        valoresMM->N = N;
        valoresMM->H = H;
        valoresMM->idH = h;                                      // Aasignar valores a los miembros de la estructura
        pthread_create(&hilos[h], NULL, MulMatrices, valoresMM); // Creas un hilo y lo iniciamos con la función multMatrices
    }

    // Esperamos a que terminen los hilos
    for (int h = 0; h < H; h++)
    {
        pthread_join(hilos[h], NULL); // Esperamos a que termine el hilo
    }

    // Imprimimos las matrices en pantalla
    impreMatrices(N, mA);
    impreMatrices(N, mB);
    impreMatrices(N, mC);

    printf("\n+Fin del programa...............!\n");

    return 0;
} // Fin el rpograma
