#include "CLFPSolver.hpp"

int main() {
  std::string str("../../instances/p");
  CLFPSolver solver;
  int i = 0;
  for (int i = 0; i < 71; ++i) {
    printf("p%d: ", i + 1);
    solver.loadData((str + std::to_string(i + 1)).c_str());
    clock_t start = clock();
    solver.GASolution();
    printf("%dms\n\n", clock() - start);
  }
  return 0;
}