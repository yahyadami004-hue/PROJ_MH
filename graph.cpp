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
    if (u == v){
        std::cerr << "Erreur ajout arête : boucle interdite (sommet " << u << ")\n";
        exit(1);
    }
    if (matrice[u][v] == 1) return; // doublon : on ne recompte pas

    matrice[u][v] = 1;
    matrice[v][u] = 1;
    nb_arete++;

    // Maintien de nbConflits / cout_total si des sommets sont deja colores
    int cu = coloration[u];
    int cv = coloration[v];
    if (cv != -1) nbConflits[u][cv]++;
    if (cu != -1) nbConflits[v][cu]++;
    if (cu != -1 && cu == cv) cout_total++;
}

void graph::set_nb_couleur(int k){
    if (k < 0){
        std::cerr << "Erreur : nombre de couleurs negatif (" << k << ")\n";
        exit(1);
    }
    nb_couleur = k;
    sansCouleur(); // les anciennes couleurs peuvent etre >= k : on repart de zero
}

void graph::colorerSommet(int u, int col){
    if (u < 0 || u >= nb_sommet){
        std::cerr << "Erreur coloration : le sommet u n'existe pas\n";
        exit(1);
    }
    if (col >= nb_couleur){
        std::cerr << "Erreur : essaye d'assigner la couleur " << col
                  << " trop grande (k = " << nb_couleur << ")\n";
        exit(1);
    }
    if (col < -1){
        std::cerr << "Erreur : essaye d'assigner la couleur " << col << " trop petite\n";
        exit(1);
    }

    int ancienne = coloration[u];
    if (ancienne == col) return;

    // Mise a jour incrementale (parcours de la ligne de la matrice : O(n))
    for (int w = 0; w < nb_sommet; w++) {
        if (matrice[u][w] != 1) continue;
        if (ancienne != -1) nbConflits[w][ancienne]--;
        if (col != -1)      nbConflits[w][col]++;
    }
    if (ancienne != -1) cout_total -= nbConflits[u][ancienne];
    if (col != -1)      cout_total += nbConflits[u][col];

    coloration[u] = col;
}

void graph::sansCouleur(){
    for (int i = 0; i < get_nb_sommet(); i++){
        coloration[i] = -1;
    }
    init_conflit();
}

void graph::toutCouleur(){
    if (nb_couleur < 1){
        std::cerr << "Erreur : toutCouleur() necessite au moins 1 couleur\n";
        exit(1);
    }
    for (int i = 0; i < get_nb_sommet(); i++){
        coloration[i] = 0;
    }
    init_conflit();
}

void graph::init_conflit(){
    nbConflits.assign(nb_sommet, std::vector<int>(nb_couleur, 0));
    cout_total = 0;

    for (int u = 0; u < nb_sommet; u++) {
        if (coloration[u] >= nb_couleur) {
            std::cerr << "Erreur init_conflit : couleur " << coloration[u]
                      << " >= k (" << nb_couleur << ")\n";
            exit(1);
        }
    }

    for (int u = 0; u < nb_sommet; u++)
        for (int v = u+1; v < nb_sommet; v++) {
            if (matrice[u][v] != 1) continue;
            int cu = coloration[u];
            int cv = coloration[v];
            if (cv != -1) nbConflits[u][cv]++;
            if (cu != -1) nbConflits[v][cu]++;
            if (cu != -1 && cu == cv) cout_total++;
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
    cout_total = 0;
    matrice.clear();
    coloration.clear();
    nbConflits.clear();

    bool en_tete_lue = false;
    int aretes_en_tete = 0;
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
            nb_arete = 0;               // compte reel, incremente par ajouterArete
            aretes_en_tete = aretes;
            
            // ALLOCATION MEMOIRE
            matrice.assign(nb_sommet, std::vector<int>(nb_sommet, 0));
            coloration.assign(nb_sommet, -1);
            init_conflit();             // nbConflits aux bonnes dimensions (k = 0 pour l'instant)
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
    if (nb_arete != aretes_en_tete) {
        std::cerr << "Attention : l'en-tete annonce " << aretes_en_tete
                  << " aretes mais " << nb_arete << " aretes distinctes ont ete lues\n";
    }
    sansCouleur(); // remet aussi nbConflits / cout_total a jour (une seule fois)
    std::cout << "Succes : Graphe charge avec " << nb_sommet << " sommets et "
              << nb_arete << " aretes et " << nb_couleur << " couleurs\n";
}
