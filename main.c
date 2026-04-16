#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CATALOGO_OPERADORES "+-*#~()"

typedef struct ArbolConjuntos ArbolConjuntos;
typedef struct ArbolDetalleConjunto ArbolDetalleConjunto;
typedef struct ArbolExpresiones ArbolExpresiones;

typedef struct Item Item;
typedef struct ListaItems ListaItems;



struct Item {
    int TipoDato;
    char nombre [999];
};

struct ListaItems {
    Item item[999];
    int cantidadItems;
};

struct ArbolConjuntos {
    char nombre[20];
    ArbolConjuntos *nodoIzquierda;
    ArbolConjuntos *nodoDerecha;
    ArbolDetalleConjunto *detalleNodo;
    ArbolExpresiones *detalleExpresion;
};

struct ArbolDetalleConjunto {
    char nombre[20];
    ArbolDetalleConjunto *nodoIzquierda;
    ArbolDetalleConjunto *nodoDerecha;

};

struct ArbolExpresiones {
    char operador;
    char nombreConjunto[999];
    int tipoDato;     // 0 - Operador, 1 - Conjunto, 2 - ELEMENTOS, 3 - NOMBRE CONJUNTO

    ArbolExpresiones *nodoIzquierda;
    ArbolExpresiones *nodoDerecha;
};

ArbolConjuntos *arbolMaestro = NULL;
ArbolDetalleConjunto *universo = NULL;

ArbolDetalleConjunto* fnInsertaElementoDetalle(ArbolDetalleConjunto *raiz, char valor[]) {

    if (raiz == NULL) {
        ArbolDetalleConjunto *nuevo =
            calloc(1, sizeof(ArbolDetalleConjunto));

        strcpy(nuevo->nombre, valor);
        return nuevo;
    }

    int cmp = strcmp(valor, raiz->nombre);

    if (cmp < 0)
        raiz->nodoIzquierda =
            fnInsertaElementoDetalle(raiz->nodoIzquierda, valor);

    else if (cmp > 0)
        raiz->nodoDerecha =
            fnInsertaElementoDetalle(raiz->nodoDerecha, valor);

    return raiz;
}

ArbolDetalleConjunto* fnObtieneListaElementos(char lista[]) {

    ArbolDetalleConjunto *arbol = NULL;

    char tmp[50];
    int j = 0;

    for (int i = 0; lista[i] != '\0'; i++) {

        if (lista[i] == ',' || lista[i] == ' ') {

            if (j > 0) {
                tmp[j] = '\0';
                arbol = fnInsertaElementoDetalle(arbol, tmp);
                j = 0;
            }
            continue;
        }

        tmp[j++] = lista[i];
    }

    if (j > 0) {
        tmp[j] = '\0';
        arbol = fnInsertaElementoDetalle(arbol, tmp);
    }

    return arbol;
}

ArbolConjuntos *fnBuscaArbolConjunto(ArbolConjuntos *arbol, char conjunto[999]) {

    if (arbol == NULL) {
        return NULL;
    }

    conjunto[strcspn(conjunto, "\n")] = '\0';
    int valor = strcmp(conjunto, arbol->nombre);

    if (valor == 0) {
        return arbol;
    }
    if (valor < 0) {
        return fnBuscaArbolConjunto(arbol->nodoIzquierda, conjunto);
    } else {
        return fnBuscaArbolConjunto(arbol->nodoDerecha, conjunto);
    }
}

ArbolExpresiones* fnConstruyeArbolExpresion(ListaItems *items) {

    ArbolExpresiones *pila[999];
    int top = -1;

    for (int i = 0; i < items->cantidadItems; i++) {


        if (items->item[i].TipoDato == 3 || items->item[i].nombre[0] == '=' || items->item[i].nombre[0] == '\n') {
            continue;
        }

        ArbolExpresiones *nuevo = calloc(1, sizeof(ArbolExpresiones));

        nuevo->tipoDato = items->item[i].TipoDato;

        if (items->item[i].TipoDato == 0) {

            nuevo->operador = items->item[i].nombre[0];

            // SI ES OPERADOR, SE DEBEN MANDAR A NODO IZQUIERDO Y DERECHO CADA EXPRESION PREVIA AL OPERADOR
            nuevo->nodoDerecha = pila[top--];
            nuevo->nodoIzquierda = pila[top--];
            nuevo->tipoDato = items->item[i].TipoDato;
            pila[++top] = nuevo;
        }
        else {
            strcpy(nuevo->nombreConjunto, items->item[i].nombre);

            pila[++top] = nuevo;
        }
    }

    return pila[top];
}

