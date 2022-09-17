#pragma once

namespace sl_lua{
    
    using byte=unsigned char;
    using lua_integer=long long;
    using lua_number=double;

    constexpr const char* LUA_SIGNATURE    = "\x1bLua";
    constexpr byte        LUAC_VERSION     = 0x53;
    constexpr byte        LUAC_FORMAT      = 0;
    constexpr const char* LUAC_DATA        = "\x19\x93\r\n\x1a\n";
    constexpr byte        CINT_SIZE        = 4;
    constexpr byte        INSTRUCTION_SIZE = 4;
    constexpr byte        LUA_INTEGER_SIZE = 8;
    constexpr byte        LUA_NUMBER_SIZE  = 8;
    constexpr lua_integer LUAC_INT         = 0x5678;
    constexpr lua_number  LUAC_NUM         = 370.5;
    
    struct header{
        byte signature[4];      // magic number 0x1B4C7561
        byte version;           // major version*16+minor version
        byte format;            // 0
        byte luac_data[6];      // 0x 19 93 0D 0A 1A 0A
        byte cintSize;          // 4
        byte sizetSize;         // 8
        byte instructionSize;   // 4
        byte luaIntegerSize;    // 8
        byte luaNumberSize;     // 8
        lua_integer luacInt;    // check big-endian or little-endian    0x5678
        lua_number luacNum;     // check float format 370.5
    };
    struct prototype{
        
    };

    
    constexpr byte TAG_NIL       = 0x00;
    constexpr byte TAG_BOOLEAN   = 0x01;
    constexpr byte TAG_NUMBER    = 0x03;
    constexpr byte TAG_INTEGER   = 0x13;
    constexpr byte TAG_SHORT_STR = 0x04;
    constexpr byte TAG_LONG_STR  = 0x14;
    
    
    struct binary_chunk{
        header h;
        unsigned char size_up_values;
        prototype* main_func;
        
    };
    
}
