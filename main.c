/// TP FINAL - MOTOR DE BUSQUEDA      LABORATORIO II        LAUTARO MANZO - JULIAN CORONEL - TOBIAS NOYA


/// LIBRERIAS Y NOMBRES DE ARCHIVOS

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "gotoxy.h"
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

// Julian
void mostrarArbol(nodoA*);

// Tobi
void mostrarOcurrencias (nodoT*);


/// FUNCIONES

// DICCIONARIO - Lauti

/// ------------- Diccionario -------------
/// Se tienen N documentos, donde se leerán todas las palabras de cada uno de ellos.
/// Los documentos deben ser archivos de texto leídos como un binario (char)), sobre un vector que contendrá cada palabra,
/// id de documento al que pertenece, y posición en dicho documento.

int cantDatosArch(char* nomArch)
{
    FILE* puntArch = fopen(nomArch, "rb");

    int total = 0;

    if(puntArch != NULL)
    {
        fseek(puntArch, 0, SEEK_END);
        total = ftell(puntArch)/sizeof(char);
        fclose(puntArch);
    }
    return total;
}

void inicIdDoc()    // USAR UNA VEZ
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
        printf("Error\n");
    }
}

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

void leerArchivo(termino* terminos, int* validos, char* nomArch)
{
    FILE* puntArchIdDoc = fopen(idDocArch, "r+b");   // r+b
    FILE* puntArch = fopen(nomArch, "rb");

    int flag = 0;   // Para saber si se leyó un caracter raro

    int pos = 0, idDoc;
    char palabra[20], caracter;

    memset(palabra, 0, 20*sizeof(palabra[0]));  // Inicializamos el arreglo porque sino la primer palabra muestra caracteres raros

    if(puntArchIdDoc != NULL && puntArch != NULL)
    {
        fread(&idDoc, sizeof(int), 1, puntArchIdDoc);
        idDoc++;
        fseek(puntArchIdDoc, -1*sizeof(int), SEEK_CUR);
        fwrite(&idDoc, sizeof(int), 1, puntArchIdDoc);

        while(fread(&caracter, sizeof(char), 1, puntArch) > 0)
        {
//                    (65 a 90 MAYUSCULAS)                   (97 a 122 MINUSCULAS)
            if((caracter >= 65 && caracter <= 90) || (caracter >= 97 && caracter && 122))
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

                    memset(palabra, 0, 20*sizeof(palabra[0]));  // Reinicializa todas las celdas en 0
                }
            }
        }

        if(palabra[0] != 0)
        {
            strcpy(terminos[*validos].palabra, palabra);
            terminos[*validos].idDOC = idDoc;
            terminos[*validos].pos = pos;
            pos++;

            (*validos)++;
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
    FILE* puntArch = fopen("diccionario.bin", "ab");    // AB

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

/// MOTOR DE BUSQUEDA - Julian

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

//Inserta la ocurrencia en la lista, creando un nodo nuevo con los datos y luego lo inserta ordenado. También comprueba la posición
void insertarOcurrencia(nodoT** ocurrencias, nodoT* nuevo)
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
                insertarOcurrencia(&(*ocurrencias)->sig, nuevo);
        }
        else if (nuevo->idDOC < (*ocurrencias)->idDOC)
        {
            nuevo->sig = *ocurrencias;
            *ocurrencias = nuevo;
        }
        else
            insertarOcurrencia(&(*ocurrencias)->sig, nuevo);
    }
    else
        *ocurrencias = nuevo;
}

//Busca e inserta ordenada alfabéticamente la palabra en el árbol, junto con su nueva ocurrencia
void buscarEInsertar (nodoA** arbol, char* palabra, nodoT* ocurrencia)
{
    if (*arbol)
    {
        if (strcmpi((*arbol)->palabra, palabra) == 0)
        {
            (*arbol)->frecuencia++;
            insertarOcurrencia(&(*arbol)->ocurrencias, ocurrencia);
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

            buscarEInsertar(ABBDiccionario, registro.palabra, ocurrencia);
        }
        fclose(diccionario);
    }
    else
        printf("ERROR.\n");
}

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

// OPERACIONES DE USUARIO - Tobi

