#include "graph.hpp"
#include <cstddef>
#include <cstdio>
#include <unordered_set>
#include <utility>

namespace automata {
/*
struct pair_hash {
std::size_t operator()(const std::pair<N, N> &v) const {
  std::size_t v1 = Hash()(v.first);
  std::size_t v2 = Hash()(v.second);
  return v2 + 0x9e3779b9 + (v1 << 6) + (v1 >> 2);
}
};

template <typename N, typename Equal = std::equal_to<N>>
struct pair_eq {
bool operator()(const std::pair<N, N> &lhs,
                const std::pair<N, N> &rhs) const {
  return Equal()(lhs.first, rhs.first) && Equal()(lhs.second, rhs.second);
}
};
 */

using arc_t = std::pair<std::pair<std::size_t, std::size_t>, char>;

struct arc_hash {
  std::size_t operator()(const arc_t &arc) const;
};

struct arc_eq {
  bool operator()(const arc_t &arc1, const arc_t &arc2) const;
};

struct Automata {
  graph::Graph<std::size_t, void, char> m_g{};
  std::size_t m_counter_curr{};
  std::unordered_set<std::size_t> m_terminal{};
  std::unordered_set<arc_t, arc_hash, arc_eq> m_arcs{};
  /*
    Initialise le graphe à un unique noeud 0, counter_curr à 1 et m_terminal à
    {}
   */
  Automata();
  /*
    Rajoute un nouveau noeud m_counter_curr à m_g
    Incrémente m_counter_curr
    Renvoie le noeud créé
   */
  std::size_t new_node();
  /*
    Vérifie la présence de pred et succ
    Vérifie qu'il n'existe pas déjà un arc entre eux
    Rajoute l'arc à m_arcs
   */
  void new_arc(std::size_t pred, std::size_t succ, char some_char);
  /*
    Vérifie que node est bien un noeud du graphe
    Le rajoute à l'ensemble des noeuds terminaux
   */
  void new_terminal(std::size_t new_term_node);
  /*
    Vérifie que le word est bien reconnu par l'automate
   */
  bool parse(const std::string &word);
  /*
  [fonction auxiliaire]
  Vérifie que le word est bien reconnu par l'automate
  */
  bool parse_rec_aux(const char *curr_char_ptr, std::size_t curr_state);
  /*
    Vérifie que le word est bien reconnu par l'automate
   */
  bool parse_rec(const std::string &word);
};
} // namespace automata