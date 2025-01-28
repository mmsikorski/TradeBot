#include "websocketpp/config/asio_client.hpp"
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <iostream>
#include <string>
#include <json/json.h> // Include a JSON library like JSON for Modern C++ or JsonCpp
#include <chrono>
#include <ctime>
#include "handler/StockExchangeHandler.h"

int main() {
    StockExchangeHandler connector;
    connector.connect("wss://ws.kraken.com"); // Kraken WebSocket endpoint
    return 0;
}
