#ifndef METAHEURISTIQUE_HPP
#define METAHEURISTIQUE_HPP

#include <chrono>
#include <random>
#include <vector>
#include "graph.hpp"

struct Mouvement {
    int sommet = -1;
    int couleur = -1;
    int delta = 0;
};

struct ParamsTabou {
    long long max_iterations = 1'000'000;
    int tenure_base = 10;
    double tenure_alpha = 0.6;
    unsigned int graine = 42;
};

class Metaheuristique {
public:
    explicit Metaheuristique(graph& graphe, unsigned int graine = 42);

    void glouton();
    void initialisation_aleatoire();

    // Voisinage V(c) : recoloration d'un seul sommet actuellement en conflit.
    const std::vector<int>& sommets_en_conflit() const { return conflits; }
    int delta(int u, int c) const;
    Mouvement meilleur_mouvement(long long iteration, int meilleur_cout);
    void appliquer(const Mouvement& m);

    // Requiert une coloration complete, restaure la meilleure coloration a la fin.
    bool tabou(const ParamsTabou& p);

    int meilleur_cout() const { return best_cout; }
    const std::vector<int>& meilleure_coloration() const { return best_coloration; }
    long long iterations_effectuees() const { return nb_iterations; }
    long long solutions_evaluees() const { return nb_evaluations; }
    int conflits_min_evalues() const { return min_conflits_evalues; }
    int conflits_max_evalues() const { return max_conflits_evalues; }
    double conflits_moy_evalues() const {
        return nb_evaluations ? static_cast<double>(somme_conflits_evalues) / nb_evaluations : 0.0;
    }
    double temps_vers_meilleure_secondes() const { return secondes_vers_meilleure; }

private:
    graph& G;
    std::mt19937 rng;
    std::vector<int> conflits;
    std::vector<int> pos_conflit;
    // tabou_jusqua[u][c] : interdiction de revenir a la couleur c pour u.
    std::vector<std::vector<long long>> tabou_jusqua;
    std::vector<int> best_coloration;
    int best_cout;
    long long nb_iterations = 0;
    long long nb_evaluations = 0;
    long long somme_conflits_evalues = 0;
    int min_conflits_evalues = 0;
    int max_conflits_evalues = 0;
    void enregistrer_evaluation(int cout_voisin);
    double secondes_vers_meilleure = 0.0;

    bool coloration_complete() const;
    void reconstruire_conflits();
    void maj_conflit(int u);
    void mettre_a_jour_apres_mouvement(int u);
    bool est_tabou(int u, int c, long long iteration) const;
    void rendre_tabou(int u, int ancienne_couleur, long long iteration, const ParamsTabou& p);
    void sauvegarder_si_meilleure();
    void restaurer_meilleure();
};

#endif
