#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>


typedef struct info{
    int players[4];
} info;


typedef struct mazo{
    char cartas[108][50];
    int n_cartas;
} mazo;


typedef struct jugada{
    int parametros[5];
    int n_eliminadas;
    int eliminadas[10];
    char carta[50];
} jugada;


// PIPES

int pipes[4][2], procesos[4] = {0,0,0,0};
char primeraCarta[50];

// Struct para el guardado de las cartas. Tiene espacio como para las 108 cartas del juego.


DIR *directorio;


/*
 Funcion Setup:  No recibe ni retorna nada.
 Genera el juego inicial, creando los directorios necesarios, junto con todas las cartas del mazo.
 Se utiliza un directorio general para todo el juego, que es la carpeta "Juego"
 */
void setup(){

    system("mkdir -p  Juego/Mazo");  // Directorio principal de las cartas
    system("mkdir -p Juego/Last"); // Directorio con la ultima carta jugada

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
        sprintf(buffer, "Juego/Mazo/+4 Negro %c.txt", numero);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");

        sprintf(buffer, "Juego/Mazo/Comodin Negro %c.txt", numero);
        carta = fopen(buffer, "w");
        fclose(carta);
        strcpy(buffer, "");
        }
    printf("\n\t\t ***** Se crearon 108 cartas en el directorio Juego/Mazo.\n\n");
    }


mazo* leerMazo(){
    // Se leen todas las cartas dentro del directorio Mazo

    mazo *Mazo = malloc(sizeof(mazo));
    struct dirent *dir;

    int count = 0;
    directorio = opendir("Juego/Mazo/.");
    // se revisan todos los archivos bajo el directorio Juego/Mazo/

    if (directorio){
        while ((dir = readdir(directorio)) != NULL){
            if ((strcmp(dir->d_name, ".") != 0 && (strcmp(dir->d_name, "..") != 0))){
                strcpy(Mazo->cartas[count],dir->d_name);
                count++;
                // Se anaden en el struct, recibiendo un indice entre [0-108]
            }
        }
        Mazo->n_cartas = count;
        closedir(directorio);
    }
    return Mazo;
}


//elimina .txt de nombre de archivo
char* sintxt(char* cartaFile){

    char *nombreCarta = malloc(50*sizeof(char));                                //MALLOC
    strncpy(nombreCarta, cartaFile, strlen(cartaFile)-4);

    return nombreCarta;
}


int esDigito(char* P){
    int i;
    for(i = 0; P[i]; i++){
        if(P[i] < '0' || P[i] > '9'){
            return 0;
        }
    }
    return 1;
}


char* analizarCarta(char* carta){
    //carta: numero + color + numero de copia de archivo

    //infoCarta = color + tipo carta + numero + orientacion

    //ncartas : 0 = dont care, 1 = tiene 1 carta, 2 = ganó
    //tipo carta : 0 = numero, 1 = reversa, 2 = comodin, 3 = bloqueo, 4 = +2, 5 = +4
    //color : 0 = negro, 1 = rojo, 2 = azul, 3 = verde, 4 = amarillo


    //se separan los elementos del string carta
    char datosCarta[3][50] = {"","",""};
    char aux[2] = "";
    int c, z = 0;

    for(c = 0; carta[c]; c++){

        if (carta[c] != ' '){
            aux[0] = carta[c];
            strcat(datosCarta[z],aux);
        }

        else{
            strcpy(aux,"");
            z++;
        }
    }

    //se interpretan los valores obtenidos
    int analisis = 0;
    char tipoCarta[6][10] = {"Numero", "Reversa", "Comodin", "Bloqueo", "+2", "+4"};
    char color[5][10] = {"Negro", "Rojo", "Azul", "Verde", "Amarillo"};


    //tipo carta
    int i;
    for (i = 0; i < 6; i++){

        if (strcmp(datosCarta[0], tipoCarta[i]) == 0){
            analisis += i * 100;
        }
    }

    //tipo carta == numero
    if (esDigito(datosCarta[0])){
        analisis += (int)datosCarta[0][0] * 10;
    }

    //color
    for (i = 0; i < 5; i++){
        if (strcmp(datosCarta[1],color[i]) == 0){
            analisis += i * 1000;
        }
    }

    //orientacion
    if (*datosCarta[1] == 1){
        analisis += 1;
    }


    char *analisisStr = malloc(5*sizeof(char));
    *analisisStr = analisis + '0';

    return analisisStr;
}


