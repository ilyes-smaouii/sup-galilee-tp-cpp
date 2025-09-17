#include <cstddef>
// #include <functional>
#include <list>
#include <stdexcept>
#include <string>
#include <format>

#include "pilelivre.hpp"

namespace pilelivre {

  Person::Person(std::string name) : m_name(name) {}
  
  void Person::add_action(Action* action_ptr) {
    // add action to person's list
    m_action.push_back(action_ptr);
    // Check if it's a Friend-related action
    auto friend_add_ptr = dynamic_cast<FriendAdd*>(action_ptr);
    if (friend_add_ptr != NULL) {
      // add friend to this' friends, and this to friend's friends
      m_friends.push_back(friend_add_ptr->m_person);
      friend_add_ptr->m_person->m_friends.push_back(this);
    }
    else {
      auto friend_remove_ptr = dynamic_cast<FriendRemove*>(action_ptr);
      if (friend_remove_ptr != NULL) {
        // remove friend from this' friends, and this from friend's friends
        m_friends.remove(friend_remove_ptr->m_person);
        friend_remove_ptr->m_person->m_friends.remove(this);
      }
    }
  }
  
  // void Person::add_action(FriendAdd* action_ptr) {
  //   m_action.push_back(action_ptr);
  //   m_friends.push_back(action_ptr->m_person);
  //   action_ptr->m_person->m_friends.push_back(this);
  // }
  
  // void Person::add_action(FriendRemove* action_ptr) {
  //   m_action.push_back(action_ptr);
  //   m_friends.remove(action_ptr->m_person);
  //   action_ptr->m_person->m_friends.remove(this);
  // }
  
  MsgPublic::MsgPublic(std::string msg) : m_msg(msg) {}
  
  MsgPrivate::MsgPrivate(std::string msg, Person* dest_ptr) : m_msg(msg), m_dest(dest_ptr) {}
  
  CatPropaganda::CatPropaganda(std::string url) : m_url(url) {}
  
  FriendAdd::FriendAdd(Person* new_friend_ptr) : m_person(new_friend_ptr) {}
  
  FriendRemove::FriendRemove(Person* old_friend_ptr) : m_person(old_friend_ptr) {}
  
  PileLivre_v1::PileLivre_v1() : m_content(), m_data(), m_counter_msg(0), m_counter_cat(0), m_counter_friend(0) {}

  PileLivre_v1::~PileLivre_v1() {
    for (auto data_ptr : m_data) {
      delete data_ptr;
    }
  }
  
  void PileLivre_v1::add_person(std::string name) {
    auto new_person_ptr = new Person(name);
    m_data.push_back(new_person_ptr);
    auto node_id = name;
    auto node_data = new_person_ptr;
    m_content.add_node(name, node_data);
  }
  
  void PileLivre_v1::add_msg_public(std::string name, std::string msg) {
    auto person_node_iter = m_content.m_nodes.find(name);
    if (person_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_public error : node with name {} not found in Graph nodes !", name));
    }
    auto person_ptr = dynamic_cast<Person*>(person_node_iter->second.m_data);
    if (person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_public error : found node with name {}, but it's not a person !", name));
    }
    auto new_msg_ptr = new MsgPublic(msg);
    person_ptr->add_action(new_msg_ptr);
    m_data.push_back(new_msg_ptr);
    // add msg node
    auto msg_id = std::format("action/msg/{}", m_counter_msg);
    m_content.add_node(msg_id, new_msg_ptr);
    m_counter_msg++;
  }
  
  void PileLivre_v1::add_msg_private(std::string writer, std::string reader, std::string msg) {
    auto writer_node_iter = m_content.m_nodes.find(writer);
    if (writer_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_private error : writer node with name {} not found in Graph nodes !", writer));
    }
    auto writer_person_ptr = dynamic_cast<Person*>(writer_node_iter->second.m_data);
    if (writer_person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_private error : found writer node with name {}, but it's not a person !", writer));
    }
    auto reader_node_iter = m_content.m_nodes.find(reader);
    if (reader_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_private error : reader node with name {} not found in Graph nodes !", reader));
    }
    auto reader_person_ptr = dynamic_cast<Person*>(reader_node_iter->second.m_data);
    if (reader_person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_msg_private error : found reader node with name {}, but it's not a person !", reader));
    }
    auto new_msg_ptr = new MsgPrivate(msg, reader_person_ptr);
    writer_person_ptr->add_action(new_msg_ptr);
    m_data.push_back(new_msg_ptr);
    // add msg node
    auto msg_id = std::format("action/msg/{}", m_counter_msg);
    m_content.add_node(msg_id, new_msg_ptr);
    m_counter_msg++;
  }
  
