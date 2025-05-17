#include <iostream>
#include <thread>
#include <chrono>

void tarefa(std::stop_token st, int i) {
  while (!st.stop_requested()) {
    std::cout << "Trabalhando...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  std::cout << "Thread " << i << " cancelada com segurança.\n";
}

int main() {
  std::jthread t1(tarefa,1);
  std::jthread t2(tarefa,2);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  t1.request_stop();
  t2.request_stop();
  return 0;
}