char* randPull(mazo* Mazo, int opcion, int jugador){

    time_t t;
    srand((unsigned) time(&t));

    int r = rand() % Mazo->n_cartas;

    char comm[100] = "mv Juego/Mazo/'";
    char strJ[2] = "";
    *strJ = jugador + '0';

    strcpy(primeraCarta, Mazo->cartas[r]);
    strcat(comm, Mazo->cartas[r]);
    strcat(comm, "'");

//retorna informacion de carta
    char* nombreCarta = sintxt(Mazo->cartas[r]);
    //printf("%s, %s\n", Mazo->cartas[r], nombreCarta);           //ERROR<<<<<<<<<<<<<<<<<<<<<

//se reemplaza carta sacada por ultima en arreglo
    strcpy(Mazo->cartas[r], Mazo->cartas[Mazo->n_cartas-1]);

    Mazo->n_cartas = Mazo->n_cartas - 1;

    switch(opcion) {
        case 0:
            strcat(comm, " Juego/Jugadores/Jugador_");
            strcat(comm, strJ);
            break;

        case 1:
            strcat(comm, " Juego/Last");
            break;

        default:
            puts("opcion invalida");
    }

    system(comm);
    return nombreCarta;
}


void pushCarta(mazo* Mano, int cartaIndex, int jugador){

    system("rm -f Juego/Last/*");

    char comm[200] = "mv /Juego/Jugadores/Jugador_";
    char player[2] = {jugador + '0','\0'};

    strcat(comm, player);
    strcat(comm, "/");
    strcat(comm, Mano->cartas[cartaIndex]);
    strcat(comm, ".txt Juego/Last");
}


