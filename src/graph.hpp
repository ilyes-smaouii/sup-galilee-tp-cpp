#include <unordered_set>
#include <unordered_map>

namespace graph
{
  template <typename N, typename ND, typename Hash, typename Equal>
  struct NodeData
  {
    ND m_data {};
    std::unordered_set<N, Hash, Equal> m_succs {};
    std::unordered_set<N, Hash, Equal> m_preds {};

    NodeData(ND data) : m_data(data), m_succs(), m_preds() {}
    void add_succ(N succ) {
      m_succs.insert(succ);
    }
    void add_pred(N pred) {
      m_preds.insert(pred);
    }
  };

  template <typename N, typename Hash = std::hash<N>>
  struct pair_hash {
    std::size_t operator()(const std::pair<N, N>& v) const {
      std::size_t v1 = v.first;
      std::size_t v2 = v.second;
      return v2 + 0x9e3779b9 + (v1<<6) + (v1 >> 2);
    }
  };
  template <typename N, typename Equal = std::equal_to<N>>
  struct pair_eq {
    bool operator()(const std::pair<N, N>& lhs, const std::pair<N, N>& rhs) const {
      return Equal()(lhs.first, rhs.first) && Equal()(lhs.second, rhs.second);
    }
  };

  template <typename N, typename ND, typename ED, typename Hash = std::hash<N>, typename Equal = std::equal_to<N>>
  struct Graph
  {
    using edge_t = std::pair<N, N>;
    using node_data_t = NodeData<N, ND, Hash, Equal>;
    using pair_data_t = NodeData<edge_t, ED, pair_hash<N, Hash>, pair_eq<N, Equal>>;
    std::unordered_map<N, node_data_t> m_nodes{};
    std::unordered_map<edge_t, pair_data_t> m_edges{};

    Graph() : m_nodes(), m_edges() {}
    void add_node(N n, ND d) {
      node_data_t node_data(d);
      m_nodes.insert(std::make_pair(n, node_data));
    }
    void add_edge(N pred, N succ, ED d) {
      // preds and succs !
    }
  };
  
  
} // namespace graph
