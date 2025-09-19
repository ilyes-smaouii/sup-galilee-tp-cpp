#pragma once

#include <concepts>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace graph {
template <typename N, typename ND, typename Hash, typename Equal>
struct NodeData {
  ND m_data{};
  std::unordered_set<N, Hash, Equal> m_succs{};
  std::unordered_set<N, Hash, Equal> m_preds{};

  NodeData(ND data) : m_data(data), m_succs(), m_preds() {}
  void add_succ(N succ) { m_succs.insert(succ); }
  void add_pred(N pred) { m_preds.insert(pred); }
  void rm_succ(N succ) { m_succs.erase(succ); }
  void rm_pred(N pred) { m_preds.erase(pred); }
};

template <typename N, typename Hash = std::hash<N>>
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

template <typename N, typename ND, typename ED, typename Hash = std::hash<N>,
          typename Equal = std::equal_to<N>>
struct Graph; // forward declaration

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
struct struct_next {
  using set_t = std::unordered_set<N, Hash, Equal>;
  using iterator_t = typename set_t::iterator;
  // using iterator_t =
  //     decltype(Graph<N, ND, ED, Hash,
  //     Equal>::node_data_t::m_succs)::iterator;
  static_assert(
      std::same_as<typename set_t::iterator,
                   typename decltype(Graph<N, ND, ED, Hash, Equal>::
                                         node_data_t::m_succs)::iterator>,
      "Error : both types should be equivalent !");
  using const_iterator_t = typename set_t::const_iterator;
  Graph<N, ND, ED, Hash, Equal> *const _g;
  const N _prev;
  struct_next(Graph<N, ND, ED> *graph, const N prev) : _g(graph), _prev(prev) {}
  iterator_t begin();
  iterator_t end();
  const_iterator_t begin() const;
  const_iterator_t end() const;
};

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
struct struct_pred {
  using set_t = std::unordered_set<N, Hash, Equal>;
  using iterator_t = typename set_t::iterator;
  using const_iterator_t = typename set_t::const_iterator;
  Graph<N, ND, ED, Hash, Equal> *const _g;
  const N _next;
  struct_pred(Graph<N, ND, ED> *graph, const N next) : _g(graph), _next(next) {}
  iterator_t begin();
  iterator_t end();
  const_iterator_t begin() const;
  const_iterator_t end() const;
};

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
struct Graph {
  using edge_t = std::pair<N, N>;
  using node_data_t = NodeData<N, ND, Hash, Equal>;
  using pair_data_t =
      NodeData<edge_t, ED, pair_hash<N, Hash>, pair_eq<N, Equal>>;
  std::unordered_map<N, node_data_t> m_nodes{};
  std::unordered_map<edge_t, pair_data_t, pair_hash<N, Hash>, pair_eq<N, Equal>>
      m_edges{};

  Graph() : m_nodes(), m_edges() {}
  void add_node(N n, ND d) {
    node_data_t node_data(d);
    m_nodes.insert(std::make_pair(n, node_data));
  }
  void add_edge(N pred, N succ, ED d) {
    auto pred_iter = m_nodes.find(pred);
    auto succ_iter = m_nodes.find(succ);
    if (pred_iter != m_nodes.end() && succ_iter != m_nodes.end()) {
      auto pair_data = pair_data_t(d);
      // insert edge
      m_edges.insert(std::make_pair(std::make_pair(pred, succ), pair_data));
      // update succ and pred
      pred_iter->second.add_succ(succ);
      succ_iter->second.add_pred(pred);
    } else {
      throw std::runtime_error(
          "add_edge() error : pred or succ missing from m_nodes !");
    }
  }
  void rm_edge(N pred, N succ) {
    auto pred_iter = m_nodes.find(pred);
    auto succ_iter = m_nodes.find(succ);
    if (pred_iter == m_nodes.end() || succ_iter == m_nodes.end()) {
      // at least one of the nodes is missing
      throw std::runtime_error("Graph::rm_edge() error : missing node(s) !");
    } else {
      auto edge_iter = m_edges.find(std::make_pair(pred, succ));
      if (edge_iter != m_edges.end()) {
        m_edges.erase(edge_iter);
        pred_iter->second.rm_succ(succ);
        pred_iter->second.rm_pred(pred);
      } else {
        // nodes present, but there's not edge between them
        throw std::runtime_error("Graph::rm_edge() error : missing edge !");
      }
    }
  }
  bool has_node(const N &node) { return (m_nodes.find(node) != m_nodes.end()); }
  ND &operator[](const N &node) {
    auto node_iter = m_nodes.find(node);
    if (node_iter == m_nodes.end()) {
      throw std::runtime_error(
          "graph::Graph::operator[] error : can't find node !");
    } else {
      return node_iter->second.m_data;
    }
  }
  ED &operator[](const std::pair<N, N> &edge) {
    auto edge_iter = m_edges.find(edge);
    if (edge_iter == m_edges.end()) {
      throw std::runtime_error(
          "graph::Graph::operator[] error : can't find edge !");
    } else {
      return edge_iter->second.m_data;
    }
  }
  struct_next<N, ND, ED, Hash, Equal> next(N node) {
    return struct_next<N, ND, ED, Hash, Equal>(this, node);
  }
  const struct_next<N, ND, ED, Hash, Equal> next(N node) const {
    return struct_next<N, ND, ED, Hash, Equal>(this, node);
  }
  struct_pred<N, ND, ED, Hash, Equal> pred(N node) {
    return struct_pred<N, ND, ED, Hash, Equal>(this, node);
  }
  const struct_pred<N, ND, ED, Hash, Equal> pred(N node) const {
    return struct_pred<N, ND, ED, Hash, Equal>(this, node);
  }
};

