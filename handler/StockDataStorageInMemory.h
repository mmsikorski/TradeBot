//
// Created by dev on 10/01/2025.
//

#ifndef MARKETMAKING_STOCKDATASTORAGEINMEMORY_H
#define MARKETMAKING_STOCKDATASTORAGEINMEMORY_H

#include <vector>

class StockDataStorageInMemory {
private:
    std::vector<double> executed_trades;
public:
    StockDataStorageInMemory();

void add(double d);


};


#endif //MARKETMAKING_STOCKDATASTORAGEINMEMORY_H
