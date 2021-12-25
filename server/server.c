/**
 * @file server.c
 * @author GANGAT Bilal et SLIMANI Missipssa
 * @brief 
 * @version 0.1
 * @date 2021-05-16
 * @copyright Copyright (c) 2021
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include<pthread.h>
#include <sys/types.h> //kill
#include <signal.h> // signal

#define FAUX 0
#define VRAI 1

#define BUFFER_SIZE 256
#define nb_thread 20
#define couleur(param) printf("\033[%sm",param)

char continue_a_travailler=VRAI;
char ecrire = FAUX;

int sock_pipe;
int sock;
int *arg;

int quest;
int score;
char nom[BUFFER_SIZE];

char* questions[] = {"-Quelle est la capitale de France ?",
                     "-Comment dit-on <Bienvenue> en anglais ?",
                     "Qelle est la formule chimique de l'eau ?",
                     "-ou se trouve le BigBen ?",
                     "-qui a gagné la coupe du monde 2014 ?"};

char* rep[] = {"paris",
              "welcome",
               "h2o",
               "londres",
               "allemagne"};


void graceful_shutdown(int parametre){
  printf("Je termine ce que je fais puis je m'arrête\n");
  continue_a_travailler = FAUX;
  if(ecrire == VRAI){
    if (write(sock_pipe, "STOP", BUFFER_SIZE) < 0) {
      perror("Erreur de send coté serveur");
    }
    printf("j'attends que le client termine\n");
  }
  else{
    close(sock);
    exit(0);
  }
}

/**********************************************************************/
/**
   IConvertis un entier en une chaine de caractere pour l'envoyer au client.
   @param socket la socket qui permet de communiquer.
   @param tab la chaine destination, celle qui recupere l'entier
   @param n l'entier à convertir
   @require socket != NULL
*/
void convert(int socket, char* tab, int n){
  sprintf(tab, "%d", n);
    if (write(socket, tab, BUFFER_SIZE) < 0) {
      perror("Erreur de send coté serveur");
    }
}

/**********************************************************************/
/**
   permet de quitter le jeu et de fermer la socket.
   @param socket la socket qu'il faut fermer 
   @require socket != NULL
*/
void quitter(int socket){
  close(socket);
  printf("le client a quitter le jeu\n");
}

/**********************************************************************/
/**
   Permet de demander le nom d'utilisateur au client pour le recuperer dans la variable nom.
   @param socket la socket qui permet de communiquer avec la client
   @param nom le nom a initialise
   @require socket != NULL
   @return nom
*/
void* demande(int socket, char* nom){
  char id[BUFFER_SIZE]={0};
  if (write(socket, "Entrez votre nom d'utilisateur: ", BUFFER_SIZE) < 0) {
    perror("Erreur de send coté serveur");
  }
  if(read(socket,id, BUFFER_SIZE) < 0){
      perror("Erreur de lecture sur la socket");
  }
  if(strcmp("2", id) == 0){
  quitter(socket);
  return 0;
  }
  else{
    strcpy(nom, id);
  }
  return nom;
}

/**********************************************************************/
/**
   Permet d'envoyer le menu à l'utilisateur, pour qu'il puisse choisir de quitter ou de débuter le jeu.
   @param socket la socket qui permet de communiquer avec le client.
   @require socket != NULL
   @return 1 si retour = 1, 0 sinon0
*/
int menu(int socket) {
  char retour[BUFFER_SIZE]={0};
	char *menu = "\nQuelle action souhaitez vous réaliser? \n   '1' débuter le jeu \n   '2' quitter le programme ";
  printf("le client choisi..\n");
  if (write(socket, menu, BUFFER_SIZE) < 0) {
    perror("Erreur de send coté serveur");
  }
  if(read(socket,retour, BUFFER_SIZE) < 0){
    perror("Erreur de lecture sur la socket");
  }
  if(strcmp("1", retour) == 0){
    return 1;
  }
  else if(strcmp("2", retour) == 0){
    quitter(socket);
    return 0;
  }
  close(socket);
  return 0;
}

/**********************************************************************/
/**
   cette fonction verifie si le nom d'utilisateur rentré est existant dans le fichier save.txt, 
   si oui, il charge l'ancienne partie.
   @param nom le nom du joueur
   @require f1 != NULL
*/
int chargement(const char* nom){
  FILE * f1;
  char pseudo[1024]= {0};
  f1 = fopen("./save.txt", "a+");
  if(f1 == NULL){
    printf("echec de l'ouverture du fichier\n");
    return 0;
  }

  while(fscanf(f1, "%s\t%d\t%d",pseudo,&quest,&score)!= EOF){
    if(strcmp(pseudo, nom) == 0){
      fclose(f1);
      return 1;
    }
  }
  fclose(f1);
  return 0;
}

/**********************************************************************/
/**
   Cette fonction ouvre un fichier save.txt, et sauvegarde la partie sous format "nom question score". 
   Cette fonction écrase l'ancienne partie si le client a deja joué.
   @param nom le nom d'utilisateur du client.'
   @param quest le numero de question
   @param score le score du client
   @require f1 != NULL
*/
void sauvegarde(const char* nom, int quest, int score){
  FILE* f1;
  int cond=0;
  char pseudo[1024]= {0};
  f1 = fopen("./save.txt", "r+a");
  if(f1 == NULL){
    printf("echec de l'ouverture du fichier\n");
    exit(0);
  }
  while(fscanf(f1, "%s\n",pseudo)!= EOF){
    if(strcmp(pseudo, nom) == 0){
      fprintf(f1, "%d\t%d\n", quest, score);
      cond = 1;
    }
  }
  if(cond == 0){
    fprintf(f1, "%s\t%d\t%d\n", nom, quest, score);
  }
  fclose(f1);
}