bool fnCargaConjuntos(ArbolConjuntos **arbol, char conjunto[999]) {
    ArbolConjuntos *nuevoArbol = malloc(sizeof(ArbolConjuntos));
    memset(nuevoArbol, 0, sizeof(ArbolConjuntos));
    strcpy(nuevoArbol->nombre, conjunto);

    // SE VALIDA SI NO TIENE RAIZ
    if (*arbol == NULL) {
        ArbolConjuntos *nuevo = malloc(sizeof(ArbolConjuntos));
        memset(nuevo, 0, sizeof(ArbolConjuntos));
        strcpy(nuevo->nombre, conjunto);

        *arbol = nuevo;
        return true;
    }

    int valor = strcmp(conjunto, (*arbol)->nombre);

    if (valor < 0) {
        fnCargaConjuntos(&(*arbol)->nodoIzquierda, conjunto);
    } else {
        fnCargaConjuntos(&(*arbol)->nodoDerecha, conjunto);
    }

    return false;
}

ArbolDetalleConjunto* copiarDetalle(ArbolDetalleConjunto *raiz) {

    if (raiz == NULL) {
        return NULL;
    }

    ArbolDetalleConjunto *nuevo =
        calloc(1, sizeof(ArbolDetalleConjunto));

    strcpy(nuevo->nombre, raiz->nombre);

    nuevo->nodoIzquierda =
        copiarDetalle(raiz->nodoIzquierda);

    nuevo->nodoDerecha =
        copiarDetalle(raiz->nodoDerecha);

    return nuevo;
}

ArbolDetalleConjunto* insertarDetalle(ArbolDetalleConjunto *raiz, char valor[]) {
    if (raiz == NULL) {
        ArbolDetalleConjunto *nuevo =
            calloc(1, sizeof(ArbolDetalleConjunto));

        strcpy(nuevo->nombre, valor);
        return nuevo;
    }

    int cmp = strcmp(valor, raiz->nombre);

    if (cmp < 0)
        raiz->nodoIzquierda =
            insertarDetalle(raiz->nodoIzquierda, valor);

    else if (cmp > 0)
        raiz->nodoDerecha =
            insertarDetalle(raiz->nodoDerecha, valor);
    else
        return raiz; // evita duplicados

    return raiz;
}

void fnCargaUniverso(ArbolDetalleConjunto *raiz) {
    if (!raiz) return;

    universo = insertarDetalle(universo, raiz->nombre);

    fnCargaUniverso(raiz->nodoIzquierda);
    fnCargaUniverso(raiz->nodoDerecha);
}

bool existeDetalle(ArbolDetalleConjunto *raiz, char valor[]) {
    if (raiz == NULL) return false;

    int cmp = strcmp(valor, raiz->nombre);

    if (cmp == 0) return true;
    if (cmp < 0) return existeDetalle(raiz->nodoIzquierda, valor);

    return existeDetalle(raiz->nodoDerecha, valor);
}

void recorrerInsertar(ArbolDetalleConjunto *origen, ArbolDetalleConjunto **destino) {
    if (origen == NULL) return;

    if (!existeDetalle(*destino, origen->nombre)) {
        *destino = insertarDetalle(*destino, origen->nombre);
    }

    recorrerInsertar(origen->nodoIzquierda, destino);
    recorrerInsertar(origen->nodoDerecha, destino);
}



ArbolDetalleConjunto* unionArboles( ArbolDetalleConjunto *a, ArbolDetalleConjunto *b) {
    ArbolDetalleConjunto *res = NULL;

    recorrerInsertar(a, &res);
    recorrerInsertar(b, &res);

    return res;
}

void interseccionRec(ArbolDetalleConjunto *a, ArbolDetalleConjunto *b, ArbolDetalleConjunto **res
) {
    if (a == NULL) return;

    if (existeDetalle(b, a->nombre))
        *res = insertarDetalle(*res, a->nombre);

    interseccionRec(a->nodoIzquierda, b, res);
    interseccionRec(a->nodoDerecha, b, res);
}

