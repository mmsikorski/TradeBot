cmake_minimum_required(VERSION 3.20)
project(MarketMaking)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the executable
add_executable(MarketMaking
        main.cpp
        handler/StockExchangeHandler.cpp
        handler/StockDataStorageInMemory.cpp
        handler/StockDataStorageInMemory.h
)

# Dependencies

# Boost
find_package(Boost 1.85 REQUIRED COMPONENTS system thread)
target_include_directories(MarketMaking PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(MarketMaking PRIVATE Boost::system Boost::thread pthread)

# WebSocket++
set(HOMEBREW_INCLUDE /opt/homebrew/include)
target_include_directories(MarketMaking PRIVATE ${HOMEBREW_INCLUDE}/websocketpp)
target_compile_definitions(MarketMaking PRIVATE ASIO_STANDALONE)

# JSONCPP
find_path(JSONCPP_INCLUDE_DIR json/json.h PATHS ${HOMEBREW_INCLUDE})
find_library(JSONCPP_LIBRARY NAMES jsoncpp PATHS /opt/homebrew/lib)

# Add JsonCpp from source if not found
if (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)
    target_include_directories(MarketMaking PRIVATE ${JSONCPP_INCLUDE_DIR})
    target_link_libraries(MarketMaking PRIVATE ${JSONCPP_LIBRARY})
else()
    # Use JsonCpp as part of the project (assumes you cloned it into the project directory)
    set(JSONCPP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/jsoncpp)
    if (EXISTS ${JSONCPP_SOURCE_DIR}/include/json/json.h)
        message(STATUS "Using JsonCpp from source.")
        add_subdirectory(${JSONCPP_SOURCE_DIR} jsoncpp)
        target_include_directories(MarketMaking PRIVATE ${JSONCPP_SOURCE_DIR}/include)
        target_link_libraries(MarketMaking PRIVATE jsoncpp_lib)
    else()
        message(FATAL_ERROR "JSONCPP library not found. Please check your installation or clone JsonCpp into 'external/jsoncpp'.")
    endif()
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)
target_include_directories(MarketMaking PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(MarketMaking PRIVATE OpenSSL::SSL OpenSSL::Crypto)
