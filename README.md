# Readme

Ce jeu est un **Quizz de culture générale**. Vous devez répondre à des questions et votre score augmentera si la réponse est correcte, sinon la prochaine réponse s'affichera sans augmenter votre score. Vous aurez votre score final à la fin de votre partie!

**Compilation et exécution**
Tapez **make** pour obtenir les fichiers exécutables, ces derniers se nommeront **server** et **client** et seront respectivement crées dans les dossiers **server** et **client**.

**Côté serveur :**
Pour lancer le serveur, il faut taper : **./server port** (exemple : ./server 1234).
Ce dernier est **multi-thread**, il peut donc accepter jusqu'à 20 clients en simultané.

**Côté client :**
 Pour lancer le client, il faut taper **./client ip port sur lequel le serveur est connecté** (exemple: ./client 127.0.0.1 1234).

 Le client répondra en **lettre minuscule**, **entrée** pour valider. Entrez un nom d'utilisateur qui vous permettra de sauvegarder votre partie. À votre prochaine connexion,
 entrez votre nom d'utilisateur si vous souhaité reprendre votre partie.
 En pleine partie, vous pouvez quitter en tapant **q**. Votre partie sera sauvegardée et vous pourrez la reprendre comme indiqué précédemment.

 **GANGAT Bilal et SLIMANI Missipssa**
