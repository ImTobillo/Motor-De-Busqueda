/// TP FINAL - MOTOR DE BUSQUEDA      LABORATORIO II        LAUTARO MANZO - JULIAN CORONEL - TOBIAS NOYA


/// LIBRERIAS Y NOMBRES DE ARCHIVOS

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define leng "c.txt"
#define boca "bocajuniors.txt"
#define idDocArch "idDoc.bin"

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

/// PROTOTIPADOS
// Primera parte
void inicIdDoc();
int cantDatosArch();
void agregaCaracterAPalabra(char*, char);
void leerArchivo(termino*, int*, char*);
// Segunda parte
void escrituraDiccionario(termino*, int);
void leer();

//
void mostrarArbol(nodoA*);


/// FUNCIONES

// DICCIONARIO - Lauti



int cantDatosArch()
{
    FILE* puntArch = fopen(boca, "rb");

    int total = 0;

    if(puntArch != NULL)
    {
        fseek(puntArch, 0, SEEK_END);
        total = ftell(puntArch)/sizeof(char);
        fclose(puntArch);
    }
    return total;
}

/// ------------- Diccionario -------------
/// Se tienen N documentos, donde se leerán todas las palabras de cada uno de ellos.
/// Los documentos deben ser archivos de texto leídos como un binario (char)), sobre un vector que contendrá cada palabra,
/// id de documento al que pertenece, y posición en dicho documento.

void agregaCaracterAPalabra(char* palabras, char caracter)
{
    int i = 0;

    while(palabras[i] != '\0')
    {
        i++;
    }

    palabras[i] = caracter;
    palabras[i+1] = '\0';
}

void inicIdDoc()
{
    FILE* puntArch = fopen(idDocArch, "wb");    //  WB

    int idDoc = 0;

    if (puntArch)
    {
        fwrite(&idDoc, sizeof(int), 1, puntArch);
        fclose(puntArch);
    }
    else
    {
        printf("ERROR.\n");
    }
}

///    a mí en este caso se me ocurre hacer una función que reciba el caracter leído y lo compare con los caracteres especiales
///   (vocales con tilde, ñ), si lo encuentra igual a uno entonces devolves un char con el valor entero de la tabla ascii,
///    ej:* entra 'ó' entonces devuelve 162, se lo asignas a tu variable caracter y ahora sí entraría en el if

void leerArchivo(termino* terminos, int* validos, char* nomArch)
{
    FILE* puntArchIdDoc = fopen(idDocArch, "r+b");   // r+b
    FILE* puntArch = fopen(nomArch, "rb");

    int flag = 0, pos = 0, idDoc;   // variable para saber si se leyó un caracter raro

    char palabra[20], caracter;

    memset(palabra, 0, 20*sizeof(palabra[0]));

    if(puntArchIdDoc != NULL && puntArch != NULL)
    {
        fread(&idDoc, sizeof(int), 1, puntArchIdDoc);
        idDoc++;
        fseek(puntArchIdDoc, -1*sizeof(int), SEEK_CUR);
        fwrite(&idDoc, sizeof(int), 1, puntArchIdDoc);

        while(fread(&caracter, sizeof(char), 1, puntArch) > 0)
        {
//                    (65 a 90 MAYUSCULAS)                   (97 a 122 MINUSCULAS)
            if((caracter >= 65 && caracter <= 90) || (caracter >= 97 && caracter && 122) || caracter == 177)   // AGREGAR ACENTOS
            {
                flag = 0;
                agregaCaracterAPalabra(palabra, caracter);
            }
            else
            {
                if(flag == 0)
                {
                    flag = 1;

                    strcpy(terminos[*validos].palabra, palabra);
                    terminos[*validos].idDOC = idDoc;
                    terminos[*validos].pos = pos;
                    pos++;

                    (*validos)++;

                    //     palabra[0] = '\0';      // VER OTRA FORMA PARA ESTO

                    memset(palabra, 0, 20*sizeof(palabra[0]));  // Inicializa todas las celdas en 0
                }
            }
        }

        fclose(puntArchIdDoc);
        fclose(puntArch);
    }
    else
    {
        printf("Error de archivo\n");
    }
}

/// Una vez finalizada la lectura de los documentos, se procede a generar el diccionario.
/// El diccionario es un archivo binario de registros de tipo término

void escrituraDiccionario(termino* terminos, int validos)
{
    FILE* puntArch = fopen("diccionario.bin", "ab"); // WB -> AB

    int i = 0;

    if(puntArch != NULL)
    {
        while(i < validos)
        {
            fwrite(&terminos[i], sizeof(termino), 1, puntArch);
            i++;
        }
        fclose(puntArch);
    }
}

