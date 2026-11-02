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

// Structure pour regrouper un fichier graphe et la liste des k demandes par l'enonce
struct BancTest {
    std::string nom_affiche;
    std::string nom_fichier;
    std::vector<int> valeurs_k;
    int nb_runs;
};

// Cherche automatiquement le fichier dans le dossier "tests/" (ou "instances/")
std::string trouverFichier(const std::string& saisie) {
    std::vector<std::string> base_names = {saisie};
    if (saisie == "flat300_26") {
        base_names.push_back("flat300_26_0");
    }

    std::vector<std::string> prefixes = {"tests/", "instances/", ""};
    std::vector<std::string> suffixes = {"", ".col", ".col.txt", ".txt"};

    for (const auto& b : base_names) {
        for (const auto& p : prefixes) {
            for (const auto& s : suffixes) {
                std::string candidat = p + b + s;
                std::ifstream f(candidat);
                if (f.is_open()) {
                    return candidat;
                }
            }
        }
    }
    return ""; // Fichier introuvable
}

// Structure interne pour stocker les moyennes d'un couple (instance, k)
struct ResultatLigne {
    std::string instance_k;
    double h_moy = 0; int h_min = 0; int h_max = 0;
    double h_t_tot = 0; double h_t_best = 0; double h_evals = 0;

    double d_moy = 0; int d_min = 0; int d_max = 0;
    double d_t_tot = 0; double d_t_best = 0; double d_evals = 0;
};

// Execute nb_runs sur un graphe deja charge en memoire pour un k donne
ResultatLigne evaluerCouple(graph& G, const std::string& nom_affiche, int k, int nb_runs) {
    G.set_nb_couleur(k);
    std::mt19937 rng(42); // Graine fixe pour la reproductibilite

    int h_min = 1e9, h_max = 0;
    double h_somme = 0, h_t_tot = 0, h_t_best = 0, h_evals = 0;

    int d_min = 1e9, d_max = 0;
    double d_somme = 0, d_t_tot = 0, d_t_best = 0, d_evals = 0;

    for (int r = 0; r < nb_runs; ++r) {
        // 1. Travail 1 : Heuristique Gloutonne Min-Conflit
        StatsResolution sh = Heuristique::gloutonMinConflit(G, rng);
        h_min = std::min(h_min, sh.conflits_finaux);
        h_max = std::max(h_max, sh.conflits_finaux);
        h_somme += sh.conflits_finaux;
        h_t_tot += sh.temps_total_sec;
        h_t_best += sh.temps_meilleur_sec;
        h_evals += sh.evals_par_seconde();

        // 2. Travail 2 : Descente Best Improvement (1-move) a partir de la solution gloutonne
        StatsResolution sd = Descente::executerBestImprovement(G, rng);
        d_min = std::min(d_min, sd.conflits_finaux);
        d_max = std::max(d_max, sd.conflits_finaux);
        d_somme += sd.conflits_finaux;
        // Temps cumule (Construction + Descente) pour atteindre l'optimum local
        d_t_tot += (sh.temps_total_sec + sd.temps_total_sec);
        d_t_best += (sh.temps_total_sec + sd.temps_meilleur_sec);
        d_evals += sd.evals_par_seconde();
    }

    ResultatLigne res;
    res.instance_k = nom_affiche + " (k=" + std::to_string(k) + ")";
    res.h_moy = h_somme / nb_runs; res.h_min = h_min; res.h_max = h_max;
    res.h_t_tot = h_t_tot / nb_runs; res.h_t_best = h_t_best / nb_runs; res.h_evals = h_evals / nb_runs;

    res.d_moy = d_somme / nb_runs; res.d_min = d_min; res.d_max = d_max;
    res.d_t_tot = d_t_tot / nb_runs; res.d_t_best = d_t_best / nb_runs; res.d_evals = d_evals / nb_runs;
    return res;
}

