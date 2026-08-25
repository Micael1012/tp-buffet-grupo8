#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int MAX_COMANDAS = 1000; // ajustalo si hace falta

int main() {
    Comanda todasLasComandas[MAX_COMANDAS];
    int totalComandas = 0;

    int cantidadDias;
    cout << "¿Cuantos dias tiene la semana a cerrar? ";
    cin >> cantidadDias;

    for (int i = 0; i < cantidadDias; i++) {
        char fecha[11];
        cout << "Fecha del dia " << (i + 1) << " (DD-MM-AAAA): ";
        cin >> fecha;

        char archivoDelDia[50];
        
        sprintf(archivoDelDia,"comandas_%s.dat",fecha); 
        FILE* f = fopen(archivoDelDia,"rb");
        
        if(f == NULL){
		cout<<"Se omite debido a que no se encontro nada del dia: "<<fecha <<endl;
		}else{
			Comanda c;
		while(fread(&c,sizeof(Comanda),1,f) == 1){
			cout<<c.idMozo<<" - "<<c.codigoProducto<<" - "<<c.cantidad<<c.comision<<endl;//Esto es para cuando pruebe ver que funciona, no forma parte
			todasLasComandas[totalComandas] = c;
			totalComandas++;
		}
		fclose(f);
    }
}

    for (int i = 0; i < totalComandas - 1; i++) {
    for (int j = 0; j < totalComandas - 1 - i; j++) {
        if (todasLasComandas[j].idMozo > todasLasComandas[j+1].idMozo) {
        	Comanda aux;
            aux=todasLasComandas[j];
            todasLasComandas[j]=todasLasComandas[j+1];
            todasLasComandas[j+1]=aux;
        }
    }
}

    // ACÁ: armar el nombre del archivo semanal y escribirlo

    cout << "Listo." << endl;
    return 0;
}