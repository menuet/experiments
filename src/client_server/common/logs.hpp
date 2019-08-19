
#pragma once

#include <iostream>

#define CS_LOG(level, logger, args) std::cout << #level << " - " << #logger << " : " << args << "\n"
