#include "dice.h"
#include <iostream>

int main(void){
  dice d1(3, 5, 6);
  dice d2(100,1000,100000);
  dice d3;

  d3.parse_dice("23+100d700");

  std::cout<<d1.form<<std::endl;
  std::cout<<d2.form<<std::endl;
  std::cout<<d3.form<<std::endl;
  std::cout<<d3.base<<std::endl;
  std::cout<<d3.num<<std::endl;
  std::cout<<d3.sides<<std::endl;


  return 0;
}