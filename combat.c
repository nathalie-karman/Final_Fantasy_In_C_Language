/**
 * \file      combat.c
 * \author    Nathalie Karman, Alexandre Papot
 * \version   1.0
 * \date     Mars 2019
 * \brief     Module contenant les fonctions principales qui s'imbriquent pour qu'un combat ait lieu
 * \details    Module contenant les fonctions principales qui s'imbriquent pour qu'un combat ait lieu
 */

 #include <combat.h>

/**
 * \fn int combat_on(void)
 * \brief Fonction qui est responsable pour tout un combat et qui gère tous ses participants
 * \details pas besoin de paramètre puisque l'inventaire et le-s personnage-s sont en global
 * \return l'etat du combat (voir macros définies dans \a perso_commun.h)
*/
/*fonction principale du combat qui gère tous les participants et leurs attributs/etats*/
int combat_on(){
   /* ind_recomp=0; responsable pour les loots générés lors du combat*/
   int choix_j=1; /*variable qui récupère l option choisie par le joueur lorsqu'il est tombé en combat */
   int xp_temp=0; /*les xp qui sont récupérés lors du combat si le joueur a gagné*/
   int retour_menu=1;  /*retour_menu sert à revenir au début du menu si le joueur a mal fait son choix*/

   /*définition des participants du combat*/
   int monster_number = entier_aleatoire(1,MAX_NB_MONSTRE); /*nb de monstre qui fera partie du combat*/
   character_t * monster[monster_number];   /*tableau de monstre généré*/
   /*pour générer les loots*/
   int nb_recompenses= entier_aleatoire(1,monster_number);  /*tableau qui correspond aux niveaux des monstres battus*/
   int tab_niv_recomp[nb_recompenses];

   int i;
   for( i = 0; i < monster_number;i++){
      monster[i] = NULL;
      monster[i] = monster_creation();
   }

   clear_screen();
   printf("Vous êtes piégé !\n\n");
   sleep(1);

   /*début des actions qui s'enchaîneront*/
   do{

      for( i = 0; i < monster_number;i++){
        printf("\tAdversaire %d : %s (vie: %d/%d ; niveau : %d)\n",i+1, monster[i]->name,monster[i]->health, monster[i]->max_health, monster[i]->level);
      }
      printf("\n\tJoueur: %s (vie: %d/%d ; niveau : %d)\n\n",Personnage->name,Personnage->health, Personnage->max_health, Personnage->level);
      choix_j= affich_choix();
      retour_menu= tour_joueur(choix_j,monster,monster_number);



      if(retour_menu != 0 && retour_menu != END_OF_GAME ){


         for( i = 0; i < monster_number; i++){

            if(is_dead (monster[i]) ){
               sleep(1);
               printf("\t%s meurt...\n\n", monster[i]->name);
               sleep(1);
               int check_xp= xp_points(*monster[i]);

               /*ajout des points d experience au joueur = effectif QUE si on gagne le combat*/
               xp_temp += check_xp;
               update_tab_monster(monster ,i , monster_number);
               monster_number--;
            }
         }
          /*à changer pour faire en sorte que il y ait une fonction qui fasse jouer le monstre*/
         for( i = 0; i < monster_number;i++)
            attack(monster[i], &Personnage);
      }
      sleep(1);

   }while( retour_menu != END_OF_GAME && retour_menu != FUITE && monster_number >0 && !is_dead(Personnage) );

   if(!monster_number){

      Personnage->xp += xp_temp;
      printf("Vous avez gagné %d d'XP...\nVos xp : %d\n",xp_temp,Personnage->xp);
      for(i = 0 ; i < nb_recompenses ; i++){

          object_t *  new= create_loot(tab_niv_recomp[i]);
          do{
            printf("Vous obtenez %s \nVoulez vous garder cet item?\n1 - Oui\n2- Non\nVotre choix : " , new->name_object);
            scanf("%d", &choix_j);
            viderBuffer();
          }while(choix_j != 1 || choix_j != 2);
      

      }
      if(levelling(Personnage)){
         sleep(1);
         clear_screen();
         printf("!! Vous avez atteint le niveau %d !!\n",Personnage->level);
      }
      sleep(5);
      printf("On continue notre chemin...");
      sleep(2);
   }else if(is_dead(Personnage)){
      printf("GAME OVER...\n");
      sleep(2);
      etat_jeu=END_OF_GAME;
   }

   for(i =0; i <monster_number ; i++){

     delete_player(&monster[i]);

   }

   return etat_jeu;

}
/**
 * \fn static int choisir_monstre(character_t* tab_adv[], int nb_monstre)
 * \brief Fonction qui demande au joueur de choisir un monstre dans le tableau
 * \param[in] tab_adv[], tableau de monstre-s
 * \param[in] nb_monstre, quantités de monstres encore présents dans le combat
 * \return le choix (l'indice) de l'utilisateur
*/
static int choisir_monstre(character_t* tab_adv[], int nb_monstre){
     int choix_j;
     int i;
     do{
        printf("Quel adversaire choisissez-vous? [0 pour retourner au menu précédent]\n");
        for( i = 0; i < nb_monstre; i++){
           printf("\tAdversaire %d : %s (vie: %d/%d)\n",i+1, tab_adv[i]->name,tab_adv[i]->health, tab_adv[i]->max_health);

        }
        printf("Votre choix: " );
        scanf("%d",&choix_j);
        viderBuffer();
     }while (choix_j > nb_monstre || choix_j< 0);
     return choix_j;

}
/**
 * \fn static void maj_mana(character_t* perso, int val_sort)
 * \brief Fonction qui met à jour le points de magie du joueur
 * \param[in] perso, le personnage qui vient d'appliquer un sort
 * \param[in] val_sort, la valeur du sort en points de magie
*/
static void maj_mana(character_t* perso, int val_sort){
    perso->mana -= val_sort;
}
/**
 * \fn void apply_spell(character_t* attacker,character_t **target, int sort_choisi)
 * \brief Fonction qui applique un sort à un personnage spécifique
 * \param[in] attacker, le personnage qui attaque
 * \param[in] target, le personnage qui reçoit un coup
 * \param[in] sort_choisi, l'indice du sort choisi par celui qui attaque
*/
void apply_spell(character_t* attacker,character_t **target, int sort_choisi){

    printf("\n\n\t<> %s <> ==============> %s ...\n",tab_sort[sort_choisi].nom_sort,(*target)->name);
    sleep(1);

    if(tab_sort[sort_choisi].type_sort == modifie_etat){
      apply_state_modifier(&(*target),tab_sort[sort_choisi].valeur_sort);

    }else{

        int degat = (attacker->stat_intelligence) * tab_sort[sort_choisi].valeur_sort;

        ((*target)->health) -= degat + (*target)->accessory ;

        if(is_dead(*target)){
           ((*target)->health)=0;
        }
        printf("\t%d de dégats causés à %s (%d/%d)\n\n", degat,(*target)->name,(*target)->health, (*target)->max_health );
    }
    maj_mana(attacker, tab_sort[sort_choisi].valeur_sort);  /*mise à jour des points de magie du perso*/
}
/**
 * \fn int tour_joueur(int choix_j, character_t* tab_monstre[], int nb_monstre)
 * \brief Fonction qui traite les choix de l'utilisateur et le retour au menu de combat
 * \param[in] choix_j, le choix saisi préalablement, déclanche une des actions de la fonction \a tour_joueur
 * \param[in] tab_monstre[], le tableau avec le-s monstre-s
 * \param[in] nb_monstre, la quantité d'adversaire
 * \return 0 pour retour au menu de combat ou autre valeur (indique qu'une action s'est produite)
*/
int tour_joueur(int choix_j, character_t* tab_monstre[], int nb_monstre){

   int retour_menu=1;

   switch (choix_j){
      case 1: {
                retour_menu = choisir_monstre(tab_monstre,nb_monstre); /*il n'y a qu'une attaque donc on choisit une cible, si on veut*/
                if(retour_menu != 0)
                  attack(Personnage,&tab_monstre[retour_menu-1]);
              }; break;
      case 2: retour_menu = taking_potion(); break;
      case 3: {
                int sort_choisi;
                sort_choisi = joueur_sort(Personnage);  /*renvoie -1 si le personnage n a pas de sort ou a mal fait son choix */

                if(sort_choisi > -1){

                     if(tab_sort[sort_choisi].type_sort != defensif){

                        retour_menu = choisir_monstre(tab_monstre,nb_monstre);
                        apply_spell(Personnage,&tab_monstre[retour_menu-1], sort_choisi);

                    }else
                        apply_auto_spell(Personnage);

                }else if(sort_choisi == -2){
                   printf("Ce sort n'est pas dans la liste.\n" );
                   sleep(1);
                   retour_menu = 0;  /*on retourne au menu puisqu il a pas de sort*/
               }



              };break;
      case 4: retour_menu = running_away();break; /*renvoie FUITE (-10 )si on s echappe*/
      case 5: retour_menu = etat_jeu =END_OF_GAME;
   }
   if(retour_menu == 0){
        printf("Retour au menu précédent...\n");
        sleep(3);
        clear_screen();
   }
   return retour_menu;

}
/**
 * \fn int affich_choix()
 * \brief Fonction qui affiche le menu de combat
 * \return le choix du joueur => un entier de 1 à 5
*/
int affich_choix(){

         int player_choice = 0;

         do{

              sleep(1);
              printf("Choisir entre les actions ci-dessous:\n\t1 - Attaquer l'adversaire\n\t2 - Prendre une potion\n\t3 - Appliquer un sort\n\t4 - S'évader\n\t5 - Quitter\nVotre choix : ");
              scanf("%d",&player_choice);
              viderBuffer();
         }while(player_choice > 5 || player_choice <1);

         return player_choice;
}
/**
 * \fn void update_tab_monster(character_t *monster_array[],int index, int nb_monstre)
 * \brief Fonction qui met à jour le tableau d'adversaire lorsque l'un d'eux est mort
 * \param[in] monster_array[], tableau des adversaires
 * \param[in] index, monstre mort
 * \param[in] nb_monstre, quantité d'adversaires
*/
/*si un monstre est mort pendant le combat, on va devoir faire en sorte qu il soit supprimé du tableau*/
void update_tab_monster(character_t *monster_array[],int index, int nb_monstre){

    int i;
    delete_player(&monster_array[index]);
    for( i = index; i <  nb_monstre-1 ; i++){
      monster_array[i] = monster_array[i+1];
    }

}
/**
 * \fn void attack(character_t* attacker,character_t **target)
 * \brief Fonction qui permet à un personnage d'attaquer un autre personnage
 * \param[in] attacker, celui qui attaque
 * \param[in] target, la cible
*/
void attack(character_t* attacker,character_t **target){

   printf("\n\t%s ATTAQUE ==============> %s ...\n",attacker->name,(*target)->name);
   sleep(2);

   int degat = (attacker->stat_strength) * (attacker->char_weapon->value_object);

   ((*target)->health) -= degat + (*target)->accessory ;

   if(is_dead(*target)){
      ((*target)->health)=0;
   }
   printf("\t%d de dégats causés à %s (%d/%d)\n\n", degat,(*target)->name,(*target)->health, (*target)->max_health );

}
/**
 * \fn int is_dead(character_t *target)
 * \brief Fonction qui permet de voir si un personnage est mort
 * \param[in] target, la cible
 * \return 0 si il est encore en vie
*/
int is_dead(character_t *target){ /* rip :(*/

  return (target->health <= 0); /*retourne 0 = il est vivant*/

}
/**
 * \fn int levelling(character_t* player)
 * \brief Fonction qui regarde si le joueur monte de niveau
 * \param[in] player, le personnage (version finale => 4 personnages)
 * \return 1 si il a atteint un autre niveau, sinon 0
*/
int levelling(character_t* player){

   int changement_niv = 50;

   if( player->xp >= (changement_niv * player->level) ){
      player->level++;
      return 1;
   }
   return 0;
}
/**
 * \fn int xp_points(character_t monster)
 * \brief Fonction qui calcule le nombre de points d'expérience obtenus lors du combat
 * \param[in] monster, pour se servir de ses statistiques
 * \return une valeur entière qui correspond aux points d'expérience
*/
int xp_points(character_t monster){

      int xp_par_niveau = 10; /*variable qui peut definir l evolution du perso dans tout le jeu ou monstre.stat_strength*/
      return (xp_par_niveau * monster.level);

}
/**
 * \fn void fight_rand(void)
 * \brief Fonction qui se sert du pseudo-aléatoire et de la map_menace pour déclancher un combat
*/
void fight_rand(){

   int trap=0;
   if (Personnage->accessory != evite_combats){
      trap= entier_aleatoire(1,100);
   }else
      trap= entier_aleatoire(51,100);

   int chances= map_threat[position_x][position_y];

   if(trap <= chances){
      etat_jeu = EN_COMBAT;
      combat_on();
   }

}

