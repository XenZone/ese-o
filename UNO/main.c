#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

DIR* directorio;


void 
        

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
    int color, tipo, cantidad;                                                                                                 // el 0 sale solo 1 vez

    for (color = 0; color < 4; color++){
        for (cantidad = 0; cantidad < 2; cantidad++){
            for (tipo = 0; tipo < 12; tipo++){
                numero = cantidad + 1 + '0';
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
        
        numero = color + 1 + '0';
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

int main(){

    char del;
    if(opendir("Juego")){    // Asi se revisa el directorio completo del juego, sin necesidad de revisar cada una de las carpetas
        puts("Existen datos previos. Desea eliminarlos? (S|N):");
        scanf("%c", &del);
        if (del == 'S' || del == 'Y'){
            system("rm -r Juego");  // Se elimina la carpeta padre, y se muere todo lo que est? adentro. izi pizi.
        }
    }
    
    setup();
    puts("Se crearon los datos del juego"); /// Se borra eventualmente.
    // Se crean todas las carpetas con las cartas.
    
    puts("Eliminar datos? (Y|N)"); // Se muere todo :(
    scanf("%c", &del);
    if (del == 'S' || del == 'Y'){
        system("rm -r Juego");
    }

    return 0;
}