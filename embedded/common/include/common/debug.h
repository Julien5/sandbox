#pragma once

#include "common/rusttypes.h"

#if defined(ARDUINO)
void serialprint(const char *buffer);
void serialprint(int x);
void serialflush();
#endif

#if defined(PC)
#define LOG(...)                                       \
    do {                                               \
        std::unique_lock<std::mutex> lock(stdout_mtx); \
        printf(__VA_ARGS__);                           \
        fflush(stdout);                                \
    } while (0)
#elif defined(ARDUINO)
#define LOG(...)                                       \
    do {                                               \
        char buffer[64];                               \
        snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        serialprint(buffer);                           \
        serialflush();                                 \
    } while (0)

#elif defined(ESP8266)
#define LOG(...)                               \
    do {                                       \
        printf("%s:%d: ", __FILE__, __LINE__); \
        printf(__VA_ARGS__);                   \
    } while (0)

#endif

#if defined(NDEBUG)
#define DBG(...) ((void)0)
#define TRACE() ((void)0)
#define PLOT(...) ((void)0)
#define assert(ignore) ((void)0)

#else

#if defined(PC)

#include <cassert>
#include <stdio.h>
#include <mutex>
#include <thread>
#include <map>

#include <string.h>
#define __BASENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static std::mutex stdout_mtx;
int thread_index();
#define DBG(...)                                                                           \
    do {                                                                                   \
        std::unique_lock<std::mutex> lock(stdout_mtx);                                     \
        printf("[%d] %s:%d:%s(): ", thread_index(), __BASENAME__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__);                                                               \
        fflush(stdout);                                                                    \
    } while (0)
//#define PLOT(...) ((void)0)

#define PLOT(...)                                                           \
    do {                                                                    \
        std::unique_lock<std::mutex> lock(stdout_mtx);                      \
        printf("%s:%s:%d: gnuplot:", __BASENAME__, __FUNCTION__, __LINE__); \
        printf(__VA_ARGS__);                                                \
        fflush(stdout);                                                     \
    } while (0)

#define TRACE()                                                                                     \
    do {                                                                                            \
        std::unique_lock<std::mutex> lock(stdout_mtx);                                              \
        printf("[%d] %s:%d:%s(): TRACE\r\n", thread_index(), __BASENAME__, __LINE__, __FUNCTION__); \
        fflush(stdout);                                                                             \
    } while (0)

#elif defined(ARDUINO)
/*
 * note: in debug mode, DBGTX takes memory.
 */
#define DBG(...)                                       \
    do {                                               \
        char buffer[64];                               \
        snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        serialprint(buffer);                           \
        serialflush();                                 \
    } while (0)
#define TRACE()                 \
    do {                        \
        serialprint(__FILE__);  \
        serialprint(":");       \
        serialprint(__LINE__);  \
        serialprint(": TRACE"); \
        serialflush();          \
    } while (0)
#define assert(x)                        \
    do {                                 \
        if (!(x)) {                      \
            serialprint(__FILE__);       \
            serialprint(":");            \
            serialprint(__LINE__);       \
            serialprint(": ASSERT\r\n"); \
            while (1) {                  \
                serialflush();           \
            };                           \
        }                                \
    } while (0)
#define PLOT(...) ((void)0)
#elif defined(ESP8266)
#include <cassert>
#include <stdio.h>
#define DBG(...)                               \
    do {                                       \
        printf("%s:%d: ", __FILE__, __LINE__); \
        printf(__VA_ARGS__);                   \
    } while (0)
#define TRACE()                                      \
    do {                                             \
        printf("%s:%d TRACE\n", __FILE__, __LINE__); \
    } while (0)
#ifndef assert
#define assert(x)                                                      \
    do {                                                               \
        if (!(x)) {                                                    \
            DBG("\r\n%s:%d assertion failed\r\n", __FILE__, __LINE__); \
            while (1) {                                                \
            };                                                         \
        }                                                              \
    } while (0)
#define PLOT(...) ((void)0)
#endif
#endif
#endif
namespace debug {
    //! necessary for Serial.print in Arduino
    void init_serial();
    void turnBuildinLED(bool on);
    void address_range(const char *, void *, size_t L);
    int freeMemory();
}
