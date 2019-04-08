#include "npc.hpp"

#include <iostream>

npc::npc() {}
npc::~npc() {}

void npc::print_fields() {
  std::cout << name << std::endl;
  std::cout << desc << std::endl;
  std::cout << symbol << std::endl;
  std::cout << color << std::endl;
  std::cout << speed.form
            << std::endl; // TODO: Create friend function to print dice.
  std::cout << abilities << std::endl;
  std::cout << hp.form << std::endl;
  std::cout << ad.form << std::endl;
  std::cout << rarity << std::endl;
}