  void PileLivre_v1::add_cat(std::string name, std::string url) {
    auto person_node_iter = m_content.m_nodes.find(name);
    if (person_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_cat error : node with name {} not found in Graph nodes !", name));
    }
    auto person_ptr = dynamic_cast<Person*>(person_node_iter->second.m_data);
    if (person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_cat error : found node with name {}, but it's not a person !", name));
    }
    auto new_cat_ptr = new CatPropaganda(url);
    person_ptr->add_action(new_cat_ptr);
    m_data.push_back(new_cat_ptr);
    // add cat node
    auto msg_id = std::format("action/cat/{}", m_counter_cat);
    m_content.add_node(msg_id, new_cat_ptr);
    m_counter_cat++;
  }
  
  void PileLivre_v1::add_friend(std::string name, std::string _friend) {
    // Find person
    auto person_node_iter = m_content.m_nodes.find(name);
    if (person_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_friend error : node with name {} not found in Graph nodes !", name));
    }
    auto person_ptr = dynamic_cast<Person*>(person_node_iter->second.m_data);
    if (person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_friend error : found node with name {}, but it's not a person !", name));
    }
    // Find friend
    auto friend_node_iter = m_content.m_nodes.find(_friend);
    if (friend_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::add_friend error : friend node with name {} not found in Graph nodes !", _friend));
    }
    auto friend_person_ptr = dynamic_cast<Person*>(friend_node_iter->second.m_data);
    if (friend_person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::add_friend error : found reader node with name {}, but it's not a person !", _friend));
    }
    // Create FriendAdd Data
    auto new_friend_add_ptr = new FriendAdd(friend_person_ptr);
    person_ptr->add_action(new_friend_add_ptr);
    person_ptr->m_friends.push_back(friend_person_ptr);
    m_data.push_back(new_friend_add_ptr);
    // add friend node
    auto msg_id = std::format("action/friend/{}", m_counter_friend);
    m_content.add_node(msg_id, new_friend_add_ptr);
    m_counter_friend++;
  }
  
  void PileLivre_v1::rm_friend(std::string name, std::string friend_no_more) {
    // Find person
    auto person_node_iter = m_content.m_nodes.find(name);
    if (person_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::rm_friend error : node with name {} not found in Graph nodes !", name));
    }
    auto person_ptr = dynamic_cast<Person*>(person_node_iter->second.m_data);
    if (person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::rm_friend error : found node with name {}, but it's not a person !", name));
    }
    // Find friend
    auto friend_node_iter = m_content.m_nodes.find(friend_no_more);
    if (friend_node_iter == m_content.m_nodes.end()) {
      throw std::runtime_error(std::format("PileLivre_v1::rm_friend error : friend node with name {} not found in Graph nodes !", friend_no_more));
    }
    auto friend_person_ptr = dynamic_cast<Person*>(friend_node_iter->second.m_data);
    if (friend_person_ptr == NULL) {
      throw std::runtime_error(std::format("PileLivre_v1::rm_friend error : found reader node with name {}, but it's not a person !", friend_no_more));
    }
    // Create FriendRemove Data
    auto new_friend_remove_ptr = new FriendRemove(friend_person_ptr);
    person_ptr->add_action(new_friend_remove_ptr);
    person_ptr->m_friends.remove(friend_person_ptr);
    m_data.push_back(new_friend_remove_ptr);
    // add friend node
    auto msg_id = std::format("action/friend/{}", m_counter_friend);
    m_content.add_node(msg_id, new_friend_remove_ptr);
    m_counter_friend++;
  }
  
}
