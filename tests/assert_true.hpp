#pragma once
#include <cassert>

// Улучшенный assert с сообщением
#define ASSERT_TRUE(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "Assertion failed: " << (msg) \
                      << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        } \
        assert(cond); \
    } while (0)
