#include "metaheuristique.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>

Metaheuristique::Metaheuristique(graph& graphe, unsigned int graine)
    : G(graphe), rng(graine),
      pos_conflit(graphe.get_nb_sommet(), -1),
      tabou_jusqua(graphe.get_nb_sommet(),
                   std::vector<long long>(graphe.get_nb_couleur(), -1)),
      best_cout(std::numeric_limits<int>::max()) {
    reconstruire_conflits();
    sauvegarder_si_meilleure();
}

bool Metaheuristique::coloration_complete() const {
    const int n = G.get_nb_sommet();
    const int k = G.get_nb_couleur();
    if (n > 0 && k <= 0) return false;
    for (int u = 0; u < n; ++u) {
        const int c = G.get_couleur_sommet(u);
        if (c < 0 || c >= k) return false;
    }
    return true;
}

void Metaheuristique::reconstruire_conflits() {
    conflits.clear();
    pos_conflit.assign(G.get_nb_sommet(), -1);
    for (int u = 0; u < G.get_nb_sommet(); ++u) maj_conflit(u);
}

void Metaheuristique::maj_conflit(int u) {
    const int c = G.get_couleur_sommet(u);
    const bool en_conflit = c >= 0 && G.get_nb_conflits(u, c) > 0;
    const int position = pos_conflit[u];
    if (en_conflit && position == -1) {
        pos_conflit[u] = static_cast<int>(conflits.size());
        conflits.push_back(u);
    } else if (!en_conflit && position != -1) {
        const int dernier = conflits.back();
        conflits[position] = dernier;
        pos_conflit[dernier] = position;
        conflits.pop_back();
        pos_conflit[u] = -1;
    }
}

void Metaheuristique::mettre_a_jour_apres_mouvement(int u) {
    maj_conflit(u);
    // La representation actuelle du graphe est une matrice d'adjacence.
    for (int v = 0; v < G.get_nb_sommet(); ++v) {
        if (G.get_valeur_matrice(u, v) == 1) maj_conflit(v);
    }
}

bool Metaheuristique::est_tabou(int u, int c, long long iteration) const {
    return iteration < tabou_jusqua[u][c];
}

void Metaheuristique::rendre_tabou(int u, int ancienne_couleur,
                                   long long iteration, const ParamsTabou& p) {
    const double duree = static_cast<double>(p.tenure_base) +
                         p.tenure_alpha * static_cast<double>(conflits.size());
    const long long tenure = std::max(1LL, static_cast<long long>(std::floor(duree)));
    // Si le mouvement est fait a l'iteration i, le retour est interdit
    // aux iterations i+1, ..., i+tenure inclus.
    tabou_jusqua[u][ancienne_couleur] = iteration + tenure + 1;
}

int Metaheuristique::delta(int u, int c) const {
    if (u < 0 || u >= G.get_nb_sommet() ||
        c < 0 || c >= G.get_nb_couleur()) {
        throw std::out_of_range("delta: sommet ou couleur invalide");
    }
    const int ancienne = G.get_couleur_sommet(u);
    if (ancienne < 0) {
        throw std::logic_error("delta: coloration incomplete");
    }
    return G.get_nb_conflits(u, c) - G.get_nb_conflits(u, ancienne);
}

void Metaheuristique::sauvegarder_si_meilleure() {
    if (!coloration_complete()) return;
    const int cout = G.get_cout_total();
    if (cout < best_cout) {
        best_cout = cout;
        best_coloration.resize(G.get_nb_sommet());
        for (int u = 0; u < G.get_nb_sommet(); ++u)
            best_coloration[u] = G.get_couleur_sommet(u);
    }
}

void Metaheuristique::restaurer_meilleure() {
    if (best_coloration.size() != static_cast<std::size_t>(G.get_nb_sommet())) return;
    for (int u = 0; u < G.get_nb_sommet(); ++u)
        G.colorerSommet(u, best_coloration[u]);
    reconstruire_conflits();
}

void Metaheuristique::glouton() {
    const int n = G.get_nb_sommet();
    const int k = G.get_nb_couleur();
    if (n > 0 && k <= 0) throw std::invalid_argument("glouton: k doit etre positif");
    G.sansCouleur();
    std::vector<int> degres(n, 0);
    for (int u = 0; u < n; ++u)
        for (int v = 0; v < n; ++v)
            degres[u] += G.get_valeur_matrice(u, v) == 1;

    // DSATUR : sommet non colore de saturation maximale, puis degre maximal.
    for (int affectes = 0; affectes < n; ++affectes) {
        int meilleur_u = -1, meilleure_sat = -1, meilleur_degre = -1;
        for (int u = 0; u < n; ++u) {
            if (G.get_couleur_sommet(u) != -1) continue;
            std::vector<unsigned char> vues(k, 0);
            int sat = 0;
            for (int v = 0; v < n; ++v) {
                if (G.get_valeur_matrice(u, v) != 1) continue;
                const int cv = G.get_couleur_sommet(v);
                if (cv >= 0 && !vues[cv]) { vues[cv] = 1; ++sat; }
            }
            if (sat > meilleure_sat || (sat == meilleure_sat && degres[u] > meilleur_degre)) {
                meilleur_u = u;
                meilleure_sat = sat;
                meilleur_degre = degres[u];
            }
        }
        int meilleur_conflit = std::numeric_limits<int>::max();
        int couleur_choisie = -1;
        int egalites = 0;
        for (int c = 0; c < k; ++c) {
            const int cout = G.get_nb_conflits(meilleur_u, c);
            if (cout < meilleur_conflit) {
                meilleur_conflit = cout;
                couleur_choisie = c;
                egalites = 1;
            } else if (cout == meilleur_conflit) {
                ++egalites;
                if (std::uniform_int_distribution<int>(1, egalites)(rng) == 1)
                    couleur_choisie = c;
            }
        }
        G.colorerSommet(meilleur_u, couleur_choisie);
    }
    reconstruire_conflits();
    sauvegarder_si_meilleure();
}