// 1) Buscar todas las apariciones de un término en algún documento (operación or).

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

// 2) Buscar todas las apariciones de un término en un documento y otro (operacion and).

int buscarIdDocEnOcurrencias (nodoT* ocurrencias, int idDoc)
{
    while (ocurrencias && ocurrencias->idDOC != idDoc)
        ocurrencias = ocurrencias->sig;

    return (ocurrencias != NULL);
}

void buscarPalabraEnDosDocumentosYMostrarOcurrencias (nodoA* arbol, char* palabra, int docA, int docB)
{
    if (arbol)
    {
        if (strcmpi(arbol->palabra, palabra) == 0)
        {
            if (buscarIdDocEnOcurrencias(arbol->ocurrencias, docA) && buscarIdDocEnOcurrencias(arbol->ocurrencias, docB))
            {
                printf("Se encontro %s en:\n", palabra);

                nodoT* aux = arbol->ocurrencias;

                while (aux)
                {
                    printf("===================\n");

                    if (aux->idDOC == docA || aux->idDOC == docB)
                    {
                        printf("ID DOC: %i\n", aux->idDOC);
                        printf("POS: %i\n", aux->pos);
                    }
                }

                printf("===================\n");
            }
            else
                printf("La palabra no se encontro en ID DOC %i e ID DOC %i\n", docA, docB);
        }
        else
        {
            if(strcmpi(arbol->palabra, palabra) < 0)
                buscarPalabraEnDosDocumentosYMostrarOcurrencias(arbol->der, palabra, docA, docB);
            else
                buscarPalabraEnDosDocumentosYMostrarOcurrencias(arbol->izq, palabra, docA, docB);
        }
    }
    else
        printf("NO SE ENCONTRO LA PALABRA '%s'.\n", palabra);
}

// 3) Buscar la aparición de más de un término en el mismo documento.

void mostrarOcurrenciasDeUnIdDoc (nodoT* ocurrencias, int idDoc)
{
    while (ocurrencias)
    {
        if (ocurrencias->idDOC == idDoc)
        {
            printf("===================\n");
            printf("POS: %i\n", ocurrencias->pos);
        }

        ocurrencias = ocurrencias->sig;
    }

    printf("===================\n\n");
}

nodoT* retornarListaOcurrenciasDeUnaPalabra (nodoA* arbol, char* palabra)
{
    if (arbol)
    {
        if (strcmpi(arbol->palabra, palabra) == 0)
            return arbol->ocurrencias;
        else
        {
            if (strcmpi(arbol->palabra, palabra) > 0)
                return retornarListaOcurrenciasDeUnaPalabra(arbol->izq, palabra);
            else
                return retornarListaOcurrenciasDeUnaPalabra(arbol->der, palabra);
        }
    }
    else
        return NULL;
}

int buscarPalabraEnArbol (nodoA* arbol, char* palabra)
{
    if (arbol)
    {
        if (strcmpi(arbol->palabra, palabra) == 0)
            return 1;
        else
        {
            if (strcmpi(arbol->palabra, palabra) < 0)
                return buscarPalabraEnArbol(arbol->der, palabra);
            else
                return buscarPalabraEnArbol(arbol->izq, palabra);
        }
    }
    else
        return 0;
}

void buscarPalabrasEnMismoDoc (nodoA* arbol, char* palabraA, char* palabraB, int idDoc)
{
    if (buscarPalabraEnArbol(arbol, palabraA) && buscarPalabraEnArbol(arbol, palabraB)) // si ambas palabras existen
    {
        nodoT* ocurrPalabraA = retornarListaOcurrenciasDeUnaPalabra(arbol, palabraA);
        nodoT* ocurrPalabraB = retornarListaOcurrenciasDeUnaPalabra(arbol, palabraB);

        if (buscarIdDocEnOcurrencias(ocurrPalabraA, idDoc) && buscarIdDocEnOcurrencias(ocurrPalabraB, idDoc)) // si las palabras se encuentran en el mismo ID DOC
        {
            printf("Ambas palabras se encuentran en ID DOC %i.\n", idDoc);

            printf("PALABRA: %s\n", palabraA);
            mostrarOcurrenciasDeUnIdDoc(ocurrPalabraA, idDoc);
            printf("PALABRA: %s\n", palabraB);
            mostrarOcurrenciasDeUnIdDoc(ocurrPalabraB, idDoc);
        }
        else
            printf("Ambas palabras se encuentran en distintos ID DOC.\n");
    }
    else
        printf("Alguna de las dos palabras no fue encontrada.\n");
}