/***********************************************************************/
/**
   Notre mini-jeu qui envoie les questions, lit les réponses et les vérifie également.
   @param socket est la socket qui permet de communiquer entre le client et le serveur
   @param question est le numéro de question en cas de chargement d'une ancienne partie
   @param score est le score en cas de chargement d'une ancienne partie.
   @require socket != NULL
   @return 0
*/
int culture_g(int socket, int question, int score){
  char reponse[BUFFER_SIZE] ={0}, fin[BUFFER_SIZE]={0}, x[BUFFER_SIZE]={0};
  int i;
  convert(socket, x, question);
  for(i = question; i < 5; i++){
    if (write(socket, questions[i], BUFFER_SIZE) < 0) {
      perror("Erreur de send coté serveur");
    }
    if(read(socket,reponse, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
    }
    if(strcmp(reponse, "q") == 0){
      sauvegarde(nom, i, score);
      convert(socket, fin, score);
      quitter(socket);
      return 0;
    }

    if(strcmp(reponse, rep[i]) == 0){
        score+=1;
        couleur("32");
        printf("Bonne réponse, score = %d\n", score);
        couleur("0");
    }
    else{
      couleur("31");
      printf("mauvaise réponse, score = %d\n", score);
      couleur("0");
    }
    if(i == 4){
      sauvegarde(nom,0,0);
      convert(socket, fin, score);
      printf("Le client a fini le jeu\n");
    }
  }
  return 0;
}

/***********************************************************************/
/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, short mon_port){
  serveur->sin_family = AF_INET;
  serveur->sin_port = htons(mon_port);
	serveur->sin_addr.s_addr = htonl(INADDR_ANY);
}

/***********************************************************************/
/**
   Initialise une socket permettant d'accepter des connexions en TCP depuis n'importe
   qu'elle adresse IP.
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
   @return une socket pouvant accepter des connexions si aucune erreur de survient,
           quitte le programme sinon.
*/
int initialiser_socket(short mon_port){
  struct sockaddr_in serveur;
  socklen_t serveur_len = sizeof(serveur);
  int sock = 0; // sera la socket de communication TCP
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("[-]Erreur de creation de socket coté serveur\n");
    return EXIT_FAILURE;
  }
  printf("[+]Creation du socket reussi\n");

  init_sockaddr_in(&serveur, mon_port);
  if (bind(sock, (struct sockaddr *) &serveur, serveur_len) < 0) {
    perror("[-]Erreur de bind de la socket coté serveur\n");
    return EXIT_FAILURE;
  }
  printf("[+]Bind reussi\n");
  if((listen(sock, 2)) < 0){
    perror("[-]Erreur lors de l'appel à la fonction listen\n");
    return EXIT_FAILURE;
  }
  printf("[+]J'ecoute...\n**********************************************\n");
  return sock;
}

/**********************************************************************/
void fermer(){
      close(sock);
      free(arg);
      close(sock_pipe);
      exit(0);
}

/***********************************************************************/
/**
   Cette fonction permet d'echanger avec le client, elle recupere les connexions, 
   et envoie aux clients le jeu. une fois terminer, il ferme la socket, libere la memoire et quitte le thread.
   @param arg un pointeur sur la socket
 */
void *echange_avec_client(void *arg){
  int socket=*(int*)arg;
  if(((menu(socket)) == 0) || ((demande(socket, nom)) == 0)){
    close(socket);
    free(arg);
    pthread_exit(NULL);
  }
  if(chargement(nom) == 1){
    culture_g(socket, quest, score);
  }
  else{
    culture_g(socket, 0, 0);
  }
  if(continue_a_travailler == FAUX){
    fermer();
  }
  close(socket);
  free(arg);
  pthread_exit(NULL);
}

/***********************************************************************/
/**
 * le maine lit le port passé en parametre au moment de l'execution, recois la connexion
 * et appel les threads pour envoyer le jeu au client.
   @param argc le nombre de parametre entré
   @param argv les parametre entré au moment de l'execution
   @require argc == 2
*/
    int main(int argc, char ** argv) {
  struct sockaddr_in client; 
  int len = sizeof(client);
  short port;
  int i=0;
  if (argc!=2) {
    fprintf(stderr,"usage: %s port\n", argv[0]);
    return EXIT_FAILURE;
  }
  port = atoi(argv[1]);
  signal(SIGINT, graceful_shutdown);
  sock = initialiser_socket(port);
  pthread_t th[nb_thread];
  while((continue_a_travailler == VRAI) && (sock_pipe = accept(sock, (struct sockaddr *)&client, (socklen_t *) &len))){
    ecrire = VRAI;
    arg=malloc(sizeof(int));
    *arg=sock_pipe;
    pthread_create(&th[i], NULL, echange_avec_client, arg);
    i++;
  }
  while(i < 0){
    pthread_join(th[i],NULL);
    i--;
  }
  printf("commencer la fermeture\n");
  close(sock_pipe);
  printf("close\n");

  return EXIT_SUCCESS;
}