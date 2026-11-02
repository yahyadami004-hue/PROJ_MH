#include "graph.hpp"
#include "metaheuristique.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using Horloge = std::chrono::steady_clock;

// Une ligne CSV par execution de recherche tabou et par couple (instance, k).
// Les statistiques de conflits concernent TOUS les voisins evalues par le tabou,
// pas les meilleures solutions des differentes repetitions.
int main(int argc, char* argv[]) {
    const fs::path dossier = (argc >= 2) ? fs::path(argv[1]) : fs::path("instances");

    // A REMPLIR : une sous-liste par fichier, en ordre alphabetique.
    const std::vector<std::vector<int>> k_par_instance = {
        {147,146,73},
        {261,260,130},
        {5},
        {44,22},
        {8},
        {28},
        {72,36},
        {86,85,84,42},
        {224,223,222,111},
        {26,13},
        {15,8},

    };
    const int repetitions = 1; // Nombre d'executions independantes de chaque (instance, k)

    ParamsTabou params;
    params.max_iterations = 1'000'000;
    params.tenure_base = 10;
    params.tenure_alpha = 0.6;
    params.graine = 42;

    try {
        if (!fs::exists(dossier) || !fs::is_directory(dossier))
            throw std::runtime_error("Dossier introuvable : " + dossier.string());
        if (repetitions <= 0) throw std::runtime_error("repetitions doit etre > 0");

        std::vector<fs::path> fichiers;
        for (const auto& entree : fs::directory_iterator(dossier)) {
            if (!entree.is_regular_file()) continue;
            const auto ext = entree.path().extension();
            if (ext == ".col" || ext == ".txt") fichiers.push_back(entree.path());
        }
        std::sort(fichiers.begin(), fichiers.end(), [](const fs::path& a, const fs::path& b) {
            return a.filename().string() < b.filename().string();
        });
        if (fichiers.empty()) throw std::runtime_error("Aucune instance .col ou .txt trouvee");

        std::cout << "Fichiers dans l'ordre alphabetique :\n";
        for (std::size_t i = 0; i < fichiers.size(); ++i)
            std::cout << "  [" << i << "] " << fichiers[i].filename().string() << '\n';

        if (k_par_instance.size() != fichiers.size())
            throw std::runtime_error("Indiquer une sous-liste de k par fichier dans k_par_instance");
        for (const auto& liste : k_par_instance) {
            if (liste.empty()) throw std::runtime_error("Chaque instance doit avoir au moins un k");
            for (int k : liste)
                if (k <= 0) throw std::runtime_error("Toutes les valeurs de k doivent etre positives");
        }

        std::ofstream csv("resultats.csv");
        if (!csv) throw std::runtime_error("Impossible de creer resultats.csv");
        csv << "instance;n;m;k;execution;graine;conflits_min_evalues;conflits_max_evalues;"
               "conflits_moy_evalues;meilleur_cout;temps_total_s;"
               "temps_vers_meilleure_s;solutions_evaluees;solutions_par_s;iterations\n";
        csv << std::setprecision(10);

        std::cout << std::left << std::setw(27) << "Instance"
                  << std::right << std::setw(5) << "k" << std::setw(7) << "Essai"
                  << std::setw(9) << "Min" << std::setw(9) << "Max"
                  << std::setw(11) << "Moy" << std::setw(10) << "Best"
                  << std::setw(14) << "Temps(s)" << std::setw(14) << "T best(s)"
                  << std::setw(15) << "Eval/s" << '\n';
        std::cout << std::string(121, '-') << '\n';

        std::size_t total_executions = 0;
        for (std::size_t i = 0; i < fichiers.size(); ++i) {
            graph original(fichiers[i].string());
            for (int k : k_par_instance[i]) {
                for (int r = 0; r < repetitions; ++r) {
                    graph G = original;
                    G.set_nb_couleur(k);
                    ParamsTabou p = params;
                    p.graine = params.graine + static_cast<unsigned int>(r);
                    Metaheuristique algo(G, p.graine);

                    const auto debut = Horloge::now();
                    algo.glouton();
                    const double temps_glouton = std::chrono::duration<double>(Horloge::now() - debut).count();
                    algo.tabou(p);
                    const double temps_total = std::chrono::duration<double>(Horloge::now() - debut).count();
                    const double temps_meilleure = temps_glouton + algo.temps_vers_meilleure_secondes();
                    const long long evals = algo.solutions_evaluees();
                    const double debit = temps_total > 0 ? evals / temps_total : 0.0;

                    // Si la recherche ne visite aucun voisin (cout initial = 0,
                    // ou k = 1), les statistiques de voisins ne sont pas definies.
                    const std::string min_s = evals ? std::to_string(algo.conflits_min_evalues()) : "NA";
                    const std::string max_s = evals ? std::to_string(algo.conflits_max_evalues()) : "NA";
                    std::cout << std::left << std::setw(27) << fichiers[i].filename().string()
                              << std::right << std::setw(5) << k << std::setw(7) << (r + 1)
                              << std::setw(9) << min_s << std::setw(9) << max_s
                              << std::setw(11) << std::fixed << std::setprecision(2)
                              << (evals ? std::to_string(algo.conflits_moy_evalues()) : "NA")
                              << std::setw(10) << algo.meilleur_cout()
                              << std::setw(14) << std::setprecision(3) << temps_total
                              << std::setw(14) << temps_meilleure
                              << std::setw(15) << std::setprecision(0) << debit << '\n';

                    csv << fichiers[i].filename().string() << ';' << G.get_nb_sommet() << ';'
                        << G.get_nb_arete() << ';' << k << ';' << (r + 1) << ';' << p.graine << ';';
                    if (evals) {
                        csv << algo.conflits_min_evalues() << ';' << algo.conflits_max_evalues()
                            << ';' << algo.conflits_moy_evalues();
                    } else {
                        csv << "NA;NA;NA";
                    }
                    csv << ';' << algo.meilleur_cout() << ';' << temps_total << ';'
                        << temps_meilleure << ';' << evals << ';' << debit << ';'
                        << algo.iterations_effectuees() << '\n';
                    ++total_executions;
                }
            }
        }
        std::cout << "\n" << total_executions << " executions. Resultats : resultats.csv\n";
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << '\n';
        return 1;
    }
    return 0;
}
