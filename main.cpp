#include <iostream>
#include <string>
#include "graph.hpp"

int main() {
    std::cout << "=== DEMARRAGE DU BANC D'ESSAI ===" << std::endl;
    
    // Chemin vers ton fichier. 
    // Assure-toi que le dossier 'instances' est dans le même répertoire que ton exécutable.
    std::string chemin_instance = "instances/test.txt"; 
    
    std::cout << "Tentative de lecture du fichier : " << chemin_instance << std::endl;
    
    // L'instanciation de l'objet déclenche le parsing défini dans graph.cpp
    graph mon_graphe(chemin_instance);
    
    std::cout << "=== FIN DU TEST ===" << std::endl;
    
    return 0;
}