void leer()
{
    FILE* puntArch = fopen("diccionario.bin", "rb");

    termino palabra;

    if(puntArch != NULL)
    {
        while(fread(&palabra, sizeof(termino), 1, puntArch) > 0)
        {
            printf("____________________________\n");
            printf("Palabra: %s \n", palabra.palabra);
            printf("Id: %i \n", palabra.idDOC);
            printf("Posicion: %i \n", palabra.pos);
            printf("____________________________\n");
        }
        fclose(puntArch);
    }
}

// MOTOR DE BUSQUEDA - Julian

//Crear nodos
nodoA* crearNodoArbol(char* palabra)
{
    nodoA* nuevo = (nodoA*)malloc(sizeof(nodoA));
    strcpy(nuevo->palabra,palabra);
    nuevo->frecuencia = 0;
    nuevo->ocurrencias = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;

    return nuevo;
}

nodoT* crearNodoOcurrencias(int idDoc, int pos)
{
    nodoT* nuevo = (nodoT*)malloc(sizeof(nodoT));
    nuevo->idDOC = idDoc;
    nuevo->pos = pos;
    nuevo->sig = NULL;

    return nuevo;
}

//Inserta ordenado alfabéticamente el nodo en el árbol
void insertarEnArbol(nodoA** arbol, char* registro)
{
    if(*arbol == NULL)
    {
        *arbol = crearNodoArbol(registro);
    }
    else
    {
        if(strcmpi((*arbol)->palabra, registro) > 0)
        {
            insertarEnArbol(&(*arbol)->izq, registro);
        }
        else
        {
            insertarEnArbol(&(*arbol)->der, registro);
        }
    }
}

//Inserta la ocurrencia en la lista, creando un nodo nuevo con los datos y luego lo inserta ordenado. También comprueba la posición
void insertarOcurrencia(nodoT** lista, nodoT* ocurrencia)
{
    nodoT* nuevo = crearNodoOcurrencias(ocurrencia->idDOC, ocurrencia->pos);
    nodoT* act;
    nodoT* ant;

    if(*lista == NULL)
    {
        *lista = nuevo;
    }
    else
    {
        if(nuevo->idDOC < (*lista)->idDOC)
        {
            nuevo->sig = *lista;
            *lista = nuevo;
        }
        else
        {
            act = (*lista)->sig;
            ant = *lista;

            while(act != NULL && act->idDOC <= nuevo->idDOC && act->pos < nuevo->pos)
            {
                ant = act;
                act = act->sig;
            }

            nuevo->sig = act;
            ant->sig = nuevo;
        }
    }
}

//Busca el arbol hasta encontrar la palabra, si no la encuentra la agrega al principio, si la encuentra, solo agrega la ocurrencia
void buscaPalabra(nodoA** arbol, char* clave, nodoT* ocurrencia)
{
    nodoA* aux = *arbol;

    while(aux != NULL && strcmpi(aux->palabra, clave) != 0)
    {
        if(strcmpi(aux->palabra, clave) > 0)
        {
            aux = aux->izq;
        }
        else
        {
            aux = aux->der;
        }
    }

    if(aux != NULL && strcmpi(aux->palabra, clave) == 0)
    {
        insertarOcurrencia(&aux->ocurrencias, ocurrencia);
        aux->frecuencia = aux->frecuencia + 1;
    }
    else
    {
        insertarEnArbol(&aux, clave);
        insertarOcurrencia(&aux->ocurrencias, ocurrencia);
        aux->frecuencia = aux->frecuencia + 1;
    }
}

void insertarOcurrenciaRec(nodoT** ocurrencias, nodoT* nuevo)
{
    if (*ocurrencias)
    {
        if ((*ocurrencias)->idDOC == nuevo->idDOC)
        {
            if (nuevo->pos < (*ocurrencias)->pos)
            {
                nuevo->sig = *ocurrencias;
                *ocurrencias = nuevo;
            }
            else
                insertarOcurrenciaRec(&(*ocurrencias)->sig, nuevo);
        }
        else if (nuevo->idDOC < (*ocurrencias)->idDOC)
        {
            nuevo->sig = *ocurrencias;
            *ocurrencias = nuevo;
        }
        else
            insertarOcurrenciaRec(&(*ocurrencias)->sig, nuevo);
    }
    else
        *ocurrencias = nuevo;
}

