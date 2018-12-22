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

using std::vector;
using std::ios;
using std::cin;

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
  // 顾客的分配表
  int customerAssignedTable[MAX_CUSTOMER];
  // 设备当前被使用的容量
  int actualCapacity[MAX_NUMOFFACILITY];
  // 分配的结果，表示当前接受的解
  int assignSolution[MAX_CUSTOMER];
  // 设备是否开放
  int openingSolution[MAX_NUMOFFACILITY];
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
    for (int i = 0; i < numOfFacility; ++i) {
      cin >> capacityOfEachFacility[i] >> openingCostOfEachFacility[i];
      actualCapacity[i] = 0;
      openingSolution[i] = 0;
    }
    for (int i = 0; i < numOfCustomer; ++i) {
      cin >> demandOfCustomer[i];
      customerAssignedTable[i] = -1;
      assignSolution[i] = -1;
    }
    for (int i = 0; i < numOfFacility; ++i) {
      for (int j = 0; j < numOfCustomer; ++j) {
        cin >> assignmentCost[i * numOfCustomer + j];
      }
    }
    totalCost = 0;
    fclose(file);
  }
  void display() {
    printf("%d %d\n", numOfFacility, numOfCustomer);
    displayArray(demandOfCustomer, numOfCustomer);
    displayArray(customerAssignedTable, numOfCustomer);
    displayArray(capacityOfEachFacility, numOfFacility);
    displayArray(actualCapacity, numOfFacility);
    displayArray(openingCostOfEachFacility, numOfFacility);
    displayArray(assignmentCost, numOfCustomer * numOfFacility);
  }
  void displayArray(int *a, int size) {
    printf("[ ");
    for (int i = 0; i < size - 1; ++i) {
      printf("%d, ", a[i]);
    }
    printf("%d]\n", a[size - 1]);
  }
  void displaySolution() {
    printf("%d\n", solutionCost);
    printf("[ ");
    for (int i = 0; i < numOfFacility - 1; ++i) {
      printf("%d, ", actualCapacity[i] > 0);
    }
    printf("%d]\n", actualCapacity[numOfFacility - 1] > 0);
    displayArray(assignSolution, numOfCustomer);
  }

  bool assignCustomerToFicility(int customer, int facility) {
    int tempCAT = customerAssignedTable[customer];
    int temp = 0, tempAC = 0, k = 0;
    if (tempCAT != -1) {
      tempAC = actualCapacity[tempCAT];
      actualCapacity[tempCAT] = actualCapacity[tempCAT] - demandOfCustomer[customer];
    }
    for (int i = 0; i < numOfFacility; ++i) {
      k = (facility + i) % numOfFacility;
      temp = actualCapacity[k] + demandOfCustomer[customer];
      if (temp <= capacityOfEachFacility[k]) {
        customerAssignedTable[customer] = k;
        actualCapacity[k] = temp;
        return true;
      }
    }
    if (tempCAT != -1) {
      actualCapacity[tempCAT] = tempAC;
    }
    customerAssignedTable[customer] = tempCAT;
    return false;
  }

  bool solutionGenerator() {
    for (int i = 0; i < numOfCustomer; ++i) {
      if (!assignCustomerToFicility(i, 0)) {
        return false;
      }
    }
    return true;
  }

  int calculationCost() {
    totalCost = 0;
    for (int i = 0; i < numOfFacility; ++i) {
      if (actualCapacity[i]) {
        totalCost = totalCost + openingCostOfEachFacility[i];
      }
    }
    for (int i = 0; i < numOfCustomer; ++i) {
      totalCost = totalCost + assignmentCost[customerAssignedTable[i] * numOfCustomer + i];
    }
    return totalCost;
  }

  void disturbance(double t) {
    int i = rand() % numOfCustomer;
    int j = rand() % numOfFacility;
    int temp = customerAssignedTable[i];
    if (assignCustomerToFicility(i, j)) {
      int dE = calculationCost() - solutionCost;
      int k = customerAssignedTable[i];
      if (dE <= 0 || rand() / (RAND_MAX + 1.0) < exp(-dE / t)) {
        openingSolution[k] = actualCapacity[k];
        openingSolution[temp] = actualCapacity[temp];
        assignSolution[i] = customerAssignedTable[i];
        solutionCost = totalCost;
      } else {
        actualCapacity[k] = openingSolution[k];
        actualCapacity[temp] = openingSolution[temp];
        customerAssignedTable[i] = assignSolution[i];
      }
    }
  }

  void acceptNewSolution() {
    for (int i = 0; i < numOfFacility; ++i) {
      openingSolution[i] = actualCapacity[i];
    }
    for (int i = 0; i < numOfCustomer; ++i) {
      assignSolution[i] = customerAssignedTable[i];
    }
    solutionCost = totalCost;
  }

  void SASolution() {
    while (!solutionGenerator());
    calculationCost();
    for (int i = 0; i < numOfCustomer; ++i) {
      assignSolution[i] = customerAssignedTable[i];
    }
    for (int i = 0; i < numOfFacility; ++i) {
      openingSolution[i] = actualCapacity[i];
    }
    solutionCost = totalCost;
    double t= 1000.0, a = 0.99;
    int MapkobChainLength = numOfCustomer * numOfFacility;
    int count = 0, lastCost = totalCost;
    while (t > 0.01 && count < 20) {
      for (int i = 0; i < MapkobChainLength; ++i) {
        disturbance(t);
      }
      if (lastCost == solutionCost) {
        count = count + 1;
      } else {
        count = 0;
      }
      t = a * t;
    }
  }
};

#endif