#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include "graph.hpp"
#include "heuristique.hpp"
#include "descente.hpp"

// Fonction utilitaire : trouve automatiquement le fichier dans "instances/"
// meme si l'utilisateur tape juste "dsjc250.5" sans extension
std::string trouverFichier(const std::string& saisie) {
    std::vector<std::string> candidats = {
        saisie,
        "instances/" + saisie,
        "instances/" + saisie + ".col.txt",
        "instances/" + saisie + ".col",
        "instances/" + saisie + ".txt",
        saisie + ".col.txt",
        saisie + ".col"
    };

    for (const std::string& chemin : candidats) {
        std::ifstream test(chemin);
        if (test.is_open()) {
            return chemin;
        }
    }
    // Si aucun candidat n'est trouve, on retourne la saisie brute
    // (chargerFichier affichera l'erreur standard)
    return "instances/" + saisie;
}

// Fonction qui execute tout ton banc d'essai sur une instance donnee
void lancerTestInstance(const std::string& nom_saisi, int k, int nb_runs) {
    std::cout << "\n=== DEMARRAGE DU BANC D'ESSAI ===" << std::endl;

    std::string chemin_instance = trouverFichier(nom_saisi);
    std::cout << "Tentative de lecture du fichier : " << chemin_instance << std::endl;

    // L'instanciation de l'objet declenche le parsing defini dans graph.cpp
    graph G(chemin_instance);
    G.set_nb_couleur(k);

    std::cout << "Nombre de sommets : " << G.get_nb_sommet()
              << " | Nombre d'aretes : " << G.get_nb_arete()
              << " | Nombre de couleurs : " << G.get_nb_couleur() << std::endl;
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
    std::cout << "Cout total initial : " << G.get_cout_total() << std::endl;

    // =========================================================================
    // TRACE DETAILLEE SUR 1 EXECUTION (HEURISTIQUE + DESCENTE)
    // =========================================================================
    std::mt19937 rng(42);

    std::cout << "\n--- [1] HEURISTIQUE GLOUTONNE MIN-CONFLIT ---\n";
    StatsResolution stats_h1 = Heuristique::gloutonMinConflit(G, rng);
    for (int i = 0; i < n; ++i) {
        std::cout << "vertex " << i << ": " << G.get_couleur_sommet(i) << '\n';
    }
    std::cout << "Cout apres Heuristique : " << G.get_cout_total()
              << " | Temps : " << stats_h1.temps_total_sec << " s\n";

    std::cout << "\n--- [2] DESCENTE BEST IMPROVEMENT (1-MOVE) ---\n";
    int cout_avant = G.get_cout_total();
    StatsResolution stats_d1 = Descente::executerBestImprovement(G, rng);
    for (int i = 0; i < n; ++i) {
        std::cout << "vertex " << i << ": " << G.get_couleur_sommet(i) << '\n';
    }
    std::cout << "Cout apres Descente    : " << G.get_cout_total()
              << " (Gain : -" << (cout_avant - G.get_cout_total()) << " conflits en "
              << stats_d1.iterations << " iterations)\n";

    // =========================================================================
    // TABLEAU RECAPITULATIF MULTI-RUNS (POUR LA SECTION 4 DU RAPPORT)
    // =========================================================================
    int h_min = 1e9, h_max = 0, d_min = 1e9, d_max = 0;
    double h_somme = 0, h_t_tot = 0, h_evals = 0;
    double d_somme = 0, d_t_tot = 0, d_t_best = 0, d_evals = 0;

    for (int r = 0; r < nb_runs; ++r) {
        StatsResolution sh = Heuristique::gloutonMinConflit(G, rng);
        h_min = std::min(h_min, sh.conflits_finaux);
        h_max = std::max(h_max, sh.conflits_finaux);
        h_somme += sh.conflits_finaux;
        h_t_tot += sh.temps_total_sec;
        h_evals += sh.evals_par_seconde();

        StatsResolution sd = Descente::executerBestImprovement(G, rng);
        d_min = std::min(d_min, sd.conflits_finaux);
        d_max = std::max(d_max, sd.conflits_finaux);
        d_somme += sd.conflits_finaux;
        d_t_tot += sh.temps_total_sec + sd.temps_total_sec;
        d_t_best += sh.temps_total_sec + sd.temps_meilleur_sec;
        d_evals += sd.evals_par_seconde();
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n================ TABLEAU DE RESULTATS (" << nb_runs << " RUNS) ================\n";
    std::cout << "Methode      | Conflits (Moy / Min / Max) | T_total (s) | T_best (s) | Evals/s\n";
    std::cout << "----------------------------------------------------------------------------------\n";
    std::cout << "Heuristique  | " << std::setw(6) << (h_somme / nb_runs) << " / "
              << std::setw(4) << h_min << " / " << std::setw(4) << h_max << "       | "
              << std::setprecision(5) << std::setw(11) << (h_t_tot / nb_runs) << " | "
              << std::setw(10) << (h_t_tot / nb_runs) << " | "
              << std::setprecision(0) << (h_evals / nb_runs) << "\n";
    std::cout << std::setprecision(2);
    std::cout << "Descente 1-m | " << std::setw(6) << (d_somme / nb_runs) << " / "
              << std::setw(4) << d_min << " / " << std::setw(4) << d_max << "       | "
              << std::setprecision(5) << std::setw(11) << (d_t_tot / nb_runs) << " | "
              << std::setw(10) << (d_t_best / nb_runs) << " | "
              << std::setprecision(0) << (d_evals / nb_runs) << "\n";
    std::cout << "==================================================================================\n";
}

int main(int argc, char* argv[]) {
    // Mode 1 : Lancement direct en ligne de commande (ex: ./prog dsjc250.5 28)
    if (argc > 1) {
        std::string nom_fichier = argv[1];
        int k = (argc > 2) ? std::stoi(argv[2]) : 20;
        int nb_runs = (argc > 3) ? std::stoi(argv[3]) : 10;
        lancerTestInstance(nom_fichier, k, nb_runs);
        return 0;
    }

    // Mode 2 : Mode interactif (tu tapes juste le nom du fichier dans la console)
    char continuer = 'o';
    while (continuer == 'o' || continuer == 'O') {
        std::string nom_fichier;
        int k = 20;
        int nb_runs = 10;

        std::cout << "\nEntrez le nom du fichier (ex: dsjc250.5 ou flat300_26) : ";
        std::cin >> nom_fichier;

        std::cout << "Entrez le nombre de couleurs k : ";
        std::cin >> k;

        std::cout << "Entrez le nombre de runs pour la moyenne (ex: 10) : ";
        std::cin >> nb_runs;

        lancerTestInstance(nom_fichier, k, nb_runs);

        std::cout << "\nVoulez-vous tester un autre fichier ? (o/n) : ";
        std::cin >> continuer;
    }

    return 0;
}
