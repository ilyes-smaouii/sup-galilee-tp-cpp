#include "automata.hpp"
#include "graph.hpp"
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace automata {

std::size_t arc_hash::operator()(const arc_t &arc) const {
  std::size_t v1 = std::hash<std::size_t>()(arc.first.first);
  std::size_t v2 = std::hash<std::size_t>()(arc.first.second);
  std::size_t v3 = std::hash<char>()(arc.second);
  std::size_t res = v1 ^ v2 + 0x9e3779b9 + (v1 << 6) + (v1 >> 2);
  res ^= v3 + 0x9e3779b9 + (v3 << 6) + (v3 >> 2);
  return res;
}

bool arc_eq::operator()(const arc_t &arc1, const arc_t &arc2) const {
  return (arc1.first.first == arc2.first.first) &&
         (arc1.first.second == arc2.first.second) &&
         (arc1.second == arc2.second);
}

Automata::Automata() : m_g(), m_counter_curr(0), m_terminal() {
  m_g.add_node(m_counter_curr++);
}

std::size_t Automata::new_node() {
  m_g.add_node(m_counter_curr++);
  return (m_counter_curr - 1);
}

void Automata::new_arc(std::size_t pred, std::size_t succ, char some_char) {
  auto pred_iter = m_g.m_nodes.find(pred);
  auto succ_iter = m_g.m_nodes.find(succ);
  if (pred_iter == m_g.m_nodes.end() || succ_iter == m_g.m_nodes.end()) {
    throw std::runtime_error(
        "Automata::new_arc() error : missing succ or pred !");
  }
  auto edge = std::make_pair(pred, succ);
  auto arc = std::make_pair(edge, some_char);
  auto edge_iter = m_arcs.find(arc);
  if (edge_iter != m_arcs.end()) {
    throw std::runtime_error(
        "Automata::new_arc() error : arc already present !");
  } else {
    decltype(m_g)::pair_data_t new_pair_data{some_char};
    m_arcs.insert(arc);
  }
}

void Automata::new_terminal(std::size_t new_term_node) {
  auto node_iter = m_g.m_nodes.find(new_term_node);
  if (node_iter == m_g.m_nodes.end()) {
    throw std::runtime_error(
        "Automata::new_terminal error : node not in graph's nodes !");
  } else {
    m_terminal.insert(new_term_node);
  }
}

bool Automata::parse(const std::string &word) {
  std::size_t state{0};
  for (const char &c : word) {
    bool found_edge{false};
    for (const auto &[edge, edge_char] : m_arcs) {
      if (found_edge) {
        break;
      }
      if (edge.first == state) {
        if (edge_char == c) {
          state = edge.second;
          found_edge = true;
        }
      }
    }
    if (!found_edge) {
      return false;
    }
  }
  // check that we're at terminal state
  if (m_terminal.find(state) != m_terminal.end()) {
    return true;
  } else {
    return false;
  }
}

bool Automata::parse_rec_aux(const char *curr_char_ptr,
                             std::size_t curr_state) {
  auto &curr_char = *curr_char_ptr;
  if (curr_char == 0) {
    return (m_terminal.find(curr_state) != m_terminal.end());
  }
  bool res{false};
  for (const auto &[edge, edge_char] : m_arcs) {
    if (edge.first == curr_state && edge_char == curr_char) {
      if (parse_rec_aux(curr_char_ptr + 1, edge.second)) {
        return true;
      }
    }
  }
  return false;
}

bool Automata::parse_rec(const std::string &word) {
  return parse_rec_aux(word.c_str(), 0);
}

} // namespace automata