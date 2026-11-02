#ifndef DESCENTE_HPP
#define DESCENTE_HPP

#include "heuristique.hpp"
#include <utility>

class Descente {
public:
    // Travail 2 : Descente Best Improvement sur le voisinage 1-move restreint aux sommets en conflit
    // Complexité par palier : O(V * k) | Complexité pire cas : O(C_0 * V * k)
    static StatsResolution executerBestImprovement(graph& G, std::mt19937& rng) {
        StatsResolution stats;
        auto t_debut = std::chrono::high_resolution_clock::now();

        int V = G.get_nb_sommet();
        int k = G.get_nb_couleur();

        std::vector<std::pair<int, int>> meilleurs_mouvements;

        while (G.get_cout_total() > 0) {
            int meilleur_delta = 0;
            meilleurs_mouvements.clear();

            // Exploration du voisinage restreint aux sommets actuellement en conflit
            for (int u = 0; u < V; ++u) {
                int c_actuelle = G.get_couleur_sommet(u);
                int conflits_actuels = G.get_nb_conflits(u, c_actuelle);

                // Filtrage O(1) : si le sommet n'est pas en conflit, aucun gain strict possible
                if (conflits_actuels == 0) continue;

                for (int c = 0; c < k; ++c) {
                    if (c == c_actuelle) continue;

                    stats.solutions_evaluees++;
                    int delta = G.get_nb_conflits(u, c) - conflits_actuels;

                    // On ne retient que les mouvements strictement améliorants (delta < 0)
                    if (delta < 0) {
                        if (delta < meilleur_delta) {
                            meilleur_delta = delta;
                            meilleurs_mouvements.clear();
                            meilleurs_mouvements.push_back({u, c});
                        } else if (delta == meilleur_delta) {
                            meilleurs_mouvements.push_back({u, c});
                        }
                    }
                }
            }

            // Optimum local atteint : aucun voisin n'améliore strictement le coût
            if (meilleurs_mouvements.empty()) {
                break;
            }

            // Choix aléatoire parmi les meilleurs mouvements ex-aequo
            std::uniform_int_distribution<int> dist(0, static_cast<int>(meilleurs_mouvements.size()) - 1);
            auto [sommet_choisi, couleur_choisie] = meilleurs_mouvements[dist(rng)];

            // Application en O(V)
            G.colorerSommet(sommet_choisi, couleur_choisie);
            stats.iterations++;

            auto t_courant = std::chrono::high_resolution_clock::now();
            stats.temps_meilleur_sec = std::chrono::duration<double>(t_courant - t_debut).count();
        }

        auto t_fin = std::chrono::high_resolution_clock::now();
        stats.temps_total_sec = std::chrono::duration<double>(t_fin - t_debut).count();
        stats.conflits_finaux = G.get_cout_total();

        return stats;
    }
};

#endif