ArbolDetalleConjunto* interseccionArboles(ArbolDetalleConjunto *a, ArbolDetalleConjunto *b) {
    ArbolDetalleConjunto *res = NULL;

    interseccionRec(a, b, &res);

    return res;
}

void diferenciaRec(ArbolDetalleConjunto *a, ArbolDetalleConjunto *b, ArbolDetalleConjunto **res) {
    if (a == NULL) return;

    if (!existeDetalle(b, a->nombre))
        *res = insertarDetalle(*res, a->nombre);

    diferenciaRec(a->nodoIzquierda, b, res);
    diferenciaRec(a->nodoDerecha, b, res);
}

ArbolDetalleConjunto* diferenciaArboles(ArbolDetalleConjunto *a, ArbolDetalleConjunto *b) {
    ArbolDetalleConjunto *res = NULL;

    diferenciaRec(a, b, &res);

    return res;
}

ArbolDetalleConjunto* diferenciaSimetrica(ArbolDetalleConjunto *a,ArbolDetalleConjunto *b) {
    ArbolDetalleConjunto *u = unionArboles(a, b);
    ArbolDetalleConjunto *i = interseccionArboles(a, b);

    return diferenciaArboles(u, i);
}

ArbolDetalleConjunto* complemento(ArbolDetalleConjunto *a) {
    return diferenciaArboles(universo, a);
}

ArbolDetalleConjunto* fnEvaluaExpresion(ArbolExpresiones *nodo) {

    if (nodo == NULL) {
        return NULL;
    }

    // NOMBRE DEL CONJUNTO (lado izquierdo de =)
    if (nodo->tipoDato == 3) {
        return NULL;
    }

    // SI ES CONJUNTO
    if (nodo->tipoDato == 1) {

        ArbolConjuntos *tmpArbolConjunto = fnBuscaArbolConjunto(arbolMaestro, nodo->nombreConjunto);

        if (tmpArbolConjunto != NULL)

            return copiarDetalle(tmpArbolConjunto->detalleNodo);

        return NULL;
    }

    // SI SON ELEMENTOS {A,B,C}
    if (nodo->tipoDato == 2) {
        return fnObtieneListaElementos(nodo->nombreConjunto);
    }

    // OPERADOR
    ArbolDetalleConjunto *izq =
        fnEvaluaExpresion(nodo->nodoIzquierda);

    ArbolDetalleConjunto *der =
        fnEvaluaExpresion(nodo->nodoDerecha);

    switch (nodo->operador) {

        case '+': return unionArboles(izq, der);
        case '*': return interseccionArboles(izq, der);
        case '-': return diferenciaArboles(izq, der);
        case '#': return diferenciaSimetrica(izq, der);
        case '~': return complemento(izq);
    }

    return NULL;
}

void fnRecorreArbolMaestro(ArbolConjuntos *arbol) {

    if (arbol == NULL) {
        return;
    }

    fnRecorreArbolMaestro(arbol->nodoIzquierda);
    fnRecorreArbolMaestro(arbol->nodoDerecha);

    if (arbol->detalleExpresion != NULL) {
        arbol->detalleNodo =
            fnEvaluaExpresion(arbol->detalleExpresion);
    }
}


// void fnInsertaItem(char tmpValor[999], int TipoDato, ListaItems *items, int cantidadItems, int *iterador) {
//     Item item2;
//     item2.TipoDato = TipoDato;
//     strcpy(item2.nombre, tmpValor);
//     items->item[cantidadItems] = item2;
//     cantidadItems++;
//     memset(tmpValor, 0, sizeof(tmpValor));
//     iterador = 0;
// }

/*
* Unión: +
Intersección: *
Diferencia: -
Diferencia Simétrica: #
Complemento: ~
 *
 */

char jerarquia[8][2] = {
    {'+', '1'},
    {'-', '1'},
    {'#', '1'},
    {'*', '2'},
    {'~', '9'},
    {'\0', '\0'}
};

int fnObtieneJerarquia(char operador[999]) {
    for (int i = 0; jerarquia[i][0] != '\0'; i++) {
        if (jerarquia[i][0] == operador[0]) {
            return jerarquia[i][1];
        }
    }
    return 0;
}

