#include "graph.hpp"



public:
        // Constructeur par défaut
        graph() : nb_sommet(0), nb_arete(0) {}

        // Constructeur qui prend le chemin du fichier et lance le parsing directement
        graph(const std::string& chemin_fichier) {
            chargerFichier(chemin_fichier);
        }

        // Méthode principale de parsing
        void chargerFichier(const std::string& chemin_fichier) {
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

                // stringstream permet de lire la ligne mot par mot très facilement
                std::stringstream ss(ligne);
                char type_ligne;
                ss >> type_ligne;

                if (type_ligne == 'c') {
                    // C'est un commentaire. L'instruction continue force à passer à la ligne suivante.
                    continue;
                } 
                else if (type_ligne == 'p') {
                    // Ligne d'en-tête DIMACS : p edge nb_sommet nb_arete
                    std::string mot_cle_edge;
                    ss >> mot_cle_edge >> nb_sommet >> nb_arete;
                    
                    // ALLOCATION MEMOIRE : On initialise la matrice à la bonne taille avec des 0
                    // On crée "nb_sommet" lignes, et chaque ligne contient "nb_sommet" zéros.
                    matrice.assign(nb_sommet, std::vector<int>(nb_sommet, 0));
                    
                    // On prépare aussi le vecteur de coloration
                    coloration.assign(nb_sommet, 0);
                } 
                else if (type_ligne == 'e') {
                    // Ligne d'arête : e u v
                    int u, v;
                    ss >> u >> v;
                    
                    // /!\ CORRECTION D'INDEXATION /!\
                    // Dans les fichiers DIMACS, les sommets commencent à 1.
                    // En C++, les tableaux commencent à 0. On fait donc -1.
                    // Le graphe est NON orienté, on place donc deux '1' de manière symétrique.
                    matrice[u - 1][v - 1] = 1;
                    matrice[v - 1][u - 1] = 1;
                }
            }
            fichier.close();
            std::cout << "Succes : Graphe charge avec " << nb_sommet << " sommets et " << nb_arete << " aretes.\n";
        }
};

#endif