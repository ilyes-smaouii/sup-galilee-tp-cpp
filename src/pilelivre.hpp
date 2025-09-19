#pragma once

#include <cstddef>
#include <list>
#include <string>

#include "graph.hpp"

namespace pilelivre {
  
  struct Data{
    virtual ~Data() = default;
  };
  
  struct Action : public Data {};
  
  struct Person;
  
  struct FriendAdd : public Action {
    Person *m_person{};
    
    FriendAdd(Person* new_friend_ptr);
  };
  
  struct FriendRemove : public Action {
    Person *m_person{};
    
    FriendRemove(Person* old_friend_ptr);
  };
  
  struct Person : public Data {
    std::string m_name{};
    std::list<Action*> m_action{};
    std::list<Person*> m_friends{};
    
    void add_action(Action* action_ptr);
    // void add_action(FriendAdd* action_ptr);
    // void add_action(FriendRemove* action_ptr);
  
    Person(std::string name);
  };
  
  struct MsgPublic : public Action {
    std::string m_msg{};
    
    MsgPublic(std::string msg);
  };
  
  struct MsgPrivate : public Action {
    std::string m_msg{};
    Person* m_dest{};
    
    MsgPrivate(std::string msg, Person* dest_ptr);
  };
  
  struct CatPropaganda : public Action {
    std::string m_url{};
    
    CatPropaganda(std::string url);
  };
  
  struct PileLivre_v1 {
    graph::Graph<std::string, Data*, int> m_content{};
    std::list<Data*> m_data{}; // make sure we de-allocate properly at destruction of PileLivre_v1 instance
    std::size_t m_counter_msg{}; // compteur pour les messages
    std::size_t m_counter_cat{}; // compteur pour les vidéos de chats
    std::size_t m_counter_friend{}; // compteur pour les actions concernant les amis
    
    /*
    Doesn't do much
    */
    PileLivre_v1();
    ~PileLivre_v1();
    void add_person(std::string name);
    /*
    "action/msg/{counter}"
    */
    void add_msg_public(std::string name, std::string msg);
    /*
    "action/msg/{counter}"
    */
    void add_msg_private(std::string writer, std::string reader, std::string msg);
    /*
    "action/cat/{counter}"
    */
    void add_cat(std::string name, std::string url);
    /*
    "action/friend/{counter}"
    */
    void add_friend(std::string name, std::string _friend);
    /*
    "action/friend/{counter}"
    */
    void rm_friend(std::string name, std::string friend_no_more);
  };

  struct PileLivre_v2 {
    graph::Graph<std::string, Data*, void> m_content{};
    std::list<Data*> m_data{}; // make sure we de-allocate properly at destruction of PileLivre_v2 instance
    std::size_t m_counter_msg{}; // compteur pour les messages
    std::size_t m_counter_cat{}; // compteur pour les vidéos de chats
    std::size_t m_counter_friend{}; // compteur pour les actions concernant les amis
    
    /*
    Doesn't do much
    */
    PileLivre_v2();
    ~PileLivre_v2();
    void add_person(std::string name);
    /*
    "action/msg/{counter}"
    */
    void add_msg_public(std::string name, std::string msg);
    /*
    "action/msg/{counter}"
    */
    void add_msg_private(std::string writer, std::string reader, std::string msg);
    /*
    "action/cat/{counter}"
    */
    void add_cat(std::string name, std::string url);
    /*
    "action/friend/{counter}"
    */
    void add_friend(std::string name, std::string _friend);
    /*
    "action/friend/{counter}"
    */
    void rm_friend(std::string name, std::string friend_no_more);
  };

}
