//
// Created by dev on 08/01/2025.
//

#include "StockExchangeHandler.h"

void StockExchangeHandler::connect(const std::string &uri) {
    websocketpp::lib::error_code ec;

    client::connection_ptr con = c.get_connection(uri, ec);

    if(ec) {
        std::cerr << "Connection initialization error: " << ec . message()  << std::endl;
        return;
    }

    c.connect(con);
    websocketpp::lib::thread asio_thread(&client::run, &c);
    asio_thread.join();
}


#include <array>

void StockExchangeHandler::on_open(websocketpp::connection_hdl hdl) {
    std::cout << "Connection opened" << std::endl;
    std::array<int, 3> a = {1, 2, 3};
    Json::StreamWriterBuilder writer;

    // Trade Subscription
    Json::Value trade_subscription_message;
    trade_subscription_message["event"] = "subscribe";
    trade_subscription_message["pair"] = Json::Value(Json::arrayValue);
    trade_subscription_message["pair"].append("XBT/USD"); // Replace with desired pair

    Json::Value trade_subscription;
    trade_subscription["name"] = "trade";
    trade_subscription_message["subscription"] = trade_subscription;

    std::string trade_message = Json::writeString(writer, trade_subscription_message);
    c.send(hdl, trade_message, websocketpp::frame::opcode::text);

    // Order Book Subscription
    Json::Value orderbook_subscription_message;
    orderbook_subscription_message["event"] = "subscribe";
    orderbook_subscription_message["pair"] = Json::Value(Json::arrayValue);
    orderbook_subscription_message["pair"].append("XBT/USD");

    Json::Value orderbook_subscription;
    orderbook_subscription["name"] = "book";
    orderbook_subscription["depth"] = 10;
    orderbook_subscription_message["subscription"] = orderbook_subscription;

    std::string orderbook_message = Json::writeString(writer, orderbook_subscription_message);
    c.send(hdl, orderbook_message, websocketpp::frame::opcode::text);

    // Order Events Subscription (requires authentication)
    // Uncomment and implement generateKrakenSignature for this to work
    /*
    Json::Value private_subscription_message;
    private_subscription_message["event"] = "subscribe";
    private_subscription_message["subscription"]["name"] = "ownTrades"; // or "openOrders"
    private_subscription_message["subscription"]["token"] = generateKrakenSignature(nonce, api_key, api_secret);

    std::string private_message = Json::writeString(writer, private_subscription_message);
    c.send(hdl, private_message, websocketpp::frame::opcode::text);
    */
}

void StockExchangeHandler::on_message(websocketpp::connection_hdl, client::message_ptr msg) {

    // Parse the message payload as JSON
    Json::Value json_message;
    Json::CharReaderBuilder reader;
    std::string errors;

    // Create a proper istringstream from the payload
    std::string payload = msg->get_payload();
    std::istringstream payload_stream(payload);

    // Parse the JSON
    if (!Json::parseFromStream(reader, payload_stream, &json_message, &errors)) {
        std::cerr << "Failed to parse message: " << errors << std::endl;
        return;
    }

    // Check if this is a trade message
    if (json_message.isArray() && json_message.size() >= 4 && json_message[2].asString() == "trade") {
        const Json::Value& trades = json_message[1]; // Array of trades

        for (const auto& trade : trades) {
            // Extract fields as strings first
            std::string price_str = trade[0].asString(); // Trade price
            std::string volume_str = trade[1].asString(); // Trade volume
            std::string timestamp_str = trade[2].asString(); // UNIX timestamp
            std::string side = trade[3].asString(); // Buy ("b") or Sell ("s")
            std::string order_type = trade[4].asString(); // Market ("m") or Limit ("l")

            // Convert strings to doubles where necessary
            double price = std::stod(price_str); // Convert price to double

            storageInMemory.add(price);

            double volume = std::stod(volume_str); // Convert volume to double
            double trade_timestamp = std::stod(timestamp_str); // Convert timestamp to double

            // Convert the trade UNIX timestamp to a readable date and time
            auto millis = static_cast<int64_t>(trade_timestamp * 1000);
            auto seconds = millis / 1000;
            auto ms = millis % 1000;

            std::time_t time_t_seconds = static_cast<std::time_t>(seconds);
            std::tm* tm = std::gmtime(&time_t_seconds); // Convert to GMT
            std::stringstream readable_trade_time;
            readable_trade_time << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "." << ms;

            // Get the current device timestamp
            auto now = std::chrono::system_clock::now();
            auto now_seconds = std::chrono::system_clock::to_time_t(now);
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            std::tm* local_tm = std::gmtime(&now_seconds); // Force UTC
            std::stringstream readable_device_time;
            readable_device_time << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << "." << now_ms.count();

            // Print the trade information
            std::cout << "Trade Details:" << std::endl;
            std::cout << "  Pair: " << json_message[3].asString() << std::endl;
            std::cout << "  Price: " << price << std::endl;
            std::cout << "  Volume: " << volume << std::endl;
            std::cout << "  Trade Timestamp: " << readable_trade_time.str() << " UTC" << std::endl;
            std::cout << "  Current Device Timestamp: " << readable_device_time.str() << " Local Time" << std::endl;
            std::cout << "  Side: " << (side == "b" ? "Buy" : "Sell") << std::endl;
            std::cout << "  Order Type: " << (order_type == "m" ? "Market" : "Limit") << std::endl;
            std::cout << "------------------------" << std::endl;
        }
    } else {
        std::cout << "Message received: " << payload << std::endl;
    }
}

void StockExchangeHandler::on_close(websocketpp::connection_hdl) {

    std::cout << "Connection closed" << std::endl;
}

void StockExchangeHandler::on_fail(websocketpp::connection_hdl) {

    std::cerr << "Connection failed" << std::endl;
}

StockExchangeHandler::StockExchangeHandler() {


    // Initialize WebSocket++ client
    c.init_asio();

    // TLS handler for secure connections
    c.set_tls_init_handler([](websocketpp::connection_hdl) -> std::shared_ptr<asio::ssl::context> {
        auto ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
        try {
            ctx->set_options(asio::ssl::context::default_workarounds |
                             asio::ssl::context::no_sslv2 |
                             asio::ssl::context::no_sslv3 |
                             asio::ssl::context::single_dh_use);
        } catch (std::exception &e) {
            std::cerr << "TLS initialization error: " << e.what() << std::endl;
            throw;
        }
        return ctx;
    });

    c.set_open_handler(websocketpp::lib::bind(&StockExchangeHandler::on_open, this, websocketpp::lib::placeholders::_1));
    c.set_message_handler(websocketpp::lib::bind(&StockExchangeHandler::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    c.set_close_handler(websocketpp::lib::bind(&StockExchangeHandler::on_close, this, websocketpp::lib::placeholders::_1));
    c.set_fail_handler(websocketpp::lib::bind(&StockExchangeHandler::on_fail, this, websocketpp::lib::placeholders::_1));
}