/**
 * \fn void apply_state_modifier(character_t ** target, int indice)
 * \brief Fonction qui change un des etats du personnage entré en paramètre
 * \param[in] target est le personnage qui aura son etat affecté (dans les deux sens)
 * \param[in] indice est l'entier qui permet d'acceder à l'état Stunt (0) par exemple
 * \details  est la valeur symbolique (voir enuération dans \a inventaire.h) qui correspond à l'indice du dictionnaire de l'état qu on veut affecter
 *      on mettra soit à VRAI si il l'état est FAUX , soit l'inverse
*/
/*marche inversement aussi, si on met un Antidote on va juste inverser l etat*/
void apply_state_modifier(character_t ** target, int indice){

    (*target)->state[indice] = ( (*target)->state[indice] == FAUX ? VRAI : FAUX ) ;

}

/*-------------------------------------------------------------*/
/**
 * \fn int running_away(void)
 * \brief Fonction qui altère l'état du jeu en essayant de s'enfuir (pseudo-aléatoire)
 * \details on se sert de la variable globale \a EVASION qui est définie dans le fichier \a perso_commun.h
 * \return HORS_COMBAT ou alors renvoie soit 1, soit la macro FUITE
*/
int running_away(){ /* true => successful*/
  /* 15% chance to flee */
  int chance=entier_aleatoire(1,100);

  if(  chance < EVASION ){
    printf("Vous arrivez à vous échapper...(-10 xp)\n" );
    Personnage->xp-=10;
    etat_jeu=HORS_COMBAT;

  }else
      printf("\n\nSorry, you have to fight...\n" );

   return (chance < EVASION ? FUITE : 1);
}
