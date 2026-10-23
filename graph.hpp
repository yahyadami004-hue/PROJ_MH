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
        int nb_arete;
        std::vector<std::vector<int>> matrice;
        int nb_couleur;
        std::vector<int> coloration;       //liste des couleurs de chaque sommet (-1 = pas de couleur)
        std::vector<std::vector<int>> nbConflits; //nbConflits[u][c] nb de voisins de v avec la couleur c
    public : 

        graph(const std::string& chemin_fichier) :
            nb_sommet(0),
            nb_arete(0),
            nb_couleur(0) {
            chargerFichier(chemin_fichier);
        } // Constructeur qui prend le chemin du fichier et lance le parsing directement      
        graph(int V, int E) : 
            nb_sommet(V), 
            nb_arete(E), 
            matrice(V, std::vector<int>(V, 0)),
            nb_couleur(0), 
            coloration(V, -1) {}
        void init_conflit();
        void ajouterArete(int u, int v);
        void colorerSommet(int u, int col);
        void sansCouleur(); //assigne la couleur -1 à tous les sommets.
        void toutCouleur();
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
