#ifndef COMMON_H
#define COMMON_H

#include <cstring>

struct ComandaHistorica {
    char fecha[11];        // "Dia-Mes-Año"
    char nombreMozo[50];
    int  codigoProducto;
    int  cantidad;
    float comision;
};

struct Producto {
    int  codigo;
    char descripcion[50];
    float precio;
    int  stockActual;
};


struct Mozo {
    int  idMozo;
    char nombre[50];
    char password[20];   
    float totalComision;
};

struct Comanda {
    int  idMozo;
    int  codigoProducto;
    int  cantidad;
    float comision;
};

const float TASA_COMISION = 0.10f;  
const int   K_CIFRADO     = 5;      

inline void cifrarPassword(char* destino, const char* origen, int n, int k) {
    int len = (int)strlen(origen);
    if (len >= n) len = n - 1;
    int i;
    for (i = 0; i < len; i++) {
        destino[i] = (char)(origen[i] + k);
    }
    for (; i < n; i++) destino[i] = '\0';
}

#endif
