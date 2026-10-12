#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>


using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int buscarNombreMozo(FILE *archivoMozos, int idBuscado, char nombreEncontrado[]) {
    struct Mozo mozo;

    rewind(archivoMozos);

    while (fread(&mozo, sizeof(struct Mozo), 1, archivoMozos) == 1) {
        if (mozo.idMozo == idBuscado) {
            strcpy(nombreEncontrado, mozo.nombre);
            return 1;
        }
    }

    return 0;
}

int main() {

    FILE *archivoComandas;
    FILE *archivoMozos;

    char numeroSemana[10];
    char mes[3];
    char nombreArchivo[50];

    struct Comanda c;
    int leido;

    int totalBuffetCantidad = 0;
    float totalBuffetComision = 0.0f;

    cout << "========================================\n";
    cout << "      RESUMEN DE CIERRE SEMANAL\n";
    cout << "========================================\n\n";

    cout << "Ingrese el numero de semana (ej: 1): S";
    cin >> setw(10) >> numeroSemana;

    cout << "Ingrese el mes (dos digitos, ej: 06): ";
    cin >> setw(3) >> mes;

    strcpy(nombreArchivo, "comandas_semana_s");
    strcat(nombreArchivo, numeroSemana);
    strcat(nombreArchivo, "-");
    strcat(nombreArchivo, mes);
    strcat(nombreArchivo, ".dat");

    archivoComandas = fopen(nombreArchivo, "rb");

    if (archivoComandas == NULL) {
        cout << "ERROR: no se pudo abrir " << nombreArchivo << "\n";
        return 1;
    }

    archivoMozos = fopen("mozos.dat", "rb");

    if (archivoMozos == NULL) {
        cout << "ERROR: no se pudo abrir mozos.dat\n";
        fclose(archivoComandas);
        return 1;
    }

    cout << "\nPlanilla: " << nombreArchivo << "\n";
    cout << "----------------------------------------\n";


    leido = fread(&c, sizeof(struct Comanda), 1, archivoComandas); // lectura anticipada

    if (leido == 0) {
        cout << "No hay ventas registradas en esta semana.\n";
        fclose(archivoComandas);
        fclose(archivoMozos);
        return 0;
    }

    while (leido == 1) {
        int idGrupo = c.idMozo;
        int cantidadGrupo = 0;
        float comisionGrupo = 0.0f;

        char nombreMozo[50];
        if (!buscarNombreMozo(archivoMozos, idGrupo, nombreMozo)) {
            strcpy(nombreMozo, "(mozo desconocido)");
        }

        while (leido == 1 && c.idMozo == idGrupo) {
            cantidadGrupo += c.cantidad;
            comisionGrupo += c.comision;
            leido = fread(&c, sizeof(struct Comanda), 1, archivoComandas);
        }

        cout << "\nMozo #" << idGrupo << " - " << nombreMozo << "\n";
        cout << "  Productos vendidos: " << cantidadGrupo << "\n";
        cout << "  Comision a pagar:   $" << comisionGrupo << "\n";

        totalBuffetCantidad += cantidadGrupo;
        totalBuffetComision += comisionGrupo;

        if (leido == 1) {
            cout << "----------------------------------------\n";
        }
    }

    fclose(archivoComandas);
    fclose(archivoMozos);

    cout << "\n========================================\n";
    cout << "Total de productos vendidos por el buffet: " << totalBuffetCantidad << "\n";
    cout << "Total de comision generada:                $" << totalBuffetComision << "\n";
    cout << "========================================\n";

    return 0;
}

