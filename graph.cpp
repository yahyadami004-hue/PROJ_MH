#include "graph.hpp"

void graph::ajouterArete(int u, int v) {
    if (u < -1 || u >= nb_sommet){
        std::cerr << "Erreur ajout arête : le sommet u n'existe pas\n";
        exit(1);
    }
    if (v < -1 || v >= nb_sommet){
        std::cerr << "Erreur ajout arête : le sommet u n'existe pas\n";
        exit(1);
    }
    matrice[u][v] = 1;
    matrice[v][u] = 1;
}

void graph::colorerSommet(int u, int col){
    if (u < -1 || u >= nb_sommet){
        std::cerr << "Erreur coloration : le sommet u n'existe pas\n";
        exit(1);
    }
    if (col > nb_couleur){
        std::cerr << "Erreur : essaye d'assigner une couleur trop grande\n";
        exit(1);
    }
    else if (col == nb_couleur){
        nb_couleur += 1;
    }
    coloration[u] = col;
}



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

        if (type_ligne == 'c') {// commentaires
            continue;
        } 
        else if (type_ligne == 'p') { //en-tête : nb de sommets, nb arêtes
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
            ajouterArete(u-1,v-1);
        }
    }
    fichier.close();
    std::cout << "Succes : Graphe charge avec " << nb_sommet << " sommets et " << nb_arete << " aretes.\n";
}