void buscarEInsertar (nodoA** arbol, char* palabra, nodoT* ocurrencia)
{
    if (*arbol)
    {
        if (strcmpi((*arbol)->palabra, palabra) == 0)
        {
            (*arbol)->frecuencia++;
            insertarOcurrenciaRec(&(*arbol)->ocurrencias, ocurrencia);
        }
        else
        {
            if (strcmpi((*arbol)->palabra, palabra) < 0)
                buscarEInsertar(&(*arbol)->der, palabra, ocurrencia);
            else
                buscarEInsertar(&(*arbol)->izq, palabra, ocurrencia);
        }
    }
    else
    {
        *arbol = crearNodoArbol(palabra);
        (*arbol)->frecuencia = 1;
        (*arbol)->ocurrencias = ocurrencia;
    }
}

//Leer archivo bin y crear el arbol con todos los datos de cada palabra
void generarArbolBusqueda(nodoA** ABBDiccionario)
{
    FILE* diccionario = fopen("diccionario.bin", "rb");
    termino registro;
    nodoT* ocurrencia = NULL;

    if(diccionario)
    {
        while(fread(&registro, sizeof(termino), 1, diccionario) > 0)
        {
            ocurrencia = crearNodoOcurrencias(registro.idDOC, registro.pos);

            //buscaPalabra(&(*ABBDiccionario), registro.palabra, ocurrencia);
            buscarEInsertar(ABBDiccionario, registro.palabra, ocurrencia);
        }
        fclose(diccionario);
    }
}

//// tobi

void mostrarOcurrencias (nodoT* lista)
{
    while(lista)
    {
        printf("=================\n");
        printf("ID DOC: %i\n", lista->idDOC);
        printf("POS: %i\n", lista->pos);
        lista = lista->sig;
    }

    printf("=================\n");
}


// OPERACIONES DE USUARIO - Tobi

// 1)

void mostrarArbol(nodoA* arbol)
{
    if (arbol)
    {
        printf("Palabra: %s\n", arbol->palabra);
        printf("Frecuencia: %i\n", arbol->frecuencia);
        mostrarOcurrencias(arbol->ocurrencias);
        mostrarArbol(arbol->izq);
        mostrarArbol(arbol->der);
    }
}

void buscarPalabraYMostrarOcurrencias (nodoA* arbol, char* palabra)
{
    if (arbol)
    {
        if (strcmpi(arbol->palabra, palabra) == 0)
        {
            printf("OCURRENCIAS DE '%s':\n", palabra);
            mostrarOcurrencias(arbol->ocurrencias);
        }
        else
        {
            if (strcmpi(arbol->palabra, palabra) > 0)
                buscarPalabraYMostrarOcurrencias(arbol->izq, palabra);
            else
                buscarPalabraYMostrarOcurrencias(arbol->der, palabra);
        }

    }
    else
        printf("NO SE ENCONTRO LA PALABRA '%s'\n", palabra);
}

// 2)
/*
void buscarPalabraEnDosDocumentosYMostrarOcurrencias (nodoA* arbol, char* palabra)
{
    if (arbol)
    {
        if ()
        {

        }
        else
        {
            if(strcmpi())
                buscarPalabraEnDosDocumentosYMostrarOcurrencias();
            else
                buscarPalabraEnDosDocumentosYMostrarOcurrencias();
        }
    }
    else
        printf("NO SE ENCONTRO LA PALABRA '%s'.\n", palabra);
}
*/


/// MAIN

int main()
{
    //inicIdDoc();  // Usar solamente al cargar el primer texto

/*    termino *arreglo;
    int validos = 0;
    arreglo = (termino*)malloc(sizeof(termino)*cantDatosArch());

    leerArchivo(arreglo, &validos, boca);
*/
/*    for (int i = 0; i < validos; i++)
    {
        printf("____________________________\n");
        printf("Palabra: %s \n", arreglo[i].palabra);
        printf("Id: %i \n", arreglo[i].idDOC);
        printf("Posicion: %i \n", arreglo[i].pos);
        printf("____________________________\n");
    }
*/
//    printf("Validos: %i\n", validos);
//    printf("Cant: %i\n", cantDatosArch());

    printf("----------- ARCHIVO DICCIONARIO -----------\n");

//    escrituraDiccionario(arreglo, validos);
    leer();

//    unsigned char letra = 162;
//    printf("LETRA: %c\n", letra);

    nodoA* ABBDiccionario = NULL;

    generarArbolBusqueda(&ABBDiccionario);

    mostrarArbol(ABBDiccionario);

    return 0;
}
