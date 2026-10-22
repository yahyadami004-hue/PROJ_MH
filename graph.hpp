#ifndef GRAPH_HPP
#define GRAPH_HPP



#include <fstream>   // Pour lire les fichiers (std::ifstream)
#include <sstream>   // Pour découper les lignes (std::stringstream)
#include <string>    // Pour manipuler les chaînes de caractères (std::string)
#include <vector>
#include <iostream>
#include <cstdlib>

class graph{
    private :
        int nb_sommet;
        int nb_arete;
        std::vector<std::vector<int>> matrice;
        std::vector<int> coloration;       

};


#endif

