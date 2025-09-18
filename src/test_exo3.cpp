#include "graph.hpp"
#include "pilelivre.hpp"
#include <functional>
#include <iostream>
#include <format>

pilelivre::PileLivre_v1 first_day() {
  pilelivre::PileLivre_v1 res{};
  res.add_person("James");
  res.add_msg_public("James", "Yoh, Salut PileLivre !");
  res.add_person("Jack");
  res.add_friend("James", "Jack");
  res.add_msg_private("Jack", "James", "t'es qui ???");
  res.rm_friend("Jack", "James");
  return res;
}

pilelivre::PileLivre_v1 second_day() {
  pilelivre::PileLivre_v1 res{};
  res.add_person("James");
  res.add_msg_public("James", "Yoh, Salut PileLivre !");
  res.add_person("Jack");
  res.add_friend("James", "Jack");
  res.add_msg_private("Jack", "James", "t'es qui ???");
  res.rm_friend("Jack", "James");
  return res;
}

void display_pilelivre_state(const pilelivre::PileLivre_v1& pl) {
  std::cout << "Content nodes :" << std::endl;
  for (const auto& node : pl.m_content.m_nodes) {
    std::cout << "Node ID : " << node.first << std::endl;
    std::cout << "Node Data : " << node.second.m_data << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Data :" << std::endl;
  for (const auto& datum_ptr : pl.m_data) {
    std::cout << "Datum : " << datum_ptr << std::endl;
    auto person_ptr = dynamic_cast<pilelivre::Person*>(datum_ptr);
    auto friend_add_ptr = dynamic_cast<pilelivre::FriendAdd*>(datum_ptr);
    auto friend_remove_ptr = dynamic_cast<pilelivre::FriendRemove*>(datum_ptr);
    auto msg_pub_ptr = dynamic_cast<pilelivre::MsgPublic*>(datum_ptr);
    auto msg_priv_ptr = dynamic_cast<pilelivre::MsgPrivate*>(datum_ptr);

    if (person_ptr != NULL) {
      std::cout << "[person]" << std::endl;
      std::cout << std::format("(name : {}, actions count : {}, friends count {})", person_ptr->m_name,
        person_ptr->m_action.size(), person_ptr->m_friends.size()) << std::endl;
      }
      if (friend_add_ptr != NULL) {
        std::cout << "[friend_add]" << std::endl;
        std::cout << std::format("(Added {})", friend_add_ptr->m_person->m_name) << std::endl;
      }
      if (friend_remove_ptr != NULL) {
        std::cout << "[friend_remove]" << std::endl;
        std::cout << std::format("(Removed {})", friend_remove_ptr->m_person->m_name) << std::endl;
      }
      if (msg_pub_ptr != NULL) {
        std::cout << "[msg_pub]" << std::endl;
        std::cout << std::format("(Msg content : \"{}\")", msg_pub_ptr->m_msg) << std::endl;
      }
      if (msg_priv_ptr != NULL) {
        std::cout << "[msg_priv]" << std::endl;
        std::cout << std::format("(Msg dest : {}, msg content : \"{}\")", msg_priv_ptr->m_dest->m_name, msg_priv_ptr->m_msg) << std::endl;
    }
  }
  std::cout << std::endl;
}

int main (int argc, char *argv[]) {
  auto res = first_day();
  // std::cout << "res.m_counter_msg : " << res.m_counter_msg << std::endl;
  // std::cout << "res.m_counter_cat : " << res.m_counter_cat << std::endl;
  // std::cout << "res.m_counter_friend : " << res.m_counter_friend << std::endl;
  display_pilelivre_state(res);
  pilelivre::MsgPublic new_msg("Hello world !");
  using graph_t = graph::Graph<std::string, pilelivre::Data*, int>;
  using nd_t = graph::NodeData<std::string, pilelivre::Data *, std::hash<std::string>, std::equal_to<std::string>>;
  nd_t new_nd(&new_msg);
  new_nd.add_succ("James");
  graph_t new_graph{};
  new_graph.add_node("James", new_nd.m_data);
  new_graph.add_node("Jack", new_nd.m_data);
  new_graph.add_node("Alice", new_nd.m_data);
  new_graph.add_node("Bob", new_nd.m_data);
  new_graph.add_edge("James", "Jack", 1);
  new_graph.add_edge("James", "Alice", 1);
  new_graph.add_edge("James", "Bob", 1);
  new_graph.add_edge("Alice", "Bob", 1);
  auto le_next = new_graph.next("James");
  std::cout << "James' next :" << std::endl;
  for (auto iter = le_next.begin(); iter != le_next.end(); iter++) {
    std::cout << *iter << std::endl;
  }
  auto le_pred = new_graph.pred("Bob");
  std::cout << "Bob's pred :" << std::endl;
  for (auto iter = le_pred.begin(); iter != le_pred.end(); iter++) {
    std::cout << *iter << std::endl;
  }
  auto le_pred_james = new_graph.pred("James");
  std::cout << "James' pred :" << std::endl;
  for (auto iter = le_pred_james.begin(); iter != le_pred_james.end(); iter++) {
    std::cout << *iter << std::endl;
  }
  auto le_pred_dan = new_graph.pred("Dan");
  std::cout << "Dan's pred :" << std::endl;
  for (auto iter = le_pred_dan.begin(); iter != le_pred_dan.end(); iter++) {
    std::cout << *iter << std::endl;
  }
}