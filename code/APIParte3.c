#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "apig23.h"
#include "APIParte3.h"

// Returns min between 2 elements
u32 min(u32 a, u32 b) {
    if (a<=b){
        return a;
    } else {
        return b;
    } 
}

// Returns max between 2 elements
u32 max(u32 a, u32 b) {
    if (a<=b){
        return b;
    } else {
        return a;
    } 
}
#define COLOR_USADO 1
#define COLOR_NO_USADO 0


u32 colorear(u32 vertice_a_colorear, Grafo G, u32* Color) {


    u32 color_a_pintar = MAX_U32;

    // Si no tiene vecino lo coloreamos con el color 0;
    if (Grado(vertice_a_colorear, G) == 0) {
        return 0;
    } 

    // Lista de colores usados, calloc inicializa todos los lugares en 0, es decir en COLOR_NO_USADO.
    u32* colores_usados = calloc(NumeroDeVertices(G), sizeof(u32)); 
    if (colores_usados==NULL) { return MAX_U32};
    
    // Recorremos los vecinos
    for (u32 i = 0; i < Grado(vertice_a_colorear, G); i++) {
        u32 color_vecino = Color[IndiceVecino(i, vertice_a_colorear, G)];

        // Vemos solo los vecinos coloreados
        if (color_vecino != MAX_U32) {
            colores_usados[color_vecino] = COLOR_USADO;
        }
        // Encontramos el minimo color no usado
        for (u32 i = 0; i < NumeroDeVertices(G); i++) {
            if (colores_usados[i]==COLOR_NO_USADO) {
                color_a_pintar = i;
                break;
            }
        }
    }   
    free(colores_usados);
    colores_usados = NULL;
    return color_a_pintar;

}

u32 GreedyDinamico(Grafo G, u32* Orden, u32* Color, u32 p) {
    /*
    RESUMEN:
        Retorna la cantidad de colores usados si no hay error
        Retorna (2^32)-1 en caso de erro
        
        Color -> array [0..n-1]   
        Orden -> array [0..n-1] 

        Colorea en orden ASCENDENTE hasta p (sin incluir), luego colorea con otro orden

        Lo que vamos a hacer es partir los arrays en 2 partes, tal que 
        array = array1 U array2: array1=array{0..(p-1)}, array2=array{p,n}.

        Luego vamos a ordenar array2 con el criterio dado y colorear con ese orden de vertices.
    */

    // Pisamos color para poder ver si coloreamos bien
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {
        Color[i] = MAX_U32;
    }

    // La variable que vamos a retornar
    u32 max_color = 0;

    // Recorremos los vertices
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {

        if (i < p) {
            u32 vertice_a_colorear = Orden[i];
        
            // obtenemos el menor color tal que es distinto de sus vecinos
            u32 nuevo_color = colorear(vertice_a_colorear, G, Color);

            // Nos fijamos si hubo error
            if (nuevo_color == MAX_U32) {return (2^32)-1;}

            // Escribimos el array con el nuevo color
            Color[Orden[i]] = nuevo_color;

            // actualizamos max_color
            max_color = max(max_color, nuevo_color);

        } else {

        }
        
    
    
    }
    













    return max_color+1;
}









































struct FO_St {
    u32* vertices;      
    u32 vertex_count;      
    u32 E_value;
    u32 color;
};

