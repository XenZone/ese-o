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


// PIPES

int pipes[4][2], procesos[4] = {0,0,0,0};
char buffer[50];

// Struct para el guardado de las cartas. Tiene espacio como para las 108 cartas del juego.


DIR *directorio;



/*
 Funcion Setup:  No recibe ni retorna nada.
 Genera el juego inicial, creando los directorios necesarios, junto con todas las cartas del mazo.
 Se utiliza un directorio general para todo el juego, que es la carpeta "Juego"
 */
void setup(){

    system("mkdir -p  Juego/Mazo");  // Directorio principal de las cartas
    system("mkdir -p Juego/Last"); // Directorio con la ?ltima carta jugada

    system("mkdir -p Juego/Jugadores/Jugador_1");  // Directorios con las manos de cada Jugador
    system("mkdir -p Juego/Jugadores/Jugador_2");
    system("mkdir -p Juego/Jugadores/Jugador_3");
    system("mkdir -p Juego/Jugadores/Jugador_4");


    FILE* carta;
    char *colorCarta[4] = {"Rojo", "Azul", "Verde", "Amarillo"};
    char buffer[35], numero, *tipoCarta[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "+2", "Salto", "Reversa"}; // 2 de cada color
    int color, tipo, cantidad;                                                                                        // el 0 sale solo 1 vez

    // En este ciclo se generan todas las cartas del mazo 
    for (color = 0; color < 4; color++){
        for (cantidad = 0; cantidad < 2; cantidad++){ // Ciclo para generar todas las cartas duplicadas.
            for (tipo = 0; tipo < 12; tipo++){
                // La variable numero se utiliza para generar archivos iguales, ya que 2 archivos no pueden tener el mismo nombre
                // por lo tanto, se generan archivos del tipo ["NumCarta ColorCarta Repeticion]
                // como lo ser?a el caso de "1 Rojo 1" y "1 Rojo 2".
                numero = cantidad + '0';
                sprintf(buffer, "Juego/Mazo/%s %s %c.txt", tipoCarta[tipo], colorCarta[color], numero);
                carta = fopen(buffer, "w");
                fclose(carta);
                strcpy(buffer, "");
            }
        }
        
        // Los 0 se generan solo 1 para cada color, por lo que quedan en el ciclo de m?s afuera
        strcpy(buffer, "");
        sprintf(buffer, "Juego/Mazo/0 %s.txt", colorCarta[color]);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");

        // Las cartas +4 y comodines se crean 4 veces, pero sin ningun color.
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
    printf("\n ***** Se crearon 108 cartas en el directorio Juego/Mazo.\n\n");
    }



mazo* leerMazo(){
    // Se leen todas las cartas dentro del directorio Mazo
    
    mazo *Mazo = malloc(sizeof(mazo));
    struct dirent *dir;

    int count = 0;
    directorio = opendir("Juego/Mazo/.");
    // se revizan todos los archivos bajo el directorio Juego/Mazo/
    
    if (directorio){
        while ((dir = readdir(directorio)) != NULL){
            if ((strcmp(dir->d_name, ".") != 0 && (strcmp(dir->d_name, "..") != 0))){
                strcpy(Mazo->cartas[count],dir->d_name);
                count++;
                // Se a?aden en el struct, recibiendo un indice entre [0-108]
            }
        }
        Mazo->n_cartas = count;
        closedir(directorio);
    }
    return Mazo;
}


void randPull(mazo* Mazo, int opcion, int jugador){
    
    time_t t;
    srand((unsigned) time(&t));

    int r = rand() % Mazo->n_cartas;

    char comm[200] = "mv Juego/Mazo/'";
    char strJ[2];
    *strJ = jugador + '0';


    strcat(comm, Mazo->cartas[r]);
    strcat(comm, "'");

    strcpy(Mazo->cartas[r], Mazo->cartas[Mazo->n_cartas-1]);

    Mazo->n_cartas = Mazo->n_cartas - 1;

    switch(opcion) {
        case 0:
            strcat(comm, " Juego/Jugadores/Jugador_");
            strcat(comm, strJ);
            break;

        case 1:
            system("rm -f Juego/Last/*");
            strcat(comm, " Juego/Last");
            break;

        default:
            puts("opcion invalida");
    }

    system(comm);
}


int buscarPrevio(int situacion){ 
    char opcion;
    int verificacion = 1;

    if (situacion == 1){
        while (verificacion == 1){
            printf("Existe una partida anterior guardada. Desea continuarla? (S|N): ");
            scanf(" %c", &opcion);

            if (opcion == 'S' || opcion == 's'){
                printf("Se retomaran los datos anteriores.\n");
                return 1;
            }
            else if (opcion == 'N' || opcion == 'n'){
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
                if (opcion == 'S' || opcion == 's'){
                    system("rm -r Juego");  // Se elimina la carpeta padre, y se muere todo lo que est? adentro. izi pizi.
                    verificacion = 0;
                    closedir(directorio);
                    return 0;
                }
                else if (opcion == 'N' || opcion == 'n'){
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
        ///////////7
        printf("Enviando 'S' se terminara el programa dejando los datos en sus respectivas carpetas\n");
        printf("En la proxima partida se eliminara todo, y se iniciara de 0, ya que aun no esta del todo implementado.\n");
        ////////////////
        printf("Desea guardar su partida? (S|N): ");
        scanf(" %c", &opcion);
        if (opcion == 'S' || opcion == 's'){
            printf("Se guardaran los datos de la partida\n");
            system("mkdir -p Juego/gameIsSaved");
            FILE* Dummy; 
            Dummy = fopen("Juego/gameIsSaved/dummy.txt", "w");
            fprintf(Dummy, "Hola, si me ves aqui es porque grabaste tu partida, y me aseguro que la carpeta exista.\nLogre mi mision! Yupy!");
            verificacion = 0;
            fclose(Dummy);
        }
        else if (opcion == 'N' || opcion == 'n'){
            verificacion = 0;
            printf("Se eliminaron los datos de la partida.\n");
            system("rm -r Juego");
            return;
        }
        else printf("Entrada invalida.\n");
    }
    return; // Nunca deberia llegar a este punto.
}


int main(){
 
    int restaurar = 0, jugador, carta, index;
 
    
    for (index = 0 ; index < 4 ; index++){
        pipe(pipes[0]);
    }
    
    //directorio = opendir("Juego/gameIsSaved");
    //if (directorio){ 
    //    restaurar = buscarPrevio(1);
    //}
    //else restaurar = buscarPrevio(2);
    if (restaurar != 1){
        directorio = opendir("Juego");
        if (directorio) system("rm -r Juego");
        setup();
    }
    closedir(directorio);

    mazo *Mazo = leerMazo();
    
    for(jugador = 1; jugador <= 4; jugador++){
        for(carta = 0; carta < 7; carta++){
            randPull(Mazo, 0, jugador);
        }
    }
    
    printf("\n***Se le han asignado 7 cartas a cada jugador, y se coloc? una carta en la carpeta 'Last'\n\n\n");
    
    //pozo
    randPull(Mazo, 1, -1);

    int proceso, contador;  // Se crea un array para los PIDs de cada uno de los procesos
    procesos[0] = getpid(); // la primera posicion del arreglo es el PID del padre
    
    for (contador = 0 ; contador < 3 ; contador++){
        proceso = fork();
        if (proceso == 0){
            procesos[contador+1] = getpid();
            break;
        }
        else procesos[contador+1] = proceso;
    }
    
    
    // CONFIGURACION DE PIPES
    
    if (getpid() == procesos[0]){   // Cierra los pipes que el padre no utiliza.
        for (index = 1; index < 4 ; index++) close(pipes[index][0]);
        close(pipes[0][1]);
    }
    
    else if (getpid() == procesos[1]){  // Cierra los pipes que el hijo 1 no utiliza
        for (index = 0 ; index < 4 ; index++){
            if (index == 0) close(pipes[0][0]);
            else if (index == 1) close(pipes[1][1]);
            else{
                close(pipes[index][0]);
                close(pipes[index][1]);
            }
        }
    }
            
    else if (getpid() == procesos[2]){ // Cierra los pipes que el hijo 2 no utiliza
        for (index = 0 ; index < 4 ; index++){
            if (index == 0) close(pipes[0][0]);
            else if (index == 2) close(pipes[2][1]);
            else{
                close(pipes[index][0]);
                close(pipes[index][1]);
            }
        }
    }    

    else if (getpid() == procesos[3]){  // Cierra los pipes que el hijo 3 no utiliza
        for (index = 0 ; index < 4 ; index++){
            if (index == 0) close(pipes[0][0]);
            else if (index == 3) close(pipes[3][1]);
            else{
                close(pipes[index][0]);
                close(pipes[index][1]);
            }
        }
    }
    
    
    //printf("%d\n", getpid());
    
    if (getpid() == procesos[1]){
      //  printf("entro\n");
        char string[] = "Soy el proceso 1\n";
        write(pipes[0][1], string, strlen(string)+1);
    }
    
    if (getpid() == procesos[2]){
        char string[] = "Soy el proceso 2\n";
        write(pipes[0][1], string, strlen(string)+1);
    }
    
    if (getpid() == procesos[3]){
        char string[] = "Soy el proceso 3\n";
        write(pipes[0][1], string, strlen(string)+1);
    }
    
    if (getpid() == procesos[0]){
        for (index = 0; index < 3; index++){
            int x = read(pipes[0][0], buffer, sizeof(buffer));
            printf("String: %s", buffer);
        }
    }
            
    //sleep(4);
    //printf(" %d: %d %d %d %d\n", getpid(), procesos[0], procesos[1], procesos[2], procesos[3]);
    
    
    
    
        
    //terminarPartida();
    
    free(Mazo);
    return 0;
}