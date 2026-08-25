#ifndef COMMON_H
#define COMMON_H

#include <cstring>

// ---------- Structs de la catedra (NO se tocan, hay que respetarlas tal cual) ----------

struct ComandaHistorica {
    char fecha[11];        // "DD-MM-AAAA"
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

// ---------- Structs nuevas (las armamos nosotros) ----------

struct Mozo {
    int  idMozo;
    char nombre[50];
    char password[20];   // clave CIFRADA, nunca en texto plano
    float totalComision;
};

struct Comanda {
    int  idMozo;
    int  codigoProducto;
    int  cantidad;
    float comision;
};

const float TASA_COMISION = 0.10f;   // 10% del total vendido
const int   K_CIFRADO     = 5;       // corrimiento elegido por el grupo (cambienlo si quieren)

// Cifra (o descifra, pasando k negativo) una clave sumando k a cada caracter.
// destino: buffer de salida de tamano n, se completa con '\0' hasta el final.
// origen : clave en texto plano (o cifrada, si se usa para descifrar y mostrar en la defensa).
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