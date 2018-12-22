#ifndef _CLFPSOLVER_HPP_
#define _CLFPSOLVER_HPP_

#include <vector>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cmath>

#define MAX_NUMOFFACILITY 100
#define MAX_CUSTOMER 1000
#define MAX_INDIVIDUAL_NUM 100

using std::vector;
using std::ios;
using std::cin;

struct Individual {
  // 顾客的分配表
  int assignTable[MAX_CUSTOMER];
  // 设备当前被使用的容量
  int capacityTable[MAX_NUMOFFACILITY];
  int totalCost;
};

class CLFPSolver {
  int numOfFacility, numOfCustomer, solutionCost, totalCost;
  // 每个设备的容量
  int capacityOfEachFacility[MAX_NUMOFFACILITY];
  // 每个设备的开放开销
  int openingCostOfEachFacility[MAX_NUMOFFACILITY];
  // 每个顾客的需求
  int demandOfCustomer[MAX_CUSTOMER];
  // 分配顾客的开销
  int assignmentCost[MAX_CUSTOMER * MAX_NUMOFFACILITY];
  // 种群
  Individual population[MAX_INDIVIDUAL_NUM];
public:
  CLFPSolver() {
    srand(time(NULL));
    memset(this, 0, sizeof(CLFPSolver));
  }
  int loadData(const char *filename) {
    FILE *file = freopen(filename, "r", stdin);
    if (file == 0) {
      return 1;
    }
    cin >> numOfFacility >> numOfCustomer;
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; ++i) {
      memset(population[i].capacityTable, 0, sizeof(int) * numOfFacility);
      memset(population[i].assignTable, -1, sizeof(int) * numOfCustomer);
      totalCost = 0;
    }
    for (int i = 0; i < numOfFacility; ++i) {
      cin >> capacityOfEachFacility[i] >> openingCostOfEachFacility[i];
    }
    for (int i = 0; i < numOfCustomer; ++i) {
      cin >> demandOfCustomer[i];
    }
    for (int i = 0; i < numOfFacility; ++i) {
      for (int j = 0; j < numOfCustomer; ++j) {
        cin >> assignmentCost[i * numOfCustomer + j];
      }
    }
    fclose(file);
  }
  void display() {
    printf("%d %d\n", numOfFacility, numOfCustomer);
    displayArray(demandOfCustomer, numOfCustomer);
    displayArray(capacityOfEachFacility, numOfFacility);
    displayArray(openingCostOfEachFacility, numOfFacility);
    displayArray(assignmentCost, numOfCustomer * numOfFacility);
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; ++i)
      displayIndividual(population[i]);
  }
  void displayBestSolution() {
    Individual *p = &population[0];
    for (int i = 1; i < MAX_INDIVIDUAL_NUM; ++i) {
      if (population[i].totalCost < p->totalCost) {
        p = &population[i];
      }
    }
    displayIndividual(*p);
  }
  void displayIndividual(Individual &a) {
    printf("%d\n", a.totalCost);
    for (int i = 0; i < numOfFacility; ++i) {
      a.capacityTable[i] = a.capacityTable[i] > 0;
    }
    displayArray(a.capacityTable, numOfFacility);
    displayArray(a.assignTable, numOfCustomer);
  }
  void displayArray(int *a, int size) {
    printf("[ ");
    for (int i = 0; i < size - 1; ++i) {
      printf("%d, ", a[i]);
    }
    printf("%d]\n", a[size - 1]);
  }

  bool assignCustomerToFicility(Individual &individual,int customer, int facility) {
    int tempCAT = individual.assignTable[customer];
    int temp = 0, tempAC = 0, k = 0;
    if (tempCAT != -1) {
      tempAC = individual.capacityTable[tempCAT];
      individual.capacityTable[tempCAT] -= demandOfCustomer[customer];
    }
    for (int i = 0; i < numOfFacility; ++i) {
      k = (facility + i) % numOfFacility;
      temp = individual.capacityTable[k] + demandOfCustomer[customer];
      if (temp <= capacityOfEachFacility[k]) {
        individual.assignTable[customer] = k;
        individual.capacityTable[k] = temp;
        return true;
      }
    }
    if (tempCAT != -1) {
      individual.capacityTable[tempCAT] = tempAC;
    }
    individual.assignTable[customer] = tempCAT;
    return false;
  }

  bool initSolutionGenerator() {
    bool isSuccess = true;
    int count = 0, maxCount = 3;
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; ++i) {
      isSuccess = true;
      for (int j = 0; j < numOfCustomer; ++j) {
        if (!assignCustomerToFicility(population[i], j, rand() % numOfFacility)) {
          isSuccess = false;
          break;
        }
      }
      if (!isSuccess) {
        --i;
        count = count + 1;
      } else {
        count = 0;
        calculationCost(population[i]);
      }
      if (count >= 3) {
        return false;
      }
    }
    return true;
  }

  int calculationCost(Individual &a) {
    a.totalCost = 0;
    for (int i = 0; i < numOfFacility; ++i) {
      if (a.capacityTable[i]) {
        a.totalCost = a.totalCost + openingCostOfEachFacility[i];
      }
    }
    for (int i = 0; i < numOfCustomer; ++i) {
      a.totalCost = a.totalCost + assignmentCost[a.assignTable[i] * numOfCustomer + i];
    }
    return a.totalCost;
  }

  void select() {
    Individual temp[MAX_INDIVIDUAL_NUM];
    memcpy(temp, population, sizeof(population));
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; ++i) {
      int x = rand() % MAX_INDIVIDUAL_NUM;
      int y = rand() % MAX_INDIVIDUAL_NUM;
      x = population[x].totalCost < population[y].totalCost ? x : y;
      memcpy(&population[i], &temp[x], sizeof(Individual));
    }
  }

  void crossover() {
    int x = rand() % (numOfCustomer - 1);
    int y = rand() % (numOfCustomer - x - 1) + x + 1;
    double Pc = 0.8;
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; i = i + 2) {
      if (rand() / (RAND_MAX + 1.0) < Pc) {
        for (int j = x; j < y; ++j) {
          assignCustomerToFicility(population[i], j, population[i + 1].assignTable[j]);
        }
        for (int j = x; j < y; ++j) {
          assignCustomerToFicility(population[i + 1], j, population[i].assignTable[j]);
        }
      }
    }
  }

  void mutation() {
    double Pm = 0.1;
    for (int i = 0; i < MAX_INDIVIDUAL_NUM; ++i) {
      if (rand() / (RAND_MAX + 1.0) < Pm) {
        int k = rand() % numOfCustomer;
        int kk = rand() % numOfFacility;
        assignCustomerToFicility(population[i], k, kk);
      }
      calculationCost(population[i]);
    }
  }

  void GASolution() {
    initSolutionGenerator();
    int maxGeneration = 5000, count = 0;
    while (count < maxGeneration) {
      select();
      crossover();
      mutation();
      count = count + 1;
    }
    displayBestSolution();
  }
};

#endif