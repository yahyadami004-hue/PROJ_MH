#include "graph.hpp"

void graph::ajouterArete(int u, int v) {
    if (u < 0 || u >= nb_sommet){
        std::cerr << "Erreur ajout arête : le sommet u n'existe pas\n";
        exit(1);
    }
    if (v < 0 || v >= nb_sommet){
        std::cerr << "Erreur ajout arête : le sommet v n'existe pas\n";
        exit(1);
    }
    matrice[u][v] = 1;
    matrice[v][u] = 1;
}

void graph::colorerSommet(int u, int col){
    if (u < 0 || u >= nb_sommet){
        std::cerr << "Erreur coloration : le sommet u n'existe pas\n";
        exit(1);
    }
    if (col > nb_couleur){
        std::cerr << "Erreur : essaye d'assigner une couleur" <<col<< "trop grande"<<nb_couleur<<"\n";
        exit(1);
    }
    if (col < -1){
        std::cerr << "Erreur : essaye d'assigner une couleur" <<col<< "trop petite"<<"\n";
        exit(1);
    }
    else if (col == nb_couleur){
        nb_couleur += 1;
    }
    coloration[u] = col;
}

void graph::sansCouleur(){
    nb_couleur = 0;
    for (int i = 0; i < get_nb_sommet(); i++){
        coloration[i] = -1;
    }
}
void graph::toutCouleur(){
    nb_couleur = 1;
    for (int i = 0; i < get_nb_sommet(); i++){
        coloration[i] = 0;
    }
}

void graph::init_conflit(){
    nbConflits.assign(nb_sommet, std::vector<int>(nb_couleur, 0));
 
    for (int u = 0; u< nb_sommet; u++)
        for (int v; v< nb_sommet; v++) {
            if (matrice[u][v] == 1) {
                nbConflits[u][coloration[v]]+=1;
                cout_total+=1;
            }
        }

}



// Méthode principale de parsing
void graph::chargerFichier(const std::string& chemin_fichier) {
    std::ifstream fichier(chemin_fichier);
    
    // Sécurité : On vérifie si le fichier s'est bien ouvert
    if (!fichier.is_open()) {
        std::cerr << "Erreur CRITIQUE : Impossible de lire " << chemin_fichier << "\n";
        exit(1);
    }

    nb_sommet = 0;
    nb_arete = 0;
    nb_couleur = 0;
    matrice.clear();
    coloration.clear();

    bool en_tete_lue = false;
    std::string ligne;
    // On lit le fichier ligne par ligne
    while (std::getline(fichier, ligne)) {
        std::stringstream ss(ligne);
        char type_ligne = '\0';
        if (!(ss >> type_ligne)) continue;

        if (type_ligne == 'c') {// commentaires
            continue;
        } 
        else if (type_ligne == 'p') { //en-tête : nb de sommets, nb arêtes
            std::string mot_cle_edge;
            int sommets;
            int aretes;
            if (en_tete_lue || !(ss >> mot_cle_edge >> sommets >> aretes) ||
                mot_cle_edge != "edge" || sommets < 0 || aretes < 0) {
                std::cerr << "Erreur : en-tete de graphe invalide\n";
                exit(1);
            }
            nb_sommet = sommets;
            nb_arete = aretes;
            
            // ALLOCATION MEMOIRE
            matrice.assign(nb_sommet, std::vector<int>(nb_sommet, 0));
            coloration.assign(nb_sommet, -1);
            en_tete_lue = true;
        } 
        else if (type_ligne == 'e') {
            // Ligne d'arête
            int u;
            int v;
            if (!en_tete_lue || !(ss >> u >> v)) {
                std::cerr << "Erreur : ligne d'arête invalide\n";
                exit(1);
            }
            if (u < 1 || u > nb_sommet || v < 1 || v > nb_sommet) {
                std::cerr << "Erreur : sommet d'une arête hors limites\n";
                exit(1);
            }
            ajouterArete(u-1,v-1);
        }
    }
    fichier.close();
    if (!en_tete_lue) {
        std::cerr << "Erreur : en-tete de graphe introuvable\n";
        exit(1);
    }
    sansCouleur();
    std::cout << "Succes : Graphe charge avec " << nb_sommet << " sommets et "
              << nb_arete << " aretes et " << nb_couleur << " couleurs\n";
}