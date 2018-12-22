import numpy as np
import re
import sys
import time

filename = "../instances/p"


class CLFPSolver:
    def __init__(self):
        self.numOfFacility, self.numOfCustomer = 0, 0
        self.capacityOfEachFacility, self.openingCostOfEachFacility = [], []
        self.demandOfCustomer, self.assignmentCost = [], []
        self.customerAssignedTable, self.actualCapacity = [], []
        self.solutionCost, self.assignSolution, self.openingSolution = 0, [], []

    def loadData(self, filename):
        count = 2
        with open(filename) as file:
            texts = re.split(r'\s+', file.read().strip())
            self.numOfFacility, self.numOfCustomer = int(texts[0]), int(texts[1])
            for i in range(self.numOfFacility):
                self.capacityOfEachFacility.append(int(texts[count]))
                self.openingCostOfEachFacility.append(int(texts[count + 1]))
                count = count + 2
            self.demandOfCustomer = np.array(texts[count:count+self.numOfCustomer], dtype=int)
            count = count + self.numOfCustomer
            self.assignmentCost = np.array(texts[count:(count+self.numOfCustomer * self.numOfFacility)], dtype=int)\
                                    .reshape(self.numOfFacility, self.numOfCustomer).T
        self.customerAssignedTable = [-1] * self.numOfCustomer
        self.actualCapacity = [0] * self.numOfFacility
        self.totalCost = 0

    def debug(self):
        print(self.numOfFacility, self.numOfCustomer)
        print(self.capacityOfEachFacility, self.openingCostOfEachFacility)
        print(self.demandOfCustomer, self.assignmentCost)
        print(self.customerAssignedTable, self.actualCapacity)
        print(self.solutionCost, self.assignSolution, self.openingSolution)

    def printf(self):
        print(self.solutionCost, self.assignSolution, self.openingSolution)

    def SolutionGenerator(self):
        self.actualCapacity = [0] * self.numOfFacility
        for i in range(self.numOfCustomer):
            if not self.assignCustomerToFicility(i, 0):
                return False
        return True

    def calculationCost(self):
        self.totalCost = 0
        for i in range(self.numOfFacility):
            if self.actualCapacity[i] > 0:
                self.totalCost += self.openingCostOfEachFacility[i]
        for i in range(self.numOfCustomer):
            self.totalCost += self.assignmentCost[i][self.customerAssignedTable[i]]
        return self.totalCost

    def acceptNewSolution(self):
        self.openingSolution = self.actualCapacity[:]
        self.assignSolution = self.customerAssignedTable[:]
        self.solutionCost = self.totalCost
    
    def refuseNewSolution(self):
        self.actualCapacity = self.openingSolution[:]
        self.customerAssignedTable = self.assignSolution[:]
        self.totalCost = self.solutionCost

    def assignCustomerToFicility(self, customer, facility):
        tempCAT, tempAC = self.customerAssignedTable[customer], 0
        if tempCAT != -1:
            tempAC = self.actualCapacity[tempCAT]
            self.actualCapacity[tempCAT] -= self.demandOfCustomer[customer]
            assert self.actualCapacity[tempCAT] >= 0, f"{customer} {self.demandOfCustomer[customer]} {tempCAT} {tempAC}"
        k = 0
        for i in range(self.numOfFacility):
            k = np.mod(facility + i, self.numOfFacility)
            temp = self.actualCapacity[k] + self.demandOfCustomer[customer]
            if temp <= self.capacityOfEachFacility[k]:
                self.customerAssignedTable[customer] = k
                self.actualCapacity[k] = temp
                return True
        if tempCAT != -1:
            self.actualCapacity[tempCAT] = tempAC
        self.customerAssignedTable[customer] = tempCAT
        print(tempCAT, tempAC)
        return False
    
    def disturbance(self, t):
        i = np.random.randint(self.numOfCustomer)
        j = np.random.randint(self.numOfFacility)
        temp = self.customerAssignedTable[i]
        if self.assignCustomerToFicility(i, j):
            dE = self.calculationCost() - self.solutionCost
            if dE <= 0 or np.random.rand() < np.exp(-dE / t):
                self.acceptNewSolution()
            else:
                k = self.customerAssignedTable[i]
                self.actualCapacity[k] = self.openingSolution[k]
                self.actualCapacity[temp] = self.openingSolution[temp]
                self.customerAssignedTable[i] = self.assignSolution[i]

    def SASolution(self):
        while not self.SolutionGenerator():
            continue
        self.calculationCost()
        self.acceptNewSolution()
        t, a, MapkobChainLength= 1000.0, 0.99, self.numOfCustomer * self.numOfFacility
        count, lastSolution = 0, self.totalCost
        while t > 0.01 and count < 20:
            for _ in range(MapkobChainLength):
                self.disturbance(t)
            if lastSolution == self.solutionCost:
                count = count + 1
            else:
                count = 0
            t = a * t
            self.printf()

    def a(self):
        self.customerAssignedTable = [ 18, 19, 21, 27, 2, 12, 21, 27, 18, 28, 4, 28, 13, 13, 28, 18, 13, 20, 27, 0, 18, 16, 23, 24, 10, 18, 27, 19, 29, 2, 19, 19, 4, 16, 27, 6, 20, 9, 24, 16, 18, 9, 6, 4, 20, 19, 13, 21, 19, 14, 24, 16, 24, 16, 20, 6, 11, 9, 18, 25, 29, 0, 7, 20, 13, 11, 23, 14, 28, 25, 18, 12, 19, 9, 23, 23, 20, 23, 20, 16, 10, 0, 9, 25, 29, 13, 6, 2, 27, 7, 28, 6, 25, 25, 2, 13, 11, 6, 0, 28, 28, 21, 25, 18, 4, 0, 4, 24, 27, 13, 19, 4, 27, 12, 12, 20, 28, 7, 14, 20, 9, 11, 28, 19, 9, 16, 2, 11, 19, 0, 28, 24, 25, 20, 23, 9, 13, 13, 28, 10, 12, 27, 11, 11, 25, 2, 24, 21, 27, 25, 24, 0, 23, 0, 20, 11, 2, 16, 13, 13, 13, 27, 10, 24, 23, 27, 23, 4, 13, 21, 23, 20, 19, 14, 4, 2, 0, 10, 29, 10, 2, 21, 2, 19, 12, 9, 28, 23, 0, 21, 2, 21, 6, 4, 7, 12, 7, 21, 10, 19]
        print(len(self.customerAssignedTable))
        self.actualCapacity = [0] * self.numOfFacility
        for i in range(self.numOfCustomer):
            self.actualCapacity[self.customerAssignedTable[i]] += self.demandOfCustomer[i]
        self.calculationCost()
        self.acceptNewSolution()
        self.printf()

def main():
    solve = CLFPSolver()
    # start = 0.0
    # start = time.perf_counter()
    solve.loadData(filename + str(71))
    # solve.SASolution()
    # print(f"p: runtime {time.process_time() - start}")
    # for i in range(1, 71):
    #     start = time.process_time()
    #     print(f"p{i}: runtime {time.process_time() - start}")
    # solve.debug()
    # solve.printf()
    solve.a()

if __name__ == "__main__":
    main()