template <typename N, typename Hash, typename Equal>
struct NodeData<N, void, Hash, Equal> {
  std::unordered_set<N, Hash, Equal> m_succs{};
  std::unordered_set<N, Hash, Equal> m_preds{};

  NodeData() : m_succs(), m_preds() {}
  void add_succ(N succ) { m_succs.insert(succ); }
  void add_pred(N pred) { m_preds.insert(pred); }
  void rm_succ(N succ) { m_succs.erase(succ); }
  void rm_pred(N pred) { m_preds.erase(pred); }
};

template <typename N, typename ED, typename Hash, typename Equal>
struct Graph<N, void, ED, Hash, Equal> {
  using edge_t = std::pair<N, N>;
  using node_data_t = NodeData<N, void, Hash, Equal>;
  using pair_data_t =
      NodeData<edge_t, ED, pair_hash<N, Hash>, pair_eq<N, Equal>>;
  std::unordered_map<N, node_data_t> m_nodes{};
  std::unordered_map<edge_t, pair_data_t, pair_hash<N, Hash>, pair_eq<N, Equal>>
      m_edges{};

  Graph() : m_nodes(), m_edges() {}
  void add_node(N n) {
    node_data_t node_data{};
    m_nodes.insert(std::make_pair(n, node_data));
  }
  void add_edge(N pred, N succ, ED d) {
    auto pred_iter = m_nodes.find(pred);
    auto succ_iter = m_nodes.find(succ);
    if (pred_iter != m_nodes.end() && succ_iter != m_nodes.end()) {
      auto pair_data = pair_data_t(d);
      // insert edge
      m_edges.insert(std::make_pair(std::make_pair(pred, succ), pair_data));
      // update succ and pred
      pred_iter->second.add_succ(succ);
      succ_iter->second.add_pred(pred);
    } else {
      throw std::runtime_error(
          "add_edge() error : pred or succ missing from m_nodes !");
    }
  }
  bool has_node(const N &node) { return (m_nodes.find(node) != m_nodes.end()); }
  // ND &operator[](const N &node) {
  //   auto node_iter = m_nodes.find(node);
  //   if (node_iter == m_nodes.end()) {
  //     throw std::runtime_error(
  //         "graph::Graph::operator[] error : can't find node !");
  //   } else {
  //     return node_iter->second.m_data;
  //   }
  // }
  ED &operator[](const std::pair<N, N> &edge) {
    auto edge_iter = m_edges.find(edge);
    if (edge_iter == m_edges.end()) {
      throw std::runtime_error(
          "graph::Graph::operator[] error : can't find edge !");
    } else {
      return edge_iter->second.m_data;
    }
  }
  struct_next<N, void, ED, Hash, Equal> next(N node) {
    return struct_next<N, void, ED, Hash, Equal>(this, node);
  }
  const struct_next<N, void, ED, Hash, Equal> next(N node) const {
    return struct_next<N, void, ED, Hash, Equal>(this, node);
  }
  struct_pred<N, void, ED, Hash, Equal> pred(N node) {
    return struct_pred<N, void, ED, Hash, Equal>(this, node);
  }
  const struct_pred<N, void, ED, Hash, Equal> pred(N node) const {
    return struct_pred<N, void, ED, Hash, Equal>(this, node);
  }
};

template <typename N, typename ND, typename Hash, typename Equal>
struct Graph<N, ND, void, Hash, Equal> {
  using edge_t = std::pair<N, N>;
  using node_data_t = NodeData<N, ND, Hash, Equal>;
  using pair_data_t =
      NodeData<edge_t, void, pair_hash<N, Hash>, pair_eq<N, Equal>>;
  std::unordered_map<N, node_data_t> m_nodes{};
  std::unordered_map<edge_t, pair_data_t, pair_hash<N, Hash>, pair_eq<N, Equal>>
      m_edges{};