// 4) Buscar una frase completa (las palabras deben estar contiguas en alguno de los documentos).

int retornarCantPalabras (char* frase)
{
    int i = 0, cant = 0;

    while (frase[i] != '\0')
    {
        if (frase[i] == ' ')
            cant++;
        i++;
    }
    cant++;

    return cant;
}

char** retornarArregloDeStrings (char* frase, int cantPalabras) // toma una frase en formato string y devuelve un arreglo de palabras
{
    int filas = cantPalabras, cols = 20; // cols = cant max de caracteres de términos (20)

    char** matriz = (char**)malloc(sizeof(char)*filas);

    for (int f = 0; f < filas; f++)
        matriz[f] = (char*)malloc(sizeof(char)*cols);

    int pos = 0, i = 0, j = 0;

    while (frase[pos] != '\0')
    {
        if (frase[pos] != ' ')
        {
            matriz[i][j] = frase[pos];
            j++;
        }
        else
        {
            matriz[i][j] = '\0';
            j = 0;
            i++;
        }

        pos++;
    }

    matriz[i][j] = '\0';

    return matriz;
}

int buscarIdDocYPosEnOcurrencias (nodoT* ocurrencias, int idDoc, int pos) // devuelve 1 o 0 si un idDoc y pos se encuentra en una lista de ocurrencias
{
    if (ocurrencias)
    {
        if (ocurrencias->idDOC == idDoc && ocurrencias->pos == pos)
            return 1;
        else
            return buscarIdDocYPosEnOcurrencias(ocurrencias->sig, idDoc, pos);
    }
    else
        return 0;
}

int buscarPalabraConIdDocYPos (nodoA* arbol, char* palabra, int idDoc, int pos) // devuelve 1 o 0 si una palabra con un idDoc y pos determinados se encuentra en el arbol
{
    if (arbol)
    {
        if (strcmpi(arbol->palabra, palabra) == 0)
            return buscarIdDocYPosEnOcurrencias(arbol->ocurrencias, idDoc, pos);
        else
        {
            if (strcmpi(arbol->palabra, palabra) > 0)
                return buscarPalabraConIdDocYPos(arbol->izq, palabra, idDoc, pos);
            else
                return buscarPalabraConIdDocYPos(arbol->der, palabra, idDoc, pos);
        }
    }
    else
        return 0;
}

void liberarMatrizChar (char** matriz, int filas) // libera matrices char
{
    for (int i = 0; i < filas; i++)
        free(matriz[i]);

    free(matriz);
}

void buscarFrase (nodoA* arbol, char* frase)
{
    int cantPalabras = retornarCantPalabras(frase);

    int i, pos, idDoc, flag = 0;

    // i: recorre arreglo de strings
    // flag: verifica si se encontró la frase

    char** palabras = retornarArregloDeStrings(frase, cantPalabras); // se crea un arreglo de strings con las palabras de la frase

    nodoT* ocurrs1erPalabra = retornarListaOcurrenciasDeUnaPalabra(arbol, palabras[0]); // se guarda la lista de ocurrencias de la primer palabra

    while (ocurrs1erPalabra && !flag) // verifica si las demás palabras de la frase pueden ser contiguas a alguna ocurrencia de la primer palabra
    {
        pos = ocurrs1erPalabra->pos + 1; // posicion de la ocurrencia (primer palabra) + 1, para verificar contiguidad de la segunda
        idDoc = ocurrs1erPalabra->idDOC; // idDoc de la ocurrencia

        i = 1; // reset i para recorrer nuevamente el arreglo de strings

        while (i < cantPalabras && buscarPalabraConIdDocYPos(arbol, palabras[i], idDoc, pos)) // verifica si todas las demás palabras de la frase se encuentran contiguas
        {
            i++;
            pos++; // aumenta pos para verificar contiguidad entre las palabras
        }

        if (i == cantPalabras) // si todas las palabras son contiguas (el anterior while cortó porque i == cantPalabras)
            flag = 1;
        else
            ocurrs1erPalabra = ocurrs1erPalabra->sig; // sino, seguir buscando con los IdDoc y Pos's de las demás ocurrencias de la primer palabra
    }

    liberarMatrizChar(palabras, cantPalabras);

    if (flag) // si se encontró la frase
        printf("SE ENCONTRO LA FRASE EN EL DOCUMENTO %i, A PARTIR DE LA POS %i.\n", ocurrs1erPalabra->idDOC, ocurrs1erPalabra->pos);
    else
        printf("NO SE ENCONTRO LA FRASE '%s'.\n", frase);
}

