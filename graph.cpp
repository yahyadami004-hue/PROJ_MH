#include "graph.hpp"




// Méthode principale de parsing
void graph::chargerFichier(const std::string& chemin_fichier) {
    std::ifstream fichier(chemin_fichier);
    
    // Sécurité : On vérifie si le fichier s'est bien ouvert
    if (!fichier.is_open()) {
        std::cerr << "Erreur CRITIQUE : Impossible de lire " << chemin_fichier << "\n";
        exit(1);
    }

    std::string ligne;
    // On lit le fichier ligne par ligne
    while (std::getline(fichier, ligne)) {
        if (ligne.empty()) continue; // On saute les lignes vides

        std::stringstream ss(ligne);
        char type_ligne;
        ss >> type_ligne;

        if (type_ligne == 'c') {
            // C'est un commentaire. On passe à la ligne suivante.
            continue;
        } 
        else if (type_ligne == 'p') {
            // Ligne d'en-tête DIMACS
            std::string mot_cle_edge;
            ss >> mot_cle_edge >> nb_sommet >> nb_arete;
            
            // ALLOCATION MEMOIRE
            matrice.assign(nb_sommet, std::vector<int>(nb_sommet, 0));
            coloration.assign(nb_sommet, 0);
        } 
        else if (type_ligne == 'e') {
            // Ligne d'arête
            int u, v;
            ss >> u >> v;
            
            // CORRECTION D'INDEXATION (1..N vers 0..N-1)
            matrice[u - 1][v - 1] = 1;
            matrice[v - 1][u - 1] = 1;
        }
    }
    fichier.close();
    std::cout << "Succes : Graphe charge avec " << nb_sommet << " sommets et " << nb_arete << " aretes.\n";
}