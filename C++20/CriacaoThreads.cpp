#include <iostream>
#include <thread>

void foo() {
  std::cout << "Executando na thread com função foo." << std::endl;
}

class Functor {
  public:
    void operator()() {
      std::cout << "Executando na thread com functor." << std::endl;
    }
};

int main() {
  Functor f;
  std::thread t1(foo); // Criação e execução imediata com função
  std::thread t2(f); // Criação e execução imediata com functor
  std::thread t3([]() { // Criação e execução imediata com lambda
    std::cout << "Executando na thread com lambda." << std::endl;
  });

  // ...

  t1.join(); // Aguarda o término da thread t1
  t2.join(); // Aguarda o término da thread t2
  t3.join(); // Aguarda o término da thread t3

  return 0;
}