void Metaheuristique::initialisation_aleatoire() {
    const int n = G.get_nb_sommet();
    const int k = G.get_nb_couleur();
    if (n > 0 && k <= 0)
        throw std::invalid_argument("initialisation_aleatoire: k doit etre positif");
    G.sansCouleur();
    if (n > 0) {
        std::uniform_int_distribution<int> distribution(0, k - 1);
        for (int u = 0; u < n; ++u) G.colorerSommet(u, distribution(rng));
    }
    reconstruire_conflits();
    sauvegarder_si_meilleure();
}

void Metaheuristique::enregistrer_evaluation(int cout_voisin) {
    if (nb_evaluations == 0) {
        min_conflits_evalues = max_conflits_evalues = cout_voisin;
    } else {
        min_conflits_evalues = std::min(min_conflits_evalues, cout_voisin);
        max_conflits_evalues = std::max(max_conflits_evalues, cout_voisin);
    }
    somme_conflits_evalues += cout_voisin;
    ++nb_evaluations;
}

Mouvement Metaheuristique::meilleur_mouvement(long long iteration, int meilleur_cout) {
    Mouvement meilleur;
    int meilleur_delta = std::numeric_limits<int>::max();
    int egalites = 0;
    const int cout_actuel = G.get_cout_total();

    // On considere les mouvements admissibles en premier.
    // En cas de blocage total, on autorise un mouvement tabou pour avancer.
    for (int passage = 0; passage < 2; ++passage) {
        for (int u : conflits) {
            const int ancienne = G.get_couleur_sommet(u);
            const int conflits_actuels = G.get_nb_conflits(u, ancienne);
            for (int c = 0; c < G.get_nb_couleur(); ++c) {
                if (c == ancienne) continue;
                const int d = G.get_nb_conflits(u, c) - conflits_actuels;
                enregistrer_evaluation(cout_actuel + d);
                const bool interdit = est_tabou(u, c, iteration) &&
                                      cout_actuel + d >= meilleur_cout;
                if (passage == 0 && interdit) continue;
                if (d < meilleur_delta) {
                    meilleur_delta = d;
                    meilleur = {u, c, d};
                    egalites = 1;
                } else if (d == meilleur_delta) {
                    ++egalites;
                    if (std::uniform_int_distribution<int>(1, egalites)(rng) == 1)
                        meilleur = {u, c, d};
                }
            }
        }
        if (meilleur.sommet != -1) break;
    }
    return meilleur;
}

void Metaheuristique::appliquer(const Mouvement& m) {
    if (m.sommet < 0 || m.sommet >= G.get_nb_sommet() ||
        m.couleur < 0 || m.couleur >= G.get_nb_couleur())
        throw std::out_of_range("appliquer: mouvement invalide");
    if (!coloration_complete()) throw std::logic_error("appliquer: coloration incomplete");
    G.colorerSommet(m.sommet, m.couleur);
    mettre_a_jour_apres_mouvement(m.sommet);
}

bool Metaheuristique::tabou(const ParamsTabou& p) {
    if (p.max_iterations < 0 || p.tenure_base < 0 ||
        !std::isfinite(p.tenure_alpha) || p.tenure_alpha < 0.0)
        throw std::invalid_argument("tabou: parametres invalides");
    if (!coloration_complete())
        throw std::logic_error("tabou: initialiser une coloration complete avant la recherche");

    rng.seed(p.graine);
    const int n = G.get_nb_sommet();
    const int k = G.get_nb_couleur();
    tabou_jusqua.assign(n, std::vector<long long>(k, -1));
    reconstruire_conflits();
    nb_iterations = 0;
    nb_evaluations = 0;
    somme_conflits_evalues = 0;
    min_conflits_evalues = 0;
    max_conflits_evalues = 0;
    secondes_vers_meilleure = 0.0;
    sauvegarder_si_meilleure();
    const auto debut_recherche = std::chrono::steady_clock::now();

    for (long long iteration = 0;
         iteration < p.max_iterations && G.get_cout_total() > 0; ++iteration) {
        const Mouvement m = meilleur_mouvement(iteration, best_cout);
        if (m.sommet == -1) break;
        const int ancienne = G.get_couleur_sommet(m.sommet);
        rendre_tabou(m.sommet, ancienne, iteration, p);
        appliquer(m);
        ++nb_iterations;
        const int precedent = best_cout;
        sauvegarder_si_meilleure();
        if (best_cout < precedent)
            secondes_vers_meilleure = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - debut_recherche).count();
    }
    restaurer_meilleure();
    return best_cout == 0;
}
