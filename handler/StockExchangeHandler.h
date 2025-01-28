//
// Created by dev on 08/01/2025.
//

#ifndef MARKETMAKING_STOCKEXCHANGEHANDLER_H
#define MARKETMAKING_STOCKEXCHANGEHANDLER_H

#include "websocketpp/config/asio_client.hpp"
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <json/json.h> // Include a JSON library like JSON for Modern C++ or JsonCpp
#include <chrono>
#include <ctime>
#include "StockDataStorageInMemory.h"

using client = websocketpp::client<websocketpp::config::asio_tls_client>;

class StockExchangeHandler {
private:
    client c;
    StockDataStorageInMemory storageInMemory;

    void on_open(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl, client ::message_ptr msg);
    void on_close(websocketpp::connection_hdl );
    void on_fail(websocketpp::connection_hdl);

public:
    StockExchangeHandler();

    void connect(const std::string &uri);

};


#endif //MARKETMAKING_STOCKEXCHANGEHANDLER_H
