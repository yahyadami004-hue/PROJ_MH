#ifndef GRAPH_HPP
#define GRAPH_HPP



#include <fstream>   // Pour lire les fichiers (std::ifstream)
#include <sstream>   // Pour découper les lignes (std::stringstream)
#include <string>    // Pour manipuler les chaînes de caractères (std::string)
#include <vector>
#include <iostream>

class graph{
    private :
        int nb_sommet;
        int nb_arete;
        std::vector<std::vector<int>> matrice;
        std::vector<int> coloration;       

    public:
        graph() : nb_sommet(0), nb_arete(0) {};    // Constructeur par défaut
        graph(const std::string& chemin_fichier) {
            chargerFichier(chemin_fichier);
        } // Constructeur qui prend le chemin du fichier et lance le parsing directement      
        void graph::chargerFichier(const std::string& chemin_fichier);
};


#endif