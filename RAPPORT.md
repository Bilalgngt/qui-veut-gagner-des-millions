**GANGAT Bilal et SLIMANI Missipssa**


# Rapport du projet de système et réseaux

Ce projet est du type **client/serveur en C** et le concept échange choisi est un **jeu question/réponse de culture générale**.
La communication est assurée via les **sockets**, selon le protocole **TCP** car une communication entre le client et le serveur est nécessaire.

**But du jeu**
Le serveur pose des questions au client, ce dernier lui répond. Le score est incrémenté si la réponse est correcte sinon on passe à la question suivante sans incrémentation. Le client peut décider de rompre la partie à n'importe quel moment du jeu, son score ainsi que le numéro de la question en cours, est enregistrés et sauvegardé dans un fichier **save.txt**, à sa prochaine connexion en entrant son nom, le client peut reprendre sa partie la où il s'est arrêté précédemment.

 **Graceful shutdown**

Le client et le serveur sont **multi-tâches**. En effet, **20 clients** peuvent se connecter en simultané, une fois que le client termine le jeu, ce dernier termine mais le serveur continue. En effet, il est toujours possible aux autres clients de se connecter et d'y jouer. Un système de **graceful shutdown (signaux, sauvegarde et chargement)** a été mis en place pour permettre aux clients ainsi qu'au serveur de se fermer correctement, **fermer les sockets**, **fermer le fichier de sauvegarde** et **fermer les threads** comme il se doivent et libérer correctement **les mémoires allouées**.
Quand le serveur reçoit un **CTRL+C (SIGINT)** depuis le terminal, ce dernier n'accepte plus de nouveaux clients et envoie un message aux clients déjà connecter, leurs demandant de finir leur partie pour qu'il puisse s'arrêter. Une fois que les clients terminent leur partie, ces derniers se ferment et ainsi, le serveur se ferme à son tour.


# Problèmes rencontrés
  1. **La sauvegarde et le chargement :**
  - Le défi était de réussir à sauvegarder le nom d'utilisateur, la question et le score au moment de la sortie du jeu en pleine partie et de réussir à chargé les bonnes données depuis le fichier save.txt pour pouvoir reprendre la partie.

2. **Graceful shutdown :**
  - Le serveur ne se fermait pas correctement quand il recevait un signal **SIGINT** depuis le terminal car les threads déjà lancés attendaient encore les réponses des clients.
  - Réussir à fermer le serveur après avoir arrêter d'accepter de nouveaux clients et après avoir attendu la fin des clients déjà connectés.

3. **TCP**
  - Problèmes liés à la gestion des write et des read du menu, des questions et à la détection du message envoyé par le serveur demandant aux clients de finir leur partie en cas d'arrêt du  serveur.

# Merci pour l'intérêt que vous avez porté à notre projet.