ListaItems* fnObtieneItemsPostFijo(ListaItems *items) {
    Item pila[999];
    int cabeza = -1;
    int j = 0;
    ListaItems *lista_Items = calloc(1, sizeof(ListaItems));

    for (int i = 0; i < items->cantidadItems; i++) {
        // SI NO ES OPERADOR, SE INSERTA DE FORMA NORMAL
        if (items->item[i].TipoDato != 0) {
            lista_Items->item[j++] = items->item[i];
        }
        else {
            // if (items->item[i].nombre[0] == '(' || items->item[i].nombre[0] == ')') {
            //     continue;
            // }

            while (
                cabeza >= 0 &&
                fnObtieneJerarquia(items->item[i].nombre) <=
                fnObtieneJerarquia(pila[cabeza].nombre)
            ) {
                lista_Items->item[j++] = pila[cabeza--];
            }

            pila[++cabeza] = items->item[i];

        }
    }

    while (cabeza >= 0) {
        lista_Items->item[j++] = pila[cabeza--];
    }
    lista_Items->cantidadItems = j;
    return lista_Items;
}

void fnObtieneItems(char lista_conjunto[999], ListaItems *items) {

    bool iniciaConjunto = false;
    char tmpValor[999];
    memset(tmpValor, 0, sizeof(tmpValor));

    bool nombreConjunto = true;

    int j = 0;
    int cantidadItems = 0;
    if (items->cantidadItems < 0) {
        items->cantidadItems = 0;
    } else {
        cantidadItems = items->cantidadItems;
    }

    for (int i = 0; lista_conjunto[i] != '\0'; i++) {
        if (lista_conjunto[i] == ' ') {
            continue;
        }

        if (nombreConjunto) {
            if (lista_conjunto[i] == '=') {
                Item item  = {0};
                Item item2  = {0};
                item.TipoDato = 3;
                strcpy(item.nombre, tmpValor);

                item2.TipoDato = 0;
                item2.nombre[0] = lista_conjunto[i];
                item2.nombre[1] = '\0';

                items->item[cantidadItems] = item;
                cantidadItems++;
                items->item[cantidadItems] = item2;
                cantidadItems++;
                nombreConjunto = false;
                memset(tmpValor, 0, sizeof(tmpValor));
                j = 0;
                continue;
            }
            tmpValor[j] = lista_conjunto[i];
            j++;
            continue;
        }

        // SE INGRESA EL CONJUNTO COMO UN SOLO ITEM
        if (lista_conjunto[i] == '{') {
            iniciaConjunto = true;
            memset(tmpValor, 0, sizeof(tmpValor));
            j = 0;
            continue;
        }

        // VALIDAR SI ES OPERADOR
        if (strchr(CATALOGO_OPERADORES, lista_conjunto[i]) != NULL) {
            // SI ANTES DEL OPERADOR HAY UNA CADENA, SE ALMACENA LA CADENA COMO UN CONJUNTO
            if (j > 0) {
                Item item  = {0};
                item.TipoDato = 1;
                strncpy(item.nombre, tmpValor, sizeof(item.nombre)-1);
                item.nombre[sizeof(item.nombre)-1] = '\0';
                items->item[cantidadItems] = item;
                cantidadItems++;
                j = 0;
            }
            memset(tmpValor, 0, sizeof(tmpValor));
            Item item2 = {0};
            item2.TipoDato = 0;
            item2.nombre[0] = lista_conjunto[i];
            items->item[cantidadItems] = item2;
            cantidadItems++;
            continue;
        }

        // ALMACENA CONJUNTOS
        if (iniciaConjunto) {
            if (lista_conjunto[i] == '}') {
                Item item  = {0};
                item.TipoDato = 2;
                strcpy(item.nombre, tmpValor);
                iniciaConjunto = false;
                items->item[cantidadItems] = item;
                cantidadItems++;
                memset(tmpValor, 0, sizeof(tmpValor));

                j = 0;
                continue;
            }

            tmpValor[j] = lista_conjunto[i];
            j++;
            continue;
        }

        // SE OBTIENE EL NOMBRE DE LOS CONJUNTOS
        tmpValor[j] = lista_conjunto[i];
        j++;
    }

    if (j > 0) {
        Item item  = {0};
        item.TipoDato = 1;
        strcpy(item.nombre, tmpValor);
        items->item[cantidadItems] = item;
        cantidadItems++;
    }
    items->cantidadItems = cantidadItems;
}