  Graph() : m_nodes(), m_edges() {}
  void add_node(N n, ND d) {
    node_data_t node_data(d);
    m_nodes.insert(std::make_pair(n, node_data));
  }
  void add_edge(N pred, N succ) {
    auto pred_iter = m_nodes.find(pred);
    auto succ_iter = m_nodes.find(succ);
    if (pred_iter != m_nodes.end() && succ_iter != m_nodes.end()) {
      auto pair_data = pair_data_t{};
      // insert edge
      m_edges.insert(std::make_pair(std::make_pair(pred, succ), pair_data));
      // update succ and pred
      pred_iter->second.add_succ(succ);
      succ_iter->second.add_pred(pred);
    } else {
      throw std::runtime_error(
          "add_edge() error : pred or succ missing from m_nodes !");
    }
  }
  void rm_edge(N pred, N succ) {
    auto pred_iter = m_nodes.find(pred);
    auto succ_iter = m_nodes.find(succ);
    if (pred_iter == m_nodes.end() || succ_iter == m_nodes.end()) {
      // at least one of the nodes is missing
      throw std::runtime_error("Graph::rm_edge() error : missing node(s) !");
    } else {
      auto edge_iter = m_edges.find(std::make_pair(pred, succ));
      if (edge_iter != m_edges.end()) {
        m_edges.erase(edge_iter);
        pred_iter->second.rm_succ(succ);
        pred_iter->second.rm_pred(pred);
      } else {
        // nodes present, but there's not edge between them
        throw std::runtime_error("Graph::rm_edge() error : missing edge !");
      }
    }
  }
  bool has_node(const N &node) { return (m_nodes.find(node) != m_nodes.end()); }
  ND &operator[](const N &node) {
    auto node_iter = m_nodes.find(node);
    if (node_iter == m_nodes.end()) {
      throw std::runtime_error(
          "graph::Graph::operator[] error : can't find node !");
    } else {
      return node_iter->second.m_data;
    }
  }
  // ED &operator[](const std::pair<N, N> &edge) {
  //   auto edge_iter = m_edges.find(edge);
  //   if (edge_iter == m_edges.end()) {
  //     throw std::runtime_error(
  //         "graph::Graph::operator[] error : can't find edge !");
  //   } else {
  //     return edge_iter->second.m_data;
  //   }
  // }
  struct_next<N, ND, void, Hash, Equal> next(N node) {
    return struct_next<N, ND, void, Hash, Equal>(this, node);
  }
  const struct_next<N, ND, void, Hash, Equal> next(N node) const {
    return struct_next<N, ND, void, Hash, Equal>(this, node);
  }
  struct_pred<N, ND, void, Hash, Equal> pred(N node) {
    return struct_pred<N, ND, void, Hash, Equal>(this, node);
  }
  const struct_pred<N, ND, void, Hash, Equal> pred(N node) const {
    return struct_pred<N, ND, void, Hash, Equal>(this, node);
  }
};

// struct_next

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_next<N, ND, ED, Hash, Equal>::iterator_t
struct_next<N, ND, ED, Hash, Equal>::begin() {
  auto prev_iter = _g->m_nodes.find(_prev);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_succs.begin();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_next<N, ND, ED, Hash, Equal>::iterator_t
struct_next<N, ND, ED, Hash, Equal>::end() {
  auto prev_iter = _g->m_nodes.find(_prev);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_succs.end();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_next<N, ND, ED, Hash, Equal>::const_iterator_t
struct_next<N, ND, ED, Hash, Equal>::begin() const {
  auto prev_iter = _g->m_nodes.find(_prev);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_succs.begin();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_next<N, ND, ED, Hash, Equal>::const_iterator_t
struct_next<N, ND, ED, Hash, Equal>::end() const {
  auto prev_iter = _g->m_nodes.find(_prev);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_succs.end();
  }
}

// struct_pred

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_pred<N, ND, ED, Hash, Equal>::iterator_t
struct_pred<N, ND, ED, Hash, Equal>::begin() {
  auto prev_iter = _g->m_nodes.find(_next);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_preds.begin();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_pred<N, ND, ED, Hash, Equal>::iterator_t
struct_pred<N, ND, ED, Hash, Equal>::end() {
  auto prev_iter = _g->m_nodes.find(_next);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_preds.end();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_pred<N, ND, ED, Hash, Equal>::const_iterator_t
struct_pred<N, ND, ED, Hash, Equal>::begin() const {
  auto prev_iter = _g->m_nodes.find(_next);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_preds.begin();
  }
}

template <typename N, typename ND, typename ED, typename Hash, typename Equal>
typename struct_pred<N, ND, ED, Hash, Equal>::const_iterator_t
struct_pred<N, ND, ED, Hash, Equal>::end() const {
  auto prev_iter = _g->m_nodes.find(_next);
  if (prev_iter == _g->m_nodes.end()) {
    return {};
  } else {
    return prev_iter->second.m_preds.end();
  }
}

} // namespace graph
