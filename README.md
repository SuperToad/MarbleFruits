# MarbleFruits


Duffaut Julien       julien.duffaut@etu.univ-amu.fr
Pastor Jean-Baptiste jb.pastor@outlook.fr


MarbleFuits 2 : Fruits' Revenge


	Pour la compilation et l'exécution : 

Ouvrir un terminal à l'emplacement du projet tapez la commande "make all" pour la compilation.
Pour l'exécution toujours dans le termianl tapez " ./Marbles "
Il est possible d'utiliser la commande "make clean" pour supprimer les fichiers .o ainsi que l'exécutable.


	Utilisation du programme : 

Une fois le programme exécuté la fenêtre s'ouvre avec seulement le canon. 
Pour lancer le jeu cliquez sur le premier menu déroulant "Game" puis cliquez sur l'élément du menu "Start".

Si vous souhaitez éditer un niveau alors dans ce cas cliquez sur le menu "Level" puis "Edit". A la fin de l'édition de votre niveau n'oubliez pas de l'enregistrer.
Pour ce faire retournez sur le menu "Game" puis cliquez sur "Save".

Sur notre jeu l'image de fond est personnalisable pour ce faire, il faut cliquer sur "Game" puis "Load BG" et là vous parcourez vos fichiers afin de choisir le fond que vous souhaitez.

Il est possible de rajouter un nouveau niveau au jeu en utilisant "New" dans le sous-menu "Level". N'oubliez pas de sauvegarder le niveau créée.


	Ce que nous avons fait pas rapport à l'énoncé : 

L'énoncé étant très libre nous avons souhaité ajouter une touche colorée à notre jeu c'est pour cela que nous avons remplacé les billes par des fruits.

Certaines des consignes n'ont pas pues être respectées :
- Certains effets de combos n'ont pas été réalisés (Retours en arrières des marbles)
- Les meilleurs scores ne sont pas sauvegardés et ne sont pas affichés en fin de partie
- Les couleurs du canon ne changent pas selon les couleurs présentes sur les tracks
- Le canon est statique, sa position ne peut être modifiée dans le menu d'édition
- Le menu d'édition de permet pas de changer le nombre de marbles ni leur vitesse


	Les parties intéressantes du programme :

Pour nous la partie sur l'utilisation des memmove a été la plus intéressante car on ne connaissait pas l'utilisation de cette méthode et cela nous a permis d'avoir un moyen de manipuler la mémoire.
