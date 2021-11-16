/// TP FINAL - MOTOR DE BUSQUEDA      LABORATORIO II        LAUTARO MANZO - JULIAN CORONEL - TOBIAS NOYA


/// LIBRERIAS Y NOMBRES DE ARCHIVOS

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define doc1


/// ESTRUCTURAS

typedef struct
{
    char palabra[20];
    int idDOC;
    int pos; //incrementa palabra por palabra, y no letra por letra
} termino;

typedef struct nodoT
{
    int idDOC;
    int pos;
    struct nodoT* sig;
} nodoT;

typedef struct nodoA
{
    char palabra[20];
    int frecuencia; //representa la cantidad de nodos de la lista
    nodoT* ocurrencias; //ordenada por idDOC, luego por pos
    struct nodoA* der;
    struct nodoA* izq;
} nodoA;


/// FUNCIONES

// DICCIONARIO - Lauti





// MOTOR DE BUSQUEDA - Julian






// OPERACIONES DE USUARIO - Tobi







/// MAIN

int main()
{

    return 0;
}
