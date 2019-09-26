#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>


typedef struct mazo{
    char cartas[108][50];
    int n_cartas;
} mazo;

DIR *directorio;

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
                sprintf(buffer, "Juego/Mazo/%s %s %c.txt", tipoCarta[tipo], colorCarta[color], numero);
                carta = fopen(buffer, "w");
                fclose(carta);
                strcpy(buffer, "");
            }
        }
        strcpy(buffer, "");
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


mazo* leerMazo(){

    mazo *Mazo = malloc(sizeof(mazo));
    struct dirent *dir;

    int count = 0;
    int index = 0;
    directorio = opendir("Juego/Mazo/.");

    if (directorio){
        while ((dir = readdir(directorio)) != NULL){
            if ((strcmp(dir->d_name, ".") != 0 && (strcmp(dir->d_name, "..") != 0))){
                strcpy(Mazo->cartas[count],dir->d_name);
                count++;
            }
        }
        Mazo->n_cartas = count;
        closedir(directorio);
    }
    return Mazo;
}


void randPull(mazo* M, int jugador){

    int numC = M->n_cartas;

    time_t t;
    srand((unsigned) time(&t));

    int r = rand() % numC;

    char comm[100] = "mv ./Juego/Mazo/";
    char strJ[1];
    *strJ = jugador + '0';


    strcat(comm, M->cartas[r]);

    strcpy(M->cartas[r],M->cartas[numC]);

    M->n_cartas--;

    strcat(comm, " ./Juego/Jugador_");
    strcat(comm, strJ);
    system(comm);
}


int buscarPrevio(int situacion){
    char opcion;
    int verificacion = 1;

    if (situacion == 1){    //??
        while (verificacion == 1){
            printf("Existe una partida anterior guardada. Desea continuarla? (S|N): ");
            scanf(" %c", &opcion);

            if (opcion == 'S'){
                printf("Se retomaran los datos anteriores.\n");
                return 1;
            }
            else if (opcion == 'N'){
                printf("La partida ha sido eliminada.\n");
                system("rm -r Juego");
                return 0;
            }
            else{
                printf("Entrada invalida\n");
            }
        }
    }

    else{
        while (verificacion == 1){
            directorio = opendir("Juego");
            if(directorio){    // Asi se revisa el directorio completo del juego, sin necesidad de revisar cada una de las carpetas
                printf("La ultima partida no termino debidamente, y los datos de la partida siguen ahi. Desea eliminarlos? (S|N):\n");
                scanf(" %c", &opcion);
                if (opcion == 'S'){
                    system("rm -r Juego");  // Se elimina la carpeta padre, y se muere todo lo que est? adentro. izi pizi.
                    verificacion = 0;
                    closedir(directorio);
                    return 0;
                }
                else if (opcion == 'N'){
                    verificacion = 0;
                    printf("Se retomara la partida guardada anteriormente\n");
                    closedir(directorio);
                    return 0;
                }
                else printf("Entrada invalida.\n");
            }
            else{
                closedir(directorio);
                return 0;
            }
        }
    }
    return 0;
}


void terminarPartida(){
    char opcion;
    int verificacion = 1;

    while (verificacion == 1){
        printf("Desea guardar su partida? (S|N): ");
        scanf(" %c", &opcion);
        if (opcion == 'S'){
            printf("Se guardaran los datos de la partida\n");
            system("mkdir -p Juego/gameIsSaved");
            FILE* Dummy;
            Dummy = fopen("Juego/gameIsSaved/dummy.txt", "w");
            fprintf(Dummy, "Hola, si me ves aqui es porque grabaste tu partida, y me aseguro que la carpeta exista.\nLogre mi mision! Yupy!");
            verificacion = 0;
            fclose(Dummy);
        }
        else if (opcion == 'N'){
            verificacion = 0;
            printf("Se eliminaron los datos de la partida.\n");
            system("rm -r Juego");
            return;
        }
        else printf("Entrada invalida.\n");
    }
    return; // Nunca deber?a llegar a este punto.
}

int main(){

    int restaurar = 0;
    int jugador, carta;
    directorio = opendir("Juego/gameIsSaved");
    if (directorio){
        restaurar = buscarPrevio(1);
    }
    else restaurar = buscarPrevio(2);
    closedir(directorio);

    if (restaurar != 1){
        setup();
    }

    mazo *Mazo = leerMazo();

    for(jugador = 1; jugador <= 4; jugador++){
        for(carta = 0; carta < 7; carta++){
            randPull(Mazo, jugador);
        }
    }

    terminarPartida();
    free(Mazo);
    return 0;
}
