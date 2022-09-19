#pragma once

#define LITTLE_ENDIAN 1

#define LUA_MAJOR_VERSION 5
#define LUA_MINOR_VERSION 3


#if 0
    #include <stdexcept>
    #define THROW_RUNTIME_ERROR(...) throw std::runtime_error(__VA_ARGS__)
#else
    #include <iostream>
    #define THROW_RUNTIME_ERROR(...) std::cerr<<__VA_ARGS__<<std::endl
#endif


#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#define CINT_SIZE 4
#define CSIZET_SIZE 8
#define INSTRUCTION_SIZE 4
#define LUA_INTEGER_SIZE 8
#define LUA_NUMBER_SIZE 8

#if CINT_SIZE==4
    #define cint int32_t
#else
    #define cint int64_t
#endif

#if CSIZET_SIZE==4
#define csize_t uint32_t
#else
#define csize_t uint64_t
#endif

#if LUA_INTEGER_SIZE==4
    #define lua_integer int32_t
#else
    #define lua_integer int64_t
#endif

#if LUA_NUMBER_SIZE==4
    #define lua_number float
#else
    #define lua_number double
#endif

#define lua_instruct uint32_t

namespace sl_lua{
    using byte = unsigned char;
    using bytes = std::vector<byte>;
    using string = std::string;
    template <typename T> using vec = std::vector<T>;
}