// Affiche les deux tableaux finaux conformes a la Section 4 du sujet
void afficherTableauxFinaux(const std::vector<ResultatLigne>& resultats) {
    std::cout << "\n========================================================================================================\n";
    std::cout << "                    TABLEAU 1 : HEURISTIQUE DEDIEE (GLOUTON MIN-CONFLIT)\n";
    std::cout << "========================================================================================================\n";
    std::cout << std::left << std::setw(22) << "Instance (k)"
              << " | " << std::right << std::setw(8) << "Moy"
              << " | " << std::setw(5) << "Min"
              << " | " << std::setw(5) << "Max"
              << " | " << std::setw(12) << "T_total (s)"
              << " | " << std::setw(12) << "T_best (s)"
              << " | " << std::setw(14) << "Evals / sec\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";
    for (const auto& r : resultats) {
        std::cout << std::left << std::setw(22) << r.instance_k
                  << " | " << std::right << std::fixed << std::setprecision(2) << std::setw(8) << r.h_moy
                  << " | " << std::setw(5) << r.h_min
                  << " | " << std::setw(5) << r.h_max
                  << " | " << std::setprecision(5) << std::setw(12) << r.h_t_tot
                  << " | " << std::setw(12) << r.h_t_best
                  << " | " << std::setprecision(0) << std::setw(14) << r.h_evals << "\n";
    }

    std::cout << "\n========================================================================================================\n";
    std::cout << "                    TABLEAU 2 : HEURISTIQUE DE DESCENTE (BEST IMPROVEMENT 1-MOVE)\n";
    std::cout << "========================================================================================================\n";
    std::cout << std::left << std::setw(22) << "Instance (k)"
              << " | " << std::right << std::setw(8) << "Moy"
              << " | " << std::setw(5) << "Min"
              << " | " << std::setw(5) << "Max"
              << " | " << std::setw(12) << "T_total (s)"
              << " | " << std::setw(12) << "T_best (s)"
              << " | " << std::setw(14) << "Evals / sec\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";
    for (const auto& r : resultats) {
        std::cout << std::left << std::setw(22) << r.instance_k
                  << " | " << std::right << std::fixed << std::setprecision(2) << std::setw(8) << r.d_moy
                  << " | " << std::setw(5) << r.d_min
                  << " | " << std::setw(5) << r.d_max
                  << " | " << std::setprecision(5) << std::setw(12) << r.d_t_tot
                  << " | " << std::setw(12) << r.d_t_best
                  << " | " << std::setprecision(0) << std::setw(14) << r.d_evals << "\n";
    }
    std::cout << "========================================================================================================\n";
}

// Usage :
//   ./prog                         -> Lance toute la batterie des 25 instances du sujet
//   ./prog <fichier> <k> [nb_runs] -> Teste une instance specifique (mode soutenance)
int main(int argc, char* argv[]) {
    // Mode test unitaire (pour la soutenance ou un test cible)
    if (argc >= 3) {
        std::string saisie = argv[1];
        int k = std::stoi(argv[2]);
        int nb_runs = (argc >= 4) ? std::stoi(argv[3]) : 10;

        std::string chemin = trouverFichier(saisie);
        if (chemin.empty()) {
            std::cerr << "Erreur : fichier '" << saisie << "' introuvable dans tests/ ou instances/\n";
            return 1;
        }

        graph G(chemin);
        ResultatLigne res = evaluerCouple(G, saisie, k, nb_runs);
        afficherTableauxFinaux({res});
        return 0;
    }

    // Mode Batterie Complete : les 25 couples (instance, k) demandes dans l'enonce
    // On effectue 10 runs pour V <= 1000 et 5 runs pour V >= 2000 afin d'optimiser le temps global
    std::vector<BancTest> batterie = {
        {"flat300_26", "flat300_26_0", {26, 13},           10},
        {"le450_15c",  "le450_15c",    {15, 8},            10},
        {"dsjc125.1",  "dsjc125.1",    {5},                10},
        {"dsjc125.9",  "dsjc125.9",    {44, 22},           10},
        {"dsjc250.1",  "dsjc250.1",    {8},                10},
        {"dsjc250.5",  "dsjc250.5",    {28},               10},
        {"dsjc250.9",  "dsjc250.9",    {72, 36},           10},
        {"dsjc1000.5", "dsjc1000.5",   {86, 85, 84, 42},   10},
        {"dsjc1000.9", "dsjc1000.9",   {224, 223, 222, 111}, 10},
        {"C2000.5",    "C2000.5",      {147, 146, 73},      5},
        {"C4000.5",    "C4000.5",      {261, 260, 130},     5}
    };

    std::vector<ResultatLigne> tous_les_resultats;

    std::cout << "=== LANCEMENT DE LA BATTERIE OFFICIELLE (25 COUPLES INSTANCE/K) ===\n";
    for (const auto& test : batterie) {
        std::string chemin = trouverFichier(test.nom_fichier);
        if (chemin.empty()) {
            std::cerr << "[IGNORE] Fichier manquant dans tests/ : " << test.nom_fichier << ".col\n";
            continue;
        }

        // On charge le graphe UNE SEULE FOIS en memoire pour toutes ses valeurs de k
        graph G(chemin);
        for (int k : test.valeurs_k) {
            std::cout << " -> Calcul en cours : " << test.nom_affiche << " avec k = " << k 
                      << " (" << test.nb_runs << " runs)...\n";
            ResultatLigne res = evaluerCouple(G, test.nom_affiche, k, test.nb_runs);
            tous_les_resultats.push_back(res);
        }
    }

    afficherTableauxFinaux(tous_les_resultats);
    return 0;
}
