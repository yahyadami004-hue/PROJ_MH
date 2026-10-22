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
    
    public : 

        graph() : nb_sommet(0), nb_arete(0) {};    // Constructeur par défaut
        graph(const std::string& chemin_fichier) {
            chargerFichier(chemin_fichier);
        } // Constructeur qui prend le chemin du fichier et lance le parsing directement      
        graph(int V, int E) : 
            nb_sommet(V), 
            nb_arete(E), 
            matrice(V, std::vector<int>(V, 0)),
            nb_couleur(0), 
            coloration(V, -1) {}

        void ajouterArete(int u, int v);
        void colorerSommet(int u, int col);
        void chargerFichier(const std::string& chemin_fichier);

        
};


#endif
