//
// Created by dev on 10/01/2025.
//

#include "StockDataStorageInMemory.h"

void StockDataStorageInMemory::add(double d) {
   executed_trades.push_back(d);
}

StockDataStorageInMemory::StockDataStorageInMemory() {

}
