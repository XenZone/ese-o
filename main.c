#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>

DIR* directorio;

void setup(){

    // recover() ->  Recuperar alguna partida anterior.
    // execute() -> Correr el juego de la forma que se haya elegido.
    // exit() ->  Salir del jeugo y preguntar save o no.

    // La idea de usar mkdir -p es para crear subdirectorios.
    // Si uno de los directorios en el camino no existe, los crea.

    system("mkdir -p  Juego/Mazo");
    system("mkdir -p Juego/Last");

    system("mkdir -p Juego/Jugadores/Jugador_1");
    system("mkdir -p Juego/Jugadores/Jugador_2");
    system("mkdir -p Juego/Jugadores/Jugador_3");
    system("mkdir -p Juego/Jugadores/Jugador_4");


    FILE* carta;
    char *colorCarta[4] = {"Rojo", "Azul", "Verde", "Amarillo"};
    char buffer[35], numero, *tipoCarta[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "+2", "Salto", "Reversa"}; // 2 de cada color
    int color, tipo, cantidad;                                                                                        // el 0 sale solo 1 vez

    for (color = 1; color < 5; color++){
        for (cantidad = 1; cantidad < 3; cantidad++){
            for (tipo = 0; tipo < 12; tipo++){
                numero = cantidad + '0';
                sprintf(buffer, "Juego/Mazo/%s %s (%c).txt", tipoCarta[tipo], colorCarta[color], numero);
                carta = fopen(buffer, "w");
                fclose(carta);
                strcpy(buffer, "");
            }
        }
        sprintf(buffer, "Juego/Mazo/0 %s.txt", colorCarta[color]);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");

        numero = color + '0';
        sprintf(buffer, "Juego/Mazo/+4 Negro %c", numero);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");

        sprintf(buffer, "Juego/Mazo/Comodin Negro %c", numero);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");
        }
    }

void repartir(){
    //se reparten 7 cartas a cada jugador y se mueve una carta a Last
    time_t t;
    srand((unsigned) time(&t));


    struct dirent* dEntrada;
    directorio = opendir("./Juego/Mazo");

    while ((dEntrada = readdir(directorio))!= NULL){
        printf("%s\n", dEntrada->d_name);

    //dEntrada = readdir(directorio);
    int carta, jugador;
    for(jugador = 0; jugador < 4; jugador++){
        for(carta = 0; carta < 7; carta++){

            int r = rand()%108;
            }
        }
    }

}

int main(){
/*
    char del;
    if(opendir("Juego")){    // Asi se revisa el directorio completo del juego, sin necesidad de revisar cada una de las carpetas
        puts("Existen datos previos. Desea eliminarlos? (S|N):");
        scanf("%c", &del);
        if (del == 'S' || del == 'Y'){
            system("rm -r Juego");  // Se elimina la carpeta padre, y se muere todo lo que est? adentro. izi pizi.
        }
    }
*/
    setup();

    repartir();
/*

    puts("Se crearon los datos del juego"); /// Se borra eventualmente.
    // Se crean todas las carpetas con las cartas.

    puts("Eliminar datos? (Y|N)"); // Se muere todo :(
    scanf("%c", &del);
    if (del == 'S' || del == 'Y'){
        system("rm -r Juego");
    }
*/
    return 0;
}