// 5) Buscar la palabra de más frecuencia que aparece en alguno de los docs.

nodoA* buscarTerminoMasFrecuente (nodoA* arbol)
{
    if (arbol)
    {
        nodoA* auxIzq = buscarTerminoMasFrecuente(arbol->izq);
        nodoA* auxDer = buscarTerminoMasFrecuente(arbol->der);

        if (auxIzq && auxDer)
        {
            if (arbol->frecuencia > auxIzq->frecuencia && arbol->frecuencia > auxDer->frecuencia)
                return arbol;
            else if (auxIzq->frecuencia > arbol->frecuencia && auxIzq->frecuencia > auxDer->frecuencia)
                return auxIzq;
            else
                return auxDer;
        }
        else if (auxIzq)
        {
            if (arbol->frecuencia > auxIzq->frecuencia)
                return arbol;
            else
                return auxIzq;
        }
        else if (auxDer)
        {
            if (arbol->frecuencia > auxDer->frecuencia)
                return arbol;
            else
                return auxDer;
        }
        else
            return arbol;

    }
    else
        return NULL;
}

void mostrarTerminoMasFrecuente (nodoA* arbol)
{
    if (arbol)
    {
        nodoA* terminoMasFrec = buscarTerminoMasFrecuente(arbol);
        nodoT* ocurrencias = terminoMasFrec->ocurrencias;

        printf("EL TERMINO MAS FRECUENTE ES '%s'.\n", terminoMasFrec->palabra);
        printf("LISTA DE OCURRENCIAS:\n");

        while(ocurrencias)
        {
            printf("===================\n");
            printf("ID DOC: %i\n", ocurrencias->idDOC);
            printf("POS: %i\n", ocurrencias->pos);
            ocurrencias = ocurrencias->sig;
        }
        printf("===================\n");
    }
    else
        printf("EL ARBOL ESTA VACIO.\n");
}

// 6) Utilizar la distancia de levenshtein en el ingreso de una palabra y sugerir similares a partir de una distancia <= 3.