int compareFO (void* a, void* b) {
    struct FO_St v1 = *(struct FO_St*) a; 
    struct FO_St v2 = *(struct FO_St*) b; 
    if (v1.E_value > v2.E_value) {
        return -1;
    } 
    else if (v1.E_value == v2.E_value) {
        if (v1.color >= v2.color) {
            return -1;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

char FirstOrder(Grafo G, u32* Orden, u32* Color) {
    /* 
    RESUMEN:

        '0' -> no error
        '1' -> error
        Color -> array [0..n-1]   
        Orden -> array [0..n-1]

        m(x) = min(Grado(i,G): Color[i]=x)
        M(x) = max(Grado(i,G): Color[i]=x)
        E(x) = {
            0x7fffffff + m(x)   si (x+2)%3 = 0
            0x1fff + M(x)       si (x+1)%3 = 0
            M(x) + m(x)         si (x+0)%3 = 0
        } 

        Se ponen primero los indices i tal que Color[i] sea igual al color x tal que E(x) es el maximo de E,
        luego los indices i tal que Color[i] es el color x tal que E(x) es el segundo mayor valor de E luego del maximo, etc.
    */ 

    // Conseguimos la cantidad de colores
    // Complejidad O(n)
    u32 color_count = 0;
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {                     
        color_count = max(color_count, Color[i]);
    }
    if (Delta(G)!=0 && color_count==0) {
        return '1';
    }
    // La cantidad de colores es max_color+1, No hay que olvidarse el color 0.
    color_count++;

    
    // Inicializamos las variables que vamos a usar
    // Complejidad O(r)
    // Listas que nos ayudan a calcular E(x)
    u32* m_list = calloc(color_count, sizeof(u32));
    u32* M_list = calloc(color_count, sizeof(u32));
    // Lista de estructura temporal
    struct FO_St* FO_list = calloc(color_count, sizeof(struct FO_St));
    // Memoria disponible
    if (m_list==NULL || M_list==NULL || FO_list==NULL) {return '1';}

    for (u32 i = 0; i < color_count; i++) {
        m_list[i] = MAX_U32;
        // M_list[i] = 0;   esto se hace en el calloc.
        FO_list[i].color = i;
        FO_list[i].E_value = 0;
        FO_list[i].vertex_count = 0;
        FO_list[i].vertices = calloc(NumeroDeVertices(G),sizeof(u32));
        // Memoria disponible
        if (FO_list[i].vertices==NULL) { return '1';}
    }
    
    // Computamos m(x) y M(x) para todos los colores
    // Complejidad O(n)
    for (u32 i = 0; i < NumeroDeVertices; i++) {
        m_list[Color[i]] = min(m_list[Color[i]], Grado(i,G));
        M_list[Color[i]] = max(M_list[Color[i]], Grado(i,G));
    }

    // Calculo de E(x) para todos los colores
    // Complejidad O(n) 
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {
        struct FO_St tmp = FO_list[Color[i]];
        if ((Color[i]+2)%3 == 0) {
            tmp.vertices[tmp.vertex_count] = i;
            tmp.vertex_count++;
            tmp.E_value = 0x7fffffff + m(Color[i]);
        } else if ((Color[i]+1)%3 == 0) {
            tmp.vertices[tmp.vertex_count] = i;
            tmp.vertex_count++;
            tmp.E_value = 0x1fff + m(Color[i]);
        } else if ((Color[i]+0)%3 == 0) {
            tmp.vertices[tmp.vertex_count] = i;
            tmp.vertex_count++;
            tmp.E_value = M(Color[i]) + m(Color[i]);
        }
    }
    
    // Ordenamos este array de estructuras.
    // Complejidad O(r log r)
    qsort(FO_list, color_count, sizeof(struct FO_St), compareFO);
    
    // Ahora escribimos lo que calculamos en el array Orden
    // Aprobechamos para liberar memoria
    // Complejidad O(n)
    u32 order_i = 0;
    for (u32 color_i = 0; color_i < color_count; color_i++) {
        for (u32 vertex_i = 0; vertex_i < FO_list[color_i].vertex_count; vertex_i++) {
            Orden[order_i] = FO_list[color_i].vertices[vertex_i];
            order_i++;
        }
        free(FO_list[color_i].vertices);
        FO_list[color_i].vertices = NULL;
    }
    
    // Liberamos memoria restante
    free(FO_list);
    FO_list = NULL;
    free(M_list);
    M_list = NULL;
    free(m_list);
    m_list = NULL;
    
    /* 
    Complejidad total:
        O(n)+O(r)+O(n)+O(n)+O(r log r)+O(n) =
        O(n + r + r log r)                  =   (r <= n)
        O(n + n log n)                      =    
        O(n log n)    
    */

    return '0';
}



struct SO_St {
    u32* vertices;     
    u32 vertex_count;       
    u32 S_value;
    u32 color;
};

int compareSO (void* a, void* b) {
    // The comparison function must return an integer less than, equal to, or greater than
    // zero if the first argument is considered to be respectively less  than,  equal  to,  
    // or greater than the second.  If two members compare as equal, their order in the 
    // sorted array is undefined.

    struct SO_St v1 = *(struct SO_St*) a; 
    struct SO_St v2 = *(struct SO_St*) b; 
    if (v1.S_value > v2.S_value) {
        return -1;
    } 
    else if (v1.S_value == v2.S_value) {
        // Desempatamos por color
        if (v1.color >= v2.color) {
            return -1;
        } else {
            return 1;
        }

    } else if (v1.S_value < v2.S_value) {
        return 1;
    }
}

char SecondOrder(Grafo G,u32* Orden,u32* Color) {
    /* RESUMEN:
        '0' -> no error
        '1' -> error
        Color -> array [0..n-1]   
        Orden -> array [0..n-1]

        Se define la funcion S:{0,1,...,(r-1)} -> Z  tal que:
            S(x) = sum[ i:(Color[i]=x) && (Grado(i,G)>1) ] (Grado(i,G))

        Se ponen primero los indices i tal que Grado(i,G)>1 y Color[i] sea igual 
        al color x tal que S(x) es el maximo de S; luego los indices i tal que Grado(i,G)>1 
        y Color[i] es el color x tal que S(x) es el segundo mayor valor de S luego del maximo, 
        etc, y al final de todo se ponen todos los indices i con Grado(i,G)=1.
    */ 

    // Conseguimos la cantidad de colores
    // Complejidad O(n)
    u32 color_count = 0;
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {                     
        color_count = max(color_count,Color[i]);
    }
    if (Delta(G)!=0 && color_count==0) {
        return '1';
    }
    // La cantidad de colores es color_count+1, No hay que olvidarse el color 0.
    color_count++;

    // Vamos a crear e inicializar una lista de estructuras SO_St
    // Complejidad O(r) <= O(n)
    struct SO_St* SO_list = calloc(color_count,sizeof(struct SO_St));
    // Memoria disponible
    if (SO_list==NULL) { return '1';}
    for (u32 i = 0; i < color_count; i++) {
        SO_list[i].vertices = calloc(NumeroDeVertices(G),sizeof(u32));
        // Memoria disponible
        if (SO_list[i].vertices==NULL) { return '1';}
        SO_list[i].vertex_count = 0;
        SO_list[i].S_value = 0;
        SO_list[i].color = i;
    }
    
    // Calculo de S(x)
    // Complejidad O(n)
    for (u32 i = 0; i < NumeroDeVertices(G); i++) {
        if (Grado(i,G)>1) {
            SO_list[Color[i]].vertices[SO_list[Color[i]].vertex_count] = i;
            SO_list[Color[i]].vertex_count++; 
            SO_list[Color[i]].S_value += Grado(i,G);
        } else {
            SO_list[color_count-1].vertices[SO_list[color_count-1].vertex_count] = i;
            SO_list[color_count-1].vertex_count++;
        }
        
    }
    
    // Vamos a ordenar por el criterio dado, los vertices con Grado(i,j)<=1 son puestos al 
    // final ya que 0 = "el menor valor de S(x)", estos vertices tambien son ordenados en 
    // bloques de colores.
    // Complejidad O(r log r) <= O(n log n)
    qsort(SO_list, color_count, sizeof(struct SO_St), compareSO);

    // Ahora vamos a escribir en Orden lo que ordenamos, agregando al ultimo los 
    // vertices tal que Grado(i,G) <= 1 
    // Aprobechamos para liberar memoria
    // Complejidad O(n)
    u32 order_i = 0;
    for (u32 color_i = 0; color_i < color_count; color_i++) {
        for (u32 vertex_i = 0; vertex_i < SO_list[color_i].vertex_count; vertex_i++) {
            Orden[order_i] = SO_list[color_i].vertices[vertex_i];
            order_i++;
        }
        free(SO_list[color_i].color);
        SO_list[color_i].color = NULL;
    }
    
    // Liberamos la memoria restante
    free(SO_list);
    SO_list = NULL;

    /* 
    Complejidad total:
        O(n)+O(r)+O(n)+O(r log r)+O(n)  =
        O(n + r + r log r)              =   (r <= n)
        O(n + n log n)                  = 
        O(n log n)    
    */


    return '0';
}