jugada* Jugar(char *cartaLast, int jugador, mazo* Mazo){
    //infoCarta = color + tipo carta + numero + orientacion
    //ncartas : 0 = dont care, 1 = tiene 1 carta, 2 = ganó
    //tipo carta : 0 = numero, 1 = reversa, 2 = comodin, 3 = bloqueo, 4 = +2, 5 = +4
    //color : 0 = negro, 1 = rojo, 2 = azul, 3 = verde, 4 = amarillo

    //jugador == 0
    //dependiendo de condiciones enviar pipe a siguiente jugador o dejar jugar a 1

    mazo* Mano = malloc(sizeof(mazo));

    jugada* J = malloc(sizeof(jugada*));


/*
    int parametros[5];
    int n_eliminadas;
    int eliminadas[10];
    char carta[50];
*/


    struct dirent *dir;

    char dirMano[100] = "Juego/Jugadores/Jugador_";

    char strJ[2] = "";
    *strJ = jugador + '0';      //jugador de int a string

    strcat(dirMano, strJ);
    directorio = opendir(dirMano);


//imprimir cartas de la mano y registrarlas en arreglo
    int i = 0;
    if (directorio){
        while ((dir = readdir(directorio)) != NULL){
            if ((strcmp(dir->d_name, ".") != 0 && (strcmp(dir->d_name, "..") != 0))){

                char* nombreCarta = sintxt(dir->d_name);    //eliminar .txt
                strcpy(Mano->cartas[i], nombreCarta);

                printf("[%d] %s\n", i, Mano->cartas[i]);

                free(nombreCarta);

                i++;
            }
        }
    }
    closedir(directorio);

    Mano->n_cartas = i;

    printf("[%d] %s\n", i, "Sacar de mazo");
    printf("[%d] %s\n", i + 1, "Pasar");

    printf("\nUltima jugada: \n%s\n", cartaLast);


//analisis ultima jugada
    char* LastDatos = analizarCarta(cartaLast);
    free(cartaLast);

//orientacion (depende de ultima jugada)
    int siguienteJugador;
    if (LastDatos[3] == 0){

        if (jugador == 4) siguienteJugador = 1;
        else siguienteJugador = jugador + 1;
    }
    else{

        if (jugador == 1 || jugador == 0) siguienteJugador = 4;
        else siguienteJugador = jugador - 1;
    }


    puts("\nELEGIR CARTA (segun indice)\n");

    int opcion, opcionValida = 0;
    scanf(" %d", &opcion);

    while(opcionValida == 0){

        if(opcion == i + 1){
            //return
            break;
        }

        else if (opcion == i){

            char* cartaPull = randPull(Mazo, jugador, 0);
            strcpy(Mazo->cartas[i],"");
            strcpy(Mazo->cartas[i], cartaPull);
            Mazo->n_cartas += 1;

            printf("Sacando de mazo... \n%s\n", cartaPull);

            char* cartaAnalizada = analizarCarta(cartaPull);
            free(cartaPull);

            /*FALTA IMPLEMENTAR
            if (cartaValida(cartaAnalizada, LastDatos)){

            }
            else{
                //return
            }*/
            //chequear si puede hacer la jugada. Si no, pasa.
            opcionValida = 1;

        }


        else if (opcion >= 0 && opcion < i){

            opcionValida = 1;
            Mazo->n_cartas -= 1;

            char* cartaDatos = analizarCarta(Mano->cartas[opcion]);

            //negra
            if (cartaDatos[0] == '0'){

                int nuevoColor;
                puts("Elegir nuevo color: \n");
                puts("[1] Rojo");
                puts("[2] Azul");
                puts("[3] Verde");
                puts("[4] Amarillo");
                scanf(" %d", &nuevoColor);

                if (cartaDatos[1] == '5'){

                    int c;
                    for (c = 0; c < 4; c++){
                        randPull(Mazo, 0, siguienteJugador);
                    }

                    siguienteJugador = jugador + 2;

                    break;
                }

                else if(cartaDatos[1] == '2'){
                    //comodin
                    //no pasa nada en realidad jsj
                    break;
                }
            }

            //infoCarta = color + tipo carta + numero + orientacion
            //tipo carta : 0 = numero, 1 = reversa, 2 = comodin, 3 = bloqueo, 4 = +2, 5 = +4

        //Si se pone una carta que no cumpla esto, se debe robar una carta.
            //colores
            if (cartaDatos[0] == LastDatos[0] || cartaDatos[2] == LastDatos[2]){

                if(cartaDatos[1] == '3'){
                    siguienteJugador = jugador + 2;
                }

                else if(cartaDatos[1] == '4'){
                    int c;
                    for (c = 0; c < 2; c++){
                        randPull(Mazo, 0, siguienteJugador);
                    }
                }
                else{
                    //probablemente reversa, pero ese caso ya se vio ps uwu
                }

            }


            else{
                puts("Jugada no permitida. Sacar carta de mazo");

                char* cartaPull = randPull(Mazo, jugador, 0);   //en vola esto podria ser una funcion
                strcpy(Mazo->cartas[Mazo->n_cartas], cartaPull);       //chequear
                Mazo->n_cartas += 1;

                printf("Sacando de mazo... \n%s\n", cartaPull);


            }

            free(cartaDatos);

        }

        if (opcionValida == 0){

            for (i = 0; i < Mano->n_cartas; i++){
                printf("[%d] %s\n", i, Mano->cartas[i]);
            }

            puts("Elegir opcion");
            scanf(" %d", &opcion);
        }
    }

//se pone la carta en monton
    pushCarta(Mano, opcion, jugador);

    J->parametros[0] = Mano->n_cartas;
    J->parametros[1] = siguienteJugador;
    //J->n_eliminadas = cartasEliminadas;     //falta implementar
    strcpy(J->carta, Mano->cartas[opcion]);

    free(Mano);

    /*
    //enviar pipe a padre con carta y n_jugador siguiente
    if (jugador == 0){

    }
    else{

    }
    */

    return J;
}