int Minimo (int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

int Levenshtein(char *s1,char *s2)
{
    int t1,t2,i,j,*m,costo,res,ancho;

// Calcula tamanios strings
    t1=strlen(s1);
    t2=strlen(s2);

// Verifica que exista algo que comparar
    if (t1==0) return(t2);
    if (t2==0) return(t1);
    ancho=t1+1;

// Reserva matriz con malloc                     m[i,j] = m[j*ancho+i] !!
    m=(int*)malloc(sizeof(int)*(t1+1)*(t2+1));
    if (m==NULL) return(-1); // ERROR!!

// Rellena primera fila y primera columna
    for (i=0; i<=t1; i++) m[i]=i;
    for (j=0; j<=t2; j++) m[j*ancho]=j;

// Recorremos resto de la matriz llenando pesos
    for (i=1; i<=t1; i++) for (j=1; j<=t2; j++)
        {
            if (s1[i-1]==s2[j-1]) costo=0;
            else costo=1;
            m[j*ancho+i]=Minimo(Minimo(m[j*ancho+i-1]+1,     // Eliminacion
                                       m[(j-1)*ancho+i]+1),              // Insercion
                                m[(j-1)*ancho+i-1]+costo);
        }      // Sustitucion

// Devolvemos esquina final de la matriz
    res=m[t2*ancho+t1];
    free(m);
    return(res);
}

void sugerirSimilares (nodoA* arbol, char* palabra)
{
    if (arbol)
    {
        if (Levenshtein(arbol->palabra, palabra) <= 3)
        {
            printf("===================\n");
            printf("%s\n", arbol->palabra);
        }
        sugerirSimilares(arbol->izq, palabra);
        sugerirSimilares(arbol->der, palabra);
    }
}

// 7) añadir texto

int retornarCantCaracteres (char* string)
{
    int i = 0, cant = 0;

    while (string[i] != '\0')
    {
        cant++;
        i++;
    }

    return cant;
}

void llenarArregloDeTerminosEIncrementarIdDoc (termino* terminos, int* validos, char* texto)
{
    FILE* fp = fopen(idDocArch, "r+b");

    if (fp)
    {
        int i = 0, pos = 0, idDoc, flag = 0;

        fread(&idDoc, sizeof(int), 1, fp);
        idDoc++;
        fseek(fp, -1*sizeof(int), SEEK_CUR);    /// se incrementa idDoc
        fwrite(&idDoc, sizeof(int), 1, fp);

        char* palabra = (char*)malloc(sizeof(char)*20), caracter;

        while(texto[i] != '\0')
        {
            caracter = texto[i];
//                    (65 a 90 MAYUSCULAS)                   (97 a 122 MINUSCULAS)
            if((caracter >= 65 && caracter <= 90) || (caracter >= 97 && caracter && 122))
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

                    palabra[0] = '\0';
                }
            }

            i++;
        }

        agregaCaracterAPalabra(palabra, '\0');
        strcpy(terminos[*validos].palabra, palabra);
        terminos[*validos].idDOC = idDoc;
        terminos[*validos].pos = pos;
        (*validos)++;

        free(palabra);

        fclose(fp);
    }
    else
        printf("ERROR.\n");
}

void recorrerArregloTerminosYAgregarAArbol(nodoA** arbol, termino* terminos, int validos)
{
    for (int i = 0; i < validos; i++)
    {
        printf("%s\n", terminos[i].palabra);
        buscarEInsertar(arbol, terminos[i].palabra, crearNodoOcurrencias(terminos[i].idDOC, terminos[i].pos));
    }
}

void agregarTexto (nodoA** arbol)
{
    char* texto = (char*)malloc(sizeof(char)*1000);

    printf("INGRESE EL TEXTO QUE QUIERA AGREGAR AL DICCIONARIO (MAXIMO 1000 CARACTERES):\n");
    scanf("%s", texto);
    fflush(stdin);            // se lee el texto en un string de tamaño 1000

    if (retornarCantPalabras(texto) > 0) // si el usuario escribio por lo menos una palabra
    {
        int validos = 0; // validos del arreglo de terminos

        termino* terminos = (termino*)malloc(sizeof(termino)*retornarCantCaracteres(texto));

        llenarArregloDeTerminosEIncrementarIdDoc(terminos, &validos, texto); // llena el arreglo de terminos con las palabras del texto

        escrituraDiccionario(terminos, validos); /// añade todos los terminos del arreglo al diccionario

        recorrerArregloTerminosYAgregarAArbol(arbol, terminos, validos); // añade todos los terminos al arbol

        free(terminos);
    }
    else
        printf("EL TEXTO TIENE 0 PALABRAS.\n");

    free(texto);
}

// 8) menú

void mostrarOpciones()
{

}

void menu (nodoA** arbol)
{
    char opcion;

    printf("INGRESE UNA OPCION:");

    printf("(1) Buscar todas las apariciones de un término en algún documento.");
    printf("(2) Buscar todas las apariciones de un término en un documento y otro.");
    printf("(3) Buscar la aparición de más de un término en el mismo documento.");
    printf("(4) Buscar una frase completa.");
    printf("(5) Buscar la palabra mas frecuente.");
    printf("(6) Sugerir palabras similares.");
    printf("(7) Inyectar texto.");

    opcion = getchar();

    switch(opcion)
    {
    case '1':
        {

            break;
        }
    case '2':
        {

            break;
        }
    case '3':
        {

            break;
        }
    case '4':
        {

            break;
        }
    case '':
        {

            break;
        }
    default:
        {
            printf("OPCION INCORRECTA");
            system("pause");
            system("cls");
            menu(arbol);
            break;
        }
    }
}


