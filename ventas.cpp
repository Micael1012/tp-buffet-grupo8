#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const int K = 3;

struct Mozo {
    int id;
    char nombre[50];
    char clave[20];
    float comisionAcumulada;
};

struct Producto {
    int codigo;
    char descripcion[48];
    float precio;
    int stock;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

void desencriptar(char texto[]) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] -= K;
    }
}

bool validarMozo(int idBuscado, char claveIngresada[]) {
    ifstream archivo("mozos.dat", ios::binary);

    if (!archivo) {
        cout << "\nERROR: No se pudo abrir mozos.dat\n";
        return false;
    }

    Mozo mozo;

    while (archivo.read(reinterpret_cast<char*>(&mozo), sizeof(Mozo))) {
        if (mozo.id == idBuscado) {
            char claveReal[20];

            strcpy(claveReal, mozo.clave);
            desencriptar(claveReal);

            archivo.close();

            if (strcmp(claveReal, claveIngresada) == 0) {
                return true;
            }

            cout << "\nClave incorrecta.\n";
            return false;
        }
    }

    archivo.close();

    cout << "\nNo existe un mozo con ese ID.\n";
    return false;
}

bool venderProducto(int codigoBuscado, int cantidad, float &precioProducto) {
    fstream archivo(
        "inventario.dat",
        ios::in | ios::out | ios::binary
    );

    if (!archivo) {
        cout << "\nERROR: No se pudo abrir inventario.dat\n";
        return false;
    }

    Producto producto;

    while (archivo.read(
        reinterpret_cast<char*>(&producto),
        sizeof(Producto)
    )) {
        if (producto.codigo == codigoBuscado) {

            if (producto.stock < cantidad) {
                cout << "\nStock insuficiente.\n";
                cout << "Stock disponible: "
                     << producto.stock << endl;

                archivo.close();
                return false;
            }

            precioProducto = producto.precio;

            producto.stock -= cantidad;

            archivo.seekp(
                -static_cast<streamoff>(sizeof(Producto)),
                ios::cur
            );

            archivo.write(
                reinterpret_cast<char*>(&producto),
                sizeof(Producto)
            );

            archivo.close();

            return true;
        }
    }

    archivo.close();

    cout << "\nProducto no encontrado.\n";
    return false;
}

void guardarComanda(
    const char nombreArchivo[],
    Comanda nuevaComanda
) {
    ofstream archivo(
        nombreArchivo,
        ios::binary | ios::app
    );

    if (!archivo) {
        cout << "\nERROR al abrir el archivo de comandas.\n";
        return;
    }

    archivo.write(
        reinterpret_cast<char*>(&nuevaComanda),
        sizeof(Comanda)
    );

    archivo.close();
}

void ordenarComandas(const char nombreArchivo[]) {
    fstream archivo(
        nombreArchivo,
        ios::in | ios::out | ios::binary
    );

    if (!archivo) {
        return;
    }

    archivo.seekg(0, ios::end);

    int cantidadRegistros =
        static_cast<int>(archivo.tellg() / sizeof(Comanda));

    for (int i = 0; i < cantidadRegistros - 1; i++) {
        for (int j = 0; j < cantidadRegistros - i - 1; j++) {

            Comanda c1;
            Comanda c2;

            streampos posicion1 =
                static_cast<streampos>(j * sizeof(Comanda));

            streampos posicion2 =
                static_cast<streampos>((j + 1) * sizeof(Comanda));

            archivo.clear();

            archivo.seekg(posicion1);
            archivo.read(
                reinterpret_cast<char*>(&c1),
                sizeof(Comanda)
            );

            archivo.clear();

            archivo.seekg(posicion2);
            archivo.read(
                reinterpret_cast<char*>(&c2),
                sizeof(Comanda)
            );

            if (c1.idMozo > c2.idMozo) {

                archivo.clear();

                archivo.seekp(posicion1);
                archivo.write(
                    reinterpret_cast<char*>(&c2),
                    sizeof(Comanda)
                );

                archivo.clear();

                archivo.seekp(posicion2);
                archivo.write(
                    reinterpret_cast<char*>(&c1),
                    sizeof(Comanda)
                );
            }
        }
    }

    archivo.close();
}

int main() {
    char fecha[20];

    cout << "=====================================\n";
    cout << "     BUFFET ALBERT EINSTEIN\n";
    cout << "        CARGA DE VENTAS\n";
    cout << "=====================================\n\n";

    cout << "Ingrese la fecha (dd-mm-aaaa): ";
    cin.getline(fecha, 20);

    char nombreArchivo[50];

    strcpy(nombreArchivo, "comandas_");
    strcat(nombreArchivo, fecha);
    strcat(nombreArchivo, ".dat");

    char continuar = 'S';

    while (continuar == 'S' || continuar == 's') {

        int idMozo;
        char clave[20];

        cout << "\n----- NUEVA VENTA -----\n";

        bool mozoValido = false;

        while (!mozoValido) {

            cout << "Ingrese ID del mozo: ";
            cin >> idMozo;

            cout << "Ingrese clave: ";
            cin >> clave;

            mozoValido =
                validarMozo(idMozo, clave);

            if (!mozoValido) {
                char intentar;

                cout << "¿Intentar nuevamente? (S/N): ";
                cin >> intentar;

                if (intentar != 'S' && intentar != 's') {
                    break;
                }
            }
        }

        if (!mozoValido) {

            cout << "\nVenta cancelada.\n";

            cout << "\n¿Desea cargar otra venta? (S/N): ";
            cin >> continuar;

            continue;
        }

        int codigoProducto;
        int cantidad;
        float precio;

        cout << "\nIngrese codigo del producto: ";
        cin >> codigoProducto;

        cout << "Ingrese cantidad: ";
        cin >> cantidad;

        if (cantidad <= 0) {

            cout << "\nLa cantidad debe ser mayor a cero.\n";

            cout << "\n¿Desea cargar otra venta? (S/N): ";
            cin >> continuar;

            continue;
        }

        if (venderProducto(
            codigoProducto,
            cantidad,
            precio
        )) {

            float comision =
                precio * cantidad * 0.10f;

            Comanda nuevaComanda;

            nuevaComanda.idMozo = idMozo;
            nuevaComanda.codigoProducto = codigoProducto;
            nuevaComanda.cantidad = cantidad;
            nuevaComanda.comision = comision;

            guardarComanda(
                nombreArchivo,
                nuevaComanda
            );

            cout << "\nVENTA REGISTRADA CORRECTAMENTE\n";

            cout << "Precio unitario: $"
                 << precio << endl;

            cout << "Cantidad: "
                 << cantidad << endl;

            cout << "Comision: $"
                 << comision << endl;
        }

        cout << "\n¿Desea cargar otra venta? (S/N): ";
        cin >> continuar;
    }

    ordenarComandas(nombreArchivo);

    cout << "\nComandas ordenadas por mozo.\n";
    cout << "Fin del programa.\n";

    return 0;
}
