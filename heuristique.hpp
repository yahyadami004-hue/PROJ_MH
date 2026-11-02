#ifndef HEURISTIQUE_HPP
#define HEURISTIQUE_HPP

#include "graph.hpp"
#include <vector>
#include <limits>
#include <random>
#include <chrono>

struct StatsResolution {
    int conflits_finaux = 0;
    double temps_total_sec = 0.0;
    double temps_meilleur_sec = 0.0;
    long long solutions_evaluees = 0;
    int iterations = 0;

    double evals_par_seconde() const {
        return (temps_total_sec > 1e-9) ? (solutions_evaluees / temps_total_sec) : 0.0;
    }
};

class Heuristique {
public:
    // Travail 1 : Glouton Min-Conflit en ordre naturel avec bris d'égalité aléatoire
    // Complexité : O(V^2 + V*k)
    static StatsResolution gloutonMinConflit(graph& G, std::mt19937& rng) {
        StatsResolution stats;
        auto t_debut = std::chrono::high_resolution_clock::now();

        int V = G.get_nb_sommet();
        int k = G.get_nb_couleur();

        G.sansCouleur();
        if (k <= 0 || V <= 0) return stats;

        std::vector<int> meilleures_couleurs;
        meilleures_couleurs.reserve(k);

        for (int u = 0; u < V; ++u) {
            int min_conflits = std::numeric_limits<int>::max();
            meilleures_couleurs.clear();

            // Évaluation en O(1) des k couleurs via nbConflits[u][c]
            for (int c = 0; c < k; ++c) {
                stats.solutions_evaluees++;
                int conflits_c = G.get_nb_conflits(u, c);

                if (conflits_c < min_conflits) {
                    min_conflits = conflits_c;
                    meilleures_couleurs.clear();
                    meilleures_couleurs.push_back(c);
                } else if (conflits_c == min_conflits) {
                    meilleures_couleurs.push_back(c);
                }
            }

            // Bris d'égalité aléatoire parmi les couleurs optimales pour le sommet u
            std::uniform_int_distribution<int> dist(0, static_cast<int>(meilleures_couleurs.size()) - 1);
            int couleur_choisie = meilleures_couleurs[dist(rng)];

            // Mise à jour incrémentale en O(V)
            G.colorerSommet(u, couleur_choisie);
            stats.iterations++;
        }

        auto t_fin = std::chrono::high_resolution_clock::now();
        stats.temps_total_sec = std::chrono::duration<double>(t_fin - t_debut).count();
        stats.temps_meilleur_sec = stats.temps_total_sec;
        stats.conflits_finaux = G.get_cout_total();

        return stats;
    }
};

#endif