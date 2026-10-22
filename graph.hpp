#ifndef GRAPH_HPP
#define GRAPH_HPP



#include <vector>
#include <iostream>

class graph{
    private :
        int nb_sommet;
        int nb_arete;
        std::vector<std::vector<int>> matrice;
        std::vector<int> coloration;       

};


#endif