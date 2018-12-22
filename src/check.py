import numpy as np
import re

checkFile = "SAout.txt"
filePath = "../instances/"

class CLFPChecker:
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

    def calculationCost(self):
        self.totalCost = 0
        for i in range(self.numOfFacility):
            if self.actualCapacity[i] > 0:
                self.totalCost += self.openingCostOfEachFacility[i]
        for i in range(self.numOfCustomer):
            self.totalCost += self.assignmentCost[i][self.customerAssignedTable[i]]
        return self.totalCost

    def check(self):
        # self.customerAssignedTable = [ 18, 19, 21, 27, 2, 12, 21, 27, 18, 28, 4, 28, 13, 13, 28, 18, 13, 20, 27, 0, 18, 16, 23, 24, 10, 18, 27, 19, 29, 2, 19, 19, 4, 16, 27, 6, 20, 9, 24, 16, 18, 9, 6, 4, 20, 19, 13, 21, 19, 14, 24, 16, 24, 16, 20, 6, 11, 9, 18, 25, 29, 0, 7, 20, 13, 11, 23, 14, 28, 25, 18, 12, 19, 9, 23, 23, 20, 23, 20, 16, 10, 0, 9, 25, 29, 13, 6, 2, 27, 7, 28, 6, 25, 25, 2, 13, 11, 6, 0, 28, 28, 21, 25, 18, 4, 0, 4, 24, 27, 13, 19, 4, 27, 12, 12, 20, 28, 7, 14, 20, 9, 11, 28, 19, 9, 16, 2, 11, 19, 0, 28, 24, 25, 20, 23, 9, 13, 13, 28, 10, 12, 27, 11, 11, 25, 2, 24, 21, 27, 25, 24, 0, 23, 0, 20, 11, 2, 16, 13, 13, 13, 27, 10, 24, 23, 27, 23, 4, 13, 21, 23, 20, 19, 14, 4, 2, 0, 10, 29, 10, 2, 21, 2, 19, 12, 9, 28, 23, 0, 21, 2, 21, 6, 4, 7, 12, 7, 21, 10, 19]
        self.actualCapacity = [0] * self.numOfFacility
        for i in range(self.numOfCustomer):
            self.actualCapacity[self.customerAssignedTable[i]] += self.demandOfCustomer[i]
        self.calculationCost()
    
    def getSize(self):
        return self.numOfCustomer, self.numOfFacility
    
    def getResult(self):
        return self.actualCapacity, self.totalCost
    
    def setTable(self, t):
        self.customerAssignedTable = t
    
    def getCapacity(self):
        return self.capacityOfEachFacility

def main():
    count = 0
    with open(checkFile, encoding="UTF-16 LE") as file:
        text = re.split(r'[^a-zA-Z0-9]+', file.read().strip())[1:]
        k = len(text)
        while count < k:
            filename = filePath + text[count]
            expectValue = np.int(text[count + 1])
            solver = CLFPChecker()
            solver.loadData(filename)
            numOfCustomer, numOfFacility = solver.getSize()
            solver.setTable(np.array(text[count+2+numOfFacility:count+2+numOfFacility+numOfCustomer], dtype=int))
            solver.check()
            expectActualCapacity = np.array(text[count+2:count+2+numOfFacility], dtype=int)
            count = count + 3 + numOfCustomer + numOfFacility
            actualCapacity, totalCost = solver.getResult()
            actualCapacity=np.array(actualCapacity)
            actualCapacity[actualCapacity>0]=1
            assert (expectActualCapacity == actualCapacity).all()
            assert totalCost == expectValue
            assert (expectActualCapacity <= solver.getCapacity()).all()

if __name__ == "__main__":
    main()