// interfaz

void portada()
{
    int x = 20, y = 4;

    gotoxy(x, y++);
    printf("  __  __  ____ _______ ____  _____\n");
    gotoxy(x, y++);
    printf(" |  \\/  |/ __ \\__   __/ __ \\|  __ \\ \n");
    gotoxy(x, y++);
    printf(" | \\  / | |  | | | | | |  | | |__) | \n");
    gotoxy(x, y++);
    printf(" | |\\/| | |  | | | | | |  | |  _  /\n");
    gotoxy(x, y++);
    printf(" | |  | | |__| | | | | |__| | | \\ \\ \n");
    gotoxy(x, y++);
    printf(" |_|  |_|\\____/  |_|  \\____/|_|  \\_\\ \n");
    gotoxy(x, y++);
    printf("                      _____  ______ \n");
    gotoxy(x, y++);
    printf("                     |  __ \\|  ____| \n");
    gotoxy(x, y++);
    printf("                     | |  | | |__ \n");
    gotoxy(x, y++);
    printf("                     | |  | |  __|\n");
    gotoxy(x, y++);
    printf("                     | |__| | |____ \n");
    gotoxy(x, y++);
    printf("                     |_____/|______|\n");
    gotoxy(x, y++);
    printf("                      ____  _    _  _____  ____  _    _ ______ _____          \n");
    gotoxy(x, y++);
    printf("                     |  _ \\| |  | |/ ____|/ __ \\| |  | |  ____|  __ \\   /\\ \n");
    gotoxy(x, y++);
    printf("                     | |_) | |  | | (___ | |  | | |  | | |__  | |  | | /  \\ \n");
    gotoxy(x, y++);
    printf("                     |  _ <| |  | |\\___ \\| |  | | |  | |  __| | |  | |/ /\\ \\ \n");
    gotoxy(x, y++);
    printf("                     | |_) | |__| |____) | |__| | |__| | |____| |__| / ____ \\ \n");
    gotoxy(x, y++);
    printf("                     |____/ \\____/|_____/ \\___\\_\\\\____/|______|_____/_/    \\_\\ \n");

    gotoxy(x+18, y+3);
    system("pause");
    system("cls");
}

void mostrarArregloTerminos (termino* arreglo, int validos)
{
    printf("------------------ ARREGLO DINAMICO DE TERMINOS ------------------\n");
    for (int i = 0; i < validos; i++)
    {
        printf("____________________________\n");
        printf("Palabra: %s \n", arreglo[i].palabra);
        printf("Id: %i \n", arreglo[i].idDOC);
        printf("Posicion: %i \n", arreglo[i].pos);
        printf("____________________________\n");
    }
}

void insertarTextosBase()
{
    inicIdDoc();

    termino* arreglo;
    int validos = 0;
    arreglo = (termino*)malloc(sizeof(termino)*cantDatosArch(boca));

    leerArchivo(arreglo, &validos, boca);
    escrituraDiccionario(arreglo, validos);

    //mostrarArregloTerminos(arreglo, validos);

    free(arreglo);

    arreglo = (termino*)malloc(sizeof(termino)*cantDatosArch(leng));
    validos = 0;
    leerArchivo(arreglo, &validos, leng);
    escrituraDiccionario(arreglo, validos);

    //mostrarArregloTerminos(arreglo, validos);

    free(arreglo);
}

/// MAIN

int main()
{
    //insertarTextosBase();

    printf("------------------ ARCHIVO DICCIONARIO ------------------\n");

    leer();
    system("pause");
    system("cls");

    portada();

    nodoA* arbolBB = NULL;

    generarArbolBusqueda(&arbolBB);

    mostrarArbol(arbolBB);

    system("pause");
    system("cls");

    //mostrarTerminoMasFrecuente(arbolBB);

    //buscarFrase(arbolBB, "en el sentido");

    //agregarTexto(&arbolBB);

    return 0;
}