void imprimirArbol(ArbolDetalleConjunto *raiz, char *prefijo, int esIzq, FILE *out) {
    if (raiz == NULL) return;

    fprintf(out, "%s", prefijo);
    fprintf(out, "%s", esIzq ? "├── " : "└── ");
    fprintf(out, "%s\n", raiz->nombre);

    char nuevoPrefijo[1000];
    strcpy(nuevoPrefijo, prefijo);
    strcat(nuevoPrefijo, esIzq ? "│   " : "    ");

    imprimirArbol(raiz->nodoIzquierda, nuevoPrefijo, 1, out);
    imprimirArbol(raiz->nodoDerecha, nuevoPrefijo, 0, out);
}

void opcionArbol(FILE *out, ArbolConjuntos *arbol, char *nombre) {

    ArbolConjuntos *nodo = fnBuscaArbolConjunto(arbol, nombre);

    if (!nodo) return;

    fprintf(out,"Arbol(%s)\n", nombre);
    imprimirArbol(nodo->detalleNodo, "", 0, out);
}

void fnImprimeArbolMaestro(ArbolConjuntos *arbol, char *prefijo, FILE *source, bool izquierda) {
    if (!arbol) return;

    fprintf(source, "%s", prefijo);
    fprintf(source, "%s", izquierda ? "├── " : "└── ");
    fprintf(source, "%s\n", arbol->nombre);

    char nuevoPrefijo[1000];
    strcpy(nuevoPrefijo, prefijo);
    strcat(nuevoPrefijo, izquierda ? "│   " : "    ");

    fnImprimeArbolMaestro(arbol->nodoIzquierda, nuevoPrefijo, source, true);
    fnImprimeArbolMaestro(arbol->nodoDerecha, nuevoPrefijo, source, false);

}

void fnOpcionArbolMaestro(FILE *source, ArbolConjuntos *arbol) {

    if (!arbol) return;

    fprintf(source,"\nArbol(Maestro)\n");
    fnImprimeArbolMaestro(arbol, "", source, false);
}

void imprimirElementos(ArbolDetalleConjunto *raiz, FILE *out, bool *esPrimero) {
    if (raiz == NULL) return;

    imprimirElementos(raiz->nodoIzquierda, out, esPrimero);

    if (!*esPrimero) {
        fprintf(out, ", ");
    }
    fprintf(out, "%s", raiz->nombre);
    *esPrimero = false;

    imprimirElementos(raiz->nodoDerecha, out, esPrimero);
}

void fnImprimeElementoConjunto(FILE *source, ArbolConjuntos arbol, char nombre[999]) {
    if (!&arbol) return;

    ArbolConjuntos *arbolPrincipal = fnBuscaArbolConjunto(&arbol, nombre);
    if (arbolPrincipal != NULL) {
        fprintf(source, "\nElementos Arbol: %s -> {", arbolPrincipal->nombre);
        bool esPrimero = true;
        imprimirElementos(arbolPrincipal->detalleNodo, source, &esPrimero);
        fprintf(source, "}");
    }
}

void fnLiberaNodo(ArbolDetalleConjunto *raiz) {
    if (!raiz) return;

    fnLiberaNodo(raiz->nodoIzquierda);
    fnLiberaNodo(raiz->nodoDerecha);
    free(raiz);
}

void fnLiberaExpresion(ArbolExpresiones *raiz) {
    if (!raiz) return;

    fnLiberaExpresion(raiz->nodoIzquierda);
    fnLiberaExpresion(raiz->nodoDerecha);
    free(raiz);
}

ArbolConjuntos* minimoNodo(ArbolConjuntos *raiz) {
    while (raiz && raiz->nodoIzquierda != NULL)
        raiz = raiz->nodoIzquierda;

    return raiz;
}

