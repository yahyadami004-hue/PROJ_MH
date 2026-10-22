#include <iostream>
#include <string>
#include "graph.hpp"

int main() {
    std::cout << "=== DEMARRAGE DU BANC D'ESSAI ===" << std::endl;
    
    // Chemin vers ton fichier. 
    // Assure-toi que le dossier 'instances' est dans le même répertoire que ton exécutable.
    std::string chemin_instance = "instances/dsjc250.5.col.txt"; 
    
    std::cout << "Tentative de lecture du fichier : " << chemin_instance << std::endl;
    
    // L'instanciation de l'objet déclenche le parsing défini dans graph.cpp
    graph G(chemin_instance);
    std::cout << "Nombre de sommets :" << G.get_nb_sommet() << "Nombre d'aretes" << G.get_nb_arete() << "Nombre de couleurs" << G.get_nb_couleur() << std::endl;
    std::cout << "=== FIN DU TEST ===" << std::endl;
    

    const int n = G.get_nb_sommet() < 5 ? G.get_nb_sommet() : 5;

    std::cout << "First " << n << "x" << n << " values of the adjacency matrix:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << G.get_valeur_matrice(i, j) << ' ';
        }
        std::cout << '\n';
    }
    
    std::cout << "Colors of the first " << n << " vertices:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "vertex " << i << ": " << G.get_couleur_sommet(i) << '\n';
    }
        return 0;
}