/*// Recibe el mazo, un entero que representa la cantidad de cartas que se deben eliminar
// Y un arreglo  con los indices de las cartas que deben eliminarse del mazo.
void eliminarCartas(mazo* Mazo, int nCartas, int *Cartas){
    int index;
    for (index = 0 ; index < nCartas; index++){
        strcpy(Mazo->cartas[Cartas[index]], Mazo->cartas[Mazo->n_cartas - 1]);
        Mazo->n_cartas = Mazo->n_cartas - 1;
    }
}
*/


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


// Funcion de Juego del Player 1
void jugadorPrincipal(mazo *Mazo){  // Funcion que controla al proceso Padre

    // Debe hacer la primera jugadada de la partida, e indicar los resultados

    jugada *Jugada = malloc(sizeof(jugada));
    char *primera = sintxt(primeraCarta);
    Jugada =  Jugar(primera, 0, Mazo);

    info *Info = malloc(sizeof(info));

    int index = 0;
    for (index = 0; index < 4; index++){
        Info->players[index] = 0;
    }
//  Realiza una "jugada fantasma" con la carta que se coloca en el pozo al azar al comienzo de la partida
//  Esto porque si la primera carta salta el turno del jugador 1, se debe comenzar con otro jugador.


    //if (Jugada->n_eliminadas > 0) eliminarCartas(Mazo, Jugada->n_eliminadas, Jugada->eliminadas);

    int proxJugador = Jugada->parametros[1];
    int jugadorActual = 0, jugadores = 0;

    // Se obtiene a que jugador le toca el primer turno, y se ejecuta la funcion como es de esperarse.

    int enPartida = 1;


            // Los structs Jugada e Info contienen informacion relevante para coordinar las jugadas.
            // Info permite indicar a qu? jugadores les queda 1 carta, m?s de 1, o ya ganaron.
            // Jugada contiene 3 parametros. Cantidad de cartas (>1, 1 o 0), a quien le toca el siguiente turno
            // Y el string con la ultima carta que se jug?.

            // Awante mandar structs por pipes c:  esta muy util

    while (enPartida == 1){

        if (proxJugador != 1){   // El caso en el que el turno es de otro jugador

            jugadorActual = Jugada->parametros[1];

            write(pipes[proxJugador-1][1], Jugada, sizeof(jugada));
            write(pipes[proxJugador-1][1], Mazo, sizeof(mazo));
            write(pipes[proxJugador-1][1], Info, sizeof(info));

            // Estos structs se envian por pipe, y son recibidas por el siguiente jugador.

            free(Info);
            free(Jugada);
            free(Mazo);

            read(pipes[0][0], Jugada, sizeof(jugada));
            read(pipes[0][0], Mazo, sizeof(mazo));
            read(pipes[0][0], Info, sizeof(info));

            // El jugador1 recibe la respuesta de la jugada realizada por el proceso hijo, los guarda y continua con el juego

            Jugada->parametros[2] = jugadorActual;
            proxJugador = Jugada->parametros[1];

            // Esto signific que cuando le toca jugar a cualquiera que no sea el J1, se le envia la informacion que requiere
            // para hacer su movimiento, y devuelve por pipe la misma informacion, pero para el siguiente turno.
        }

        else{  // Le toca jugar al jugador 1.

            jugadorActual = Jugada->parametros[1];

            Jugada = Jugar(Jugada->carta, 1, Mazo);
            // if (Jugada->n_eliminadas > 0) eliminarCartas(Mazo, Jugada->n_eliminadas, Jugada->eliminadas);

            for (jugadores = 1 ; jugadores < 4 ; jugadores++){
                if(Info->players[jugadores] == 1)
                    printf("El jugador %d tiene 1 sola carta!\n", jugadores);
            }

            // Si le toca jugar al jugador 1, realiza su jugada y luego le env?a los resultados al jugador que sigue.

            Jugada->parametros[2] = jugadorActual;
            proxJugador = Jugada->parametros[1];

            write(pipes[proxJugador-1][1], Jugada, sizeof(jugada));
            write(pipes[proxJugador-1][1], Mazo, sizeof(mazo));
            write(pipes[proxJugador-1][1], Info, sizeof(info));

            free(Info);
            free(Jugada);
            free(Mazo);


        } // Si la primera jugada la hace el jugador 1
    }
}


