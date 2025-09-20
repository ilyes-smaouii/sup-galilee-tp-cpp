#include "automata.hpp"
#include <cassert>
#include <iostream>

automata::Automata start_ab() {
  automata::Automata new_auto{};
  new_auto.new_node();
  new_auto.new_node();
  new_auto.new_arc(0, 1, 'a');
  new_auto.new_arc(1, 2, 'b');
  new_auto.new_arc(2, 2, 'b');
  new_auto.new_arc(2, 2, 'a');
  new_auto.new_terminal(2);
  return new_auto;
}

void start_ab_check() {
  auto autom = start_ab();
  assert(autom.parse("abba"));
  assert(!autom.parse("baba"));
  assert(!autom.parse("abcba"));
}

int main(int argc, char *argv[]) {
  start_ab_check();

  auto autom = start_ab();

  if (argc > 1) {
    std::string word{argv[1]};
    auto checks_out = autom.parse(word);
    if (checks_out) {
      std::cout << "\"" << word << "\"" << " checks out !" << std::endl;
    } else {
      std::cout << "\""<< word << "\"" << " doesn't check out !" << std::endl;
    }
  }
}