ArbolConjuntos* borrarConjunto(ArbolConjuntos *raiz, char nombre[]) {

    if (raiz == NULL) return NULL;

    int cmp = strcmp(nombre, raiz->nombre);

    if (cmp < 0) {
        raiz->nodoIzquierda = borrarConjunto(raiz->nodoIzquierda, nombre);
    }
    else if (cmp > 0) {
        raiz->nodoDerecha = borrarConjunto(raiz->nodoDerecha, nombre);
    }
    else {

        fnLiberaNodo(raiz->detalleNodo);
        fnLiberaExpresion(raiz->detalleExpresion);

        if (raiz->nodoIzquierda == NULL && raiz->nodoDerecha == NULL) {
            free(raiz);
            return NULL;
        }

        // caso 2: un hijo
        if (raiz->nodoIzquierda == NULL) {
            ArbolConjuntos *tmp = raiz->nodoDerecha;
            free(raiz);
            return tmp;
        }

        if (raiz->nodoDerecha == NULL) {
            ArbolConjuntos *tmp = raiz->nodoIzquierda;
            free(raiz);
            return tmp;
        }

        // caso 3: dos hijos
        ArbolConjuntos *min = minimoNodo(raiz->nodoDerecha);

        strcpy(raiz->nombre, min->nombre);
        raiz->detalleNodo = min->detalleNodo;
        raiz->detalleExpresion = min->detalleExpresion;

        raiz->nodoDerecha =
            borrarConjunto(raiz->nodoDerecha, min->nombre);
    }

    return raiz;
}

ArbolDetalleConjunto* borrarElementoDetalle(ArbolDetalleConjunto *raiz, char nombre[]){
    if (!raiz) return NULL;

    int cmp = strcmp(nombre, raiz->nombre);

    if (cmp < 0)
        raiz->nodoIzquierda =
            borrarElementoDetalle(raiz->nodoIzquierda, nombre);

    else if (cmp > 0)
        raiz->nodoDerecha =
            borrarElementoDetalle(raiz->nodoDerecha, nombre);

    else {
        // sin hijos
        if (!raiz->nodoIzquierda && !raiz->nodoDerecha){
            free(raiz);
            return NULL;
        }

        // un hijo
        if (!raiz->nodoIzquierda){
            ArbolDetalleConjunto *tmp = raiz->nodoDerecha;
            free(raiz);
            return tmp;
        }

        if (!raiz->nodoDerecha){
            ArbolDetalleConjunto *tmp = raiz->nodoIzquierda;
            free(raiz);
            return tmp;
        }

        // dos hijos
        ArbolDetalleConjunto *min = raiz->nodoDerecha;
        while(min->nodoIzquierda)
            min = min->nodoIzquierda;

        strcpy(raiz->nombre, min->nombre);

        raiz->nodoDerecha =
            borrarElementoDetalle(raiz->nodoDerecha, min->nombre);
    }

    return raiz;
}

void borrarElementoEnTodos(ArbolConjuntos *raiz, char elemento[]){
    if (!raiz) return;

    borrarElementoEnTodos(raiz->nodoIzquierda, elemento);

    raiz->detalleNodo =
        borrarElementoDetalle(raiz->detalleNodo, elemento);

    borrarElementoEnTodos(raiz->nodoDerecha, elemento);
}

void borrarElementoEnConjunto(ArbolConjuntos *arbol, char conjunto[], char elemento[]){
    ArbolConjuntos *nodo =
        fnBuscaArbolConjunto(arbol, conjunto);

    if (!nodo) return;

    nodo->detalleNodo =
        borrarElementoDetalle(nodo->detalleNodo, elemento);
}

void opcionBorrarElementos(ArbolConjuntos *arbol, char conjunto[], char elemento[]){
    // SI CONJUNTO VACIO -> BORRAR EN TODOS
    if (conjunto[0] == '*'){
        borrarElementoEnTodos(arbol, elemento);
    }
    else{
        borrarElementoEnConjunto(arbol, conjunto, elemento);
    }
}

void fnBorrarTodos(ArbolConjuntos *raiz) {
    if (!raiz) return;

    fnBorrarTodos(raiz->nodoIzquierda);
    fnBorrarTodos(raiz->nodoDerecha);

    fnLiberaNodo(raiz->detalleNodo);
    fnLiberaExpresion(raiz->detalleExpresion);

    free(raiz);
}

