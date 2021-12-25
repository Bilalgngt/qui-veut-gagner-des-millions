/**
 * @file client.c
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
#include <stdbool.h>

#define couleur(param) printf("\033[%sm",param)
#define BUFFER_SIZE 256




/***********************************************************************/
/**
   permet de quitter le jeu et de fermer la socket.
   @param sock la socket qu'il faut fermer 
   @require sock != NULL
   @return 0
*/
void* quitter(int sock){
  printf("vous avez demande la fermeture du jeu.\n");
  close(sock);
  return 0;
}

/***********************************************************************/
/**
   Permer de lire le menu ainsi que le nom d'utilsateur demandé par le serveur.
   @param sock la socket
   @param reponse la chaine de caractere à envoyer.
   @param question la chaine de caractere recu.
   @require sock != NULL
   @return 1 si le client souhaite fermer, 0 sinon.
*/
int demande(int sock, char* reponse, char* question){
  int continuer = 0;
  do{

    if(read(sock,question, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
    }
    if(strcmp(question, "STOP") == 0){
      couleur("31");
      printf("\nLe serveur doit redemarrer, veuillez finir votre partie.\n\n");
      couleur("0");
      if(read(sock,question, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
      }
    }
    couleur("33");
    printf("%s\n", question);
    couleur("0");
    if(continuer == 0){
      printf("votre choix : ");
    }
    else printf("votre nom : ");
    scanf("%s",reponse);

    if(write(sock, reponse, BUFFER_SIZE) < 0) {
      perror("Erreur de send coté client");
    }
    continuer+=1;

  }while((strcmp(reponse, "2" ) != 0) && (continuer < 2));
    if(strcmp(reponse, "2" ) == 0){
      printf("Vous avez demande la fermeture du jeu.\n");
      return 1;
    }
    return 0;
}

/***********************************************************************/
/**
   Permet de recevoir les questions du jeu et d'y repondre.
   @param sock la socket pour communiquer
   @param envoi la chaine de caractere à envoyer.
   @param question la chaine de caractere recu.
   @param reponse la chaine de caractere recevant le numero score du client.
   @require serveur != NULL
   @return 0
*/
void* jeu(int sock, char* reponse, char* question, char* envoi){
  int x=0;
  do{

    if(x == 0){
      if(read(sock,question, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
      }
      x = atoi(question);
    }
    if(strcmp(question, "STOP") == 0){
      couleur("31");
      printf("\nLe serveur doit redemarrer, veuillez finir votre partie.\n\n");
      couleur("0");
      if(read(sock,question, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
      }
    }

    if(read(sock,reponse, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
    }
    if(strcmp(reponse, "STOP") == 0){
      couleur("31");
      printf("\nLe serveur doit redemarrer, veuillez finir votre partie.\n\n");
      couleur("0");
      if(read(sock,reponse, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
      }
    }
    couleur("35");
    printf("%s\n", reponse);
    couleur("0");
    printf("choix :");
    scanf("%s",envoi);

    if(write(sock, envoi, BUFFER_SIZE) < 0) {
      perror("Erreur de send coté client");
    }
    x+=1;
    if((strcmp(envoi, "q" ) == 0) || (x == 5)){
      if(read(sock,reponse, BUFFER_SIZE) < 0){
        perror("Erreur de lecture sur la socket");
      }
    }
  }while((strcmp(envoi, "q" ) != 0) && (x < 5));
  if(strcmp(envoi, "q" ) == 0){
    printf("vous avez demande la fermeture du jeu.\n");
  }
  couleur("32");
  printf("votre score est de : %s\nfin de la partie!\n", reponse);
  couleur("0");
  return 0;
}

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec le serveur d'adresse 'ip_serveur', sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @require serveur != NULL && ip_serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur){
  serveur->sin_family = AF_INET;
  serveur->sin_port = htons(port_serveur);
  inet_aton(ip_serveur, (struct in_addr*) &serveur->sin_addr);
}

/**
   Etablit une connexion TCP avec le serveur d'adresse 'ip_serveur' sur le port 'port_serveur'.
   Initialise les structures `serveur` et `moi`,
   qui contienne des informations sur l'ip et le port utilisés par les deux points d'accès.
   @param ip_serveur est l'adresse du serveur auquel on se souhaite se connecter
   @param port_serveur est le port sur serveur auquel on se souhaite se connecter
   @param serveur est un pointeur vers la structure représentant les informations du serveur
   @require serveur != NULL && ip_serveur != NULL
   @return la socket conectée au serveur, quitte le programme en cas d'échec.
*/
int initialiser_socket(char * ip_serveur, short port_serveur, struct sockaddr_in * serveur){  
 int sock = 0; // sera la socket de communication UDP

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Erreur de creation de socket coté client");
    return EXIT_FAILURE;
  }
  init_sockaddr_in(serveur, port_serveur, ip_serveur);
  if((connect(sock, (struct sockaddr *)serveur, sizeof(struct sockaddr_in)))<0){
	  perror("Erreur lors de la connexion au serveur");
    return EXIT_FAILURE;
  }
  return sock;

}


/**
   Cette fonction fait appels aux fonctions permettant de jouer au jeu et de fermer correctement le jeu.
   @param sock est la socket qui est utilisée pour communiquer.
   @require sock a été initialisée avec la fonction initialiser_socket.
*/
void echange_avec_serveur(int sock){
  
  char reponse[BUFFER_SIZE]={0};
  char question[BUFFER_SIZE]={0};
  char envoi[BUFFER_SIZE] = {0};
  if((demande(sock, reponse, question)) == 0){
    jeu(sock, reponse, question, envoi);
  }
  else
  {
    close(sock);
    exit(0);
  }
  close(sock);
}

/***********************************************************************/
/**
   Le main permet de lire l'ip et le port sur lequel se connecter
   connecte le client au serveur et fait appel à echange_avec_serveur.
   @param argc le nombre de parametre entré
   @param argv les parametre entré au moment de l'execution
   @require argc == 3
   @return EXIT_SUCCESS
*/
int main(int argc, char** argv) {
    struct sockaddr_in serveur;
    short port_serveur;
    char * ip_serveur;
    int sock; 
    if (argc != 3) {
      fprintf(stderr,"usage: %s host port_serveur\n", argv[0]);
      return EXIT_FAILURE;
    }
    port_serveur = atoi(argv[2]);
    ip_serveur = argv[1];

    
    printf("Connexion au serveur\n");
    sock = initialiser_socket(ip_serveur, port_serveur, &serveur);
    
    echange_avec_serveur(sock);
    close(sock);
    
    return EXIT_SUCCESS;
}