// Funcion de juego de los procesos hijos
// No tiene tanto comentario porque es similar a lo del padre
void jugadorPC(){
    int pipeJugador = 0; // Numero del pipe que se comunica con el Padre

    if (getpid() == procesos[1]) pipeJugador = 1;   // Obtiene el numero del pipe que utiliza para recibir mensajes del padre
    else if (getpid() == procesos[2]) pipeJugador = 2;
    else if (getpid() == procesos[3]) pipeJugador = 3;


    int enPartida = 1;

    while (enPartida == 1){

        jugada *Jugada = malloc(sizeof(jugada));
        mazo *Mazo = malloc(sizeof(mazo));
        info *Info = malloc(sizeof(info));

        read(pipes[pipeJugador][0], Jugada, sizeof(jugada)); // Recibe la informacion enviada por el padre
        read(pipes[pipeJugador][0],   Mazo, sizeof(mazo));
        read(pipes[pipeJugador][0],   Info, sizeof(info));

        int jugadores = 0;

        for (jugadores = 0 ; jugadores < 4 ; jugadores++){
            if((Info->players[jugadores] == 1) && (jugadores != pipeJugador))
                printf("P%d: El jugador %d tiene 1 sola carta!\n", pipeJugador, jugadores);
            else if (Info->players[jugadores] == 2){
                if (jugadores == pipeJugador) printf("P%d: Has ganado!\n", pipeJugador);
                else printf("P%d: Ha ganado el jugador %d!\n", pipeJugador, jugadores);
            }
        }  // Verifica quienes tienen 1 sola carta y a quien le queda 0. (Aun no termina la partida)

        free(Jugada);

        Jugada = Jugar(Jugada->carta, pipeJugador + 1 ,Mazo);
       // if (Jugada->n_eliminadas > 0) eliminarCartas(Mazo, Jugada->n_eliminadas, Jugada->eliminadas);

        Info->players[pipeJugador] = Jugada->parametros[0];

        // Realiza su jugada y le informa al proceso padre el resultado y a quien le toca jugar ahora.
        write(pipes[0][1], Jugada, sizeof(jugada));
        write(pipes[0][1], Mazo, sizeof(mazo));
        write(pipes[0][1], Info, sizeof(info));

        free(Info);
        free(Jugada);
        free(Mazo);
    }
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

// Se asignan las cartas a las manos de cada jugador.
    for(jugador = 1; jugador <= 4; jugador++){
        for(carta = 0; carta < 7; carta++){
            free(randPull(Mazo, 0, jugador));
        }
    }

    printf("\n\t***Se le han asignado 7 cartas a cada jugador, y se coloc? una carta en la carpeta 'Last'\n\n\n");

    //pozo
    free(randPull(Mazo, 1, -1));

    char* cartax = sintxt("7 Azul 1");
    free(Jugar(cartax, 1, Mazo));
    free(cartax);


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
    /*

     Se utilizan 4 pipes, guardados en un arreglo global, llamados Pipes[0], ... , Pipes[3]

     Todos los hijos escriben en Pipes[0], y el padre solo lee ese
     Ademas, el padre posee un 3 pipes, con los que se comunica con los 3 hijos Pipe1: P-h1, Pipe2: P-h2, Pipe3: P-h3

    */
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


    if (getpid() == procesos[0]){
        jugadorPrincipal(Mazo);
        free(Mazo);
    }

    else{
        free(Mazo);
        jugadorPC();
    }
    //terminarPartida();
    return 0;
}
