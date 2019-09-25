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
                sprintf(buffer, "Juego/Mazo/%s %s %c.txt", tipoCarta[tipo], colorCarta[color], numero);
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

    char** cardL;
    cardL = (char**)malloc(108 * sizeof(char*));

    int it = 0;
    //while ((dEntrada = readdir(directorio))!= NULL){
    while (it < 108){
        cardL[it] = (char*)malloc(50 * sizeof(char));
        cardL[it] = dEntrada->d_name;
        it++;

    }
    for(it = 0; it < 108; it++){
        printf("%s\n", cardL[it]);
    }


    int carta, jugador;
    for(jugador = 1; jugador < 5; jugador++){
        for(carta = 0; carta < 7; carta++){

            int r = rand()%108;

            char comm[100] = "mv ./Juego/Mazo/";
            char strJ[1];
            *strJ = jugador + '0';

            strcat(comm,cardL[r]);
            strcat(comm,"./Juego/Jugador_");
            strcat(comm,strJ);

            system(comm);
        }
    }

    int i;
    for(i = 0; i < 108; i++){
        free(cardL[i]);
    }
    free(cardL);
}


int buscarPrevio(int situacion){
 
    char opcion;
    int verificacion = 1;

    if (situacion == 1){
        while (verificacion == 1){
            printf("Existe una partida anterior guardada. Desea continuarla? (S|N)");
            scanf(" %c", &opcion);

            if (opcion == 'S'){
                printf("Se retomaran los datos anteriores.");
                return 1;
            }
            else if (opcion == 'N'){
                printf("La partida ha sido eliminada.");
                system("rm -r Juego");
                return 0;
            }
            else{
                printf("Entrada invalida");
            }
        }
    }
    
    else{
        while (verificacion == 1){
            if(opendir("Juego")){    // Asi se revisa el directorio completo del juego, sin necesidad de revisar cada una de las carpetas
                printf("La ultima partida no termino debidamente, y los datos de la partida siguen ahi. Desea eliminarlos? (S|N):\n");
                scanf(" %c", &opcion);
                if (opcion == 'S'){
                    system("rm -r Juego");  // Se elimina la carpeta padre, y se muere todo lo que est? adentro. izi pizi.
                    verificacion = 0;
                    return 1;
                }
                else if (opcion == 'N'){
                    verificacion = 0;
                    printf("Se retomar? la partida guardada anteriormente");
                    return 0;
                }
                else printf("Entrada inv?lida.");
            }
            else return 0;
        }
    }
    return 0; // Nunca deber?a llegar a este punto.
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
        }
        else if (opcion == 'N'){
            verificacion = 0;
            printf("Se eliminaron los datos de la partida.\n");
            system("rm -r Juego");
            return;
        }
        else printf("Entrada inv?lida.\n");
    }
    return; // Nunca deber?a llegar a este punto.
}

int main(){
    
    int restaurar = 0;
    if (opendir("Juego/gameIsSaved")){ 
        restaurar = buscarPrevio(1);
    }
    else restaurar = buscarPrevio(2);
    
    if (restaurar != 1){
        setup();
        puts("3,..............................................\n");
        repartir();
    }
    
    terminarPartida();
    
    
    return 0;
}
