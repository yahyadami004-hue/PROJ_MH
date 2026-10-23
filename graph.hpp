#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream> 
#include <sstream>   
#include <string>

#include <vector>
#include <iostream>
#include <cstdlib>

class graph{
    private :
        int nb_sommet;
        int nb_arete;                      // nombre d'aretes reellement ajoutees (sans doublons)
        std::vector<std::vector<int>> matrice;
        int nb_couleur; //k (valeur fixe du nombre max de couleurs assignable)
        std::vector<int> coloration;       //liste des couleurs de chaque sommet (-1 = pas de couleur)
        std::vector<std::vector<int>> nbConflits; //nbConflits[u][c] = nb de voisins de u avec la couleur c
        int cout_total = 0; //nombre total d'aretes en conflit (deux extremites de meme couleur)
    public : 

        graph(const std::string& chemin_fichier) :
            nb_sommet(0),
            nb_arete(0),
            nb_couleur(0) {
            chargerFichier(chemin_fichier);
        } // Constructeur qui prend le chemin du fichier et lance le parsing directement      

        // Graphe vide a V sommets (les aretes sont ajoutees avec ajouterArete)
        graph(int V) : 
            nb_sommet(V), 
            nb_arete(0), 
            matrice(V, std::vector<int>(V, 0)),
            nb_couleur(0), 
            coloration(V, -1) {
            init_conflit();
        }

        void init_conflit();                 // recalcule nbConflits et cout_total depuis zero (O(n^2))
        void set_nb_couleur(int k);          // fixe k, remet tous les sommets sans couleur
        void ajouterArete(int u, int v);
        void colorerSommet(int u, int col);  // col dans [-1, k-1], met a jour nbConflits/cout_total en O(n)
        void sansCouleur(); //assigne la couleur -1 à tous les sommets.
        void toutCouleur(); //assigne la couleur 0 à tous les sommets (necessite k >= 1)
        void chargerFichier(const std::string& chemin_fichier);

        int get_nb_sommet() const{ 
                return(nb_sommet);
        }
        int get_nb_arete() const{ 
                return(nb_arete);
        }
        int get_nb_couleur() const{ 
                return(nb_couleur);
        }
        int get_cout_total() const{
                return(cout_total);
        }
        int get_nb_conflits(int u, int c) const{
                return nbConflits.at(u).at(c);
        }

        int get_valeur_matrice(int u, int v) const {
            return matrice.at(u).at(v);
        }

        int get_couleur_sommet(int u) const {
            return coloration.at(u);
        }

        bool is_conflict(int u, int v) const{
            return(matrice.at(u).at(v) == 1 && coloration.at(u) == coloration.at(v) && coloration.at(u) != -1);
        }
};


#endif