int main(void) {


    FILE *archivo;
    char lista_conjuntos[999][999];
    char nombreArchivo[999];
    char tmpNombreArchivo[999];

    printf("Ingrese nombre del archivo: ");
    scanf("%s", tmpNombreArchivo);
    sprintf(nombreArchivo, "../%s.txt", tmpNombreArchivo);


    archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("El archivo no fue encontrado en la raiz del proyecto");
        return 1;
    }

    printf("Iniciando el proceso de carga de archivo...");
    int i = 0;
    while (
        fgets(lista_conjuntos[i], 999, archivo) != NULL
    ) {
        int cmp = strcmp(lista_conjuntos[i], "\n");
        if (cmp == 0)
            continue;
        i++;
    }

    fclose(archivo);

    if (i == 0){
        printf("No se encontraron registros en archivo");
        return 1;
    }

    printf("Iniciando carga de conjuntos");

    // if(!fnCargaConjuntos(lista_conjuntos)) {
    //     printf("Ocurrió un error al realizar la carga de los conjuntos");
    // }

    ListaItems *listaItems = NULL;
    for (int i = 0; lista_conjuntos[i][0] != '\0'; i++) {
        ListaItems *items = calloc(1, sizeof(ListaItems));
        fnObtieneItems(lista_conjuntos[i], items);
        listaItems = fnObtieneItemsPostFijo(items);
        fnCargaConjuntos(&arbolMaestro, items->item[0].nombre);
        ArbolExpresiones *nuevoArbolExpresiones = fnConstruyeArbolExpresion(listaItems);
        ArbolConjuntos *tmpArbolConjunto = fnBuscaArbolConjunto(arbolMaestro, items->item[0].nombre);

        if (tmpArbolConjunto != NULL) {
            tmpArbolConjunto->detalleExpresion = nuevoArbolExpresiones;
            tmpArbolConjunto->detalleNodo = fnEvaluaExpresion(tmpArbolConjunto->detalleExpresion);
            fnCargaUniverso(tmpArbolConjunto->detalleNodo);
        }


        // printf("=================== NUEVO CONJUNTO ========================== \n");
        // for (int i = 0; i < items->cantidadItems; i++) {
        //     printf("%s \n", items->item[i].nombre);
        //     break;
        // }
    }

    //fnRecorreArbolMaestro(arbolMaestro);

    char nombreSalida[999] = "";
    sprintf(nombreSalida, "../%s.OUT.txt", tmpNombreArchivo);
    FILE *archivoSalida = fopen(nombreSalida, "w");
    int opcion;
    char conjunto[50];
    char elemento[50];

    do {
        printf("\n===== MENU =====\n");
        printf("1. Arbol(Conjunto)\n");
        printf("2. Elementos(Conjunto)\n");
        printf("3. Conjuntos()\n");
        printf("4. Borrar(Conjunto)\n");
        printf("5. Borrar_Elemento(Conjunto, elemento)\n");
        printf("6. Borrar_Conjuntos()\n");
        printf("7. Salir\n");
        printf("Seleccione opcion: ");
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                printf("Conjunto: ");
                scanf("%s", conjunto);
                opcionArbol(archivoSalida, arbolMaestro, conjunto);
                printf("Cargado correctamente en archivo de salida!!");
                break;

            case 2:
                printf("Conjunto: ");
                scanf("%s", conjunto);
                fnImprimeElementoConjunto(archivoSalida, *arbolMaestro, conjunto);
                printf("Cargado correctamente en archivo de salida!!");
                break;

            case 3:
                fnOpcionArbolMaestro(archivoSalida, arbolMaestro);
                printf("Cargado correctamente en archivo de salida!!");
                break;

            case 4:
                printf("Conjunto a borrar: ");
                scanf("%s", conjunto);
                borrarConjunto(arbolMaestro, conjunto);
                printf("Se ha borrado el conjunto!!");
                break;

            case 5:
                printf("Conjunto (* para todos): ");
                scanf("%s", conjunto);

                printf("Elemento: ");
                scanf("%s", elemento);

                opcionBorrarElementos(arbolMaestro, conjunto, elemento);
                printf("Elemento borrado correctamente");
                break;

            case 6:
                fnBorrarTodos(arbolMaestro);
                arbolMaestro = NULL;
                printf("Se han eliminado todos los conjuntos");
                break;
        }
        fflush(archivoSalida);
    } while(opcion != 7);
    fclose(archivoSalida);


    return 0;
}