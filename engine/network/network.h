#pragma once
#include "server.h"
#include "client.h"
#include <iostream>
#define LOG(msg)std::cout << msg
namespace Net {
#define IP_STREAM(IP) (IP & 0xFF) << '.' << ((IP >> 8) & 0xFF) << '.' << ((IP >> 16) & 0xFF) << '.' << ((IP >> 24) & 0xFF)

    void Initialize();
}
