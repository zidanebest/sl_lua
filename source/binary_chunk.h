#pragma once
#include "sl_lua.h"

namespace sl_lua{
    class binary_chunk_reader;

    const bytes       LUA_SIGNATURE = {0x1b,'L','u','a'};
    const byte        LUAC_VERSION  = LUA_MAJOR_VERSION * 16 + LUA_MINOR_VERSION;
    const byte        LUAC_FORMAT   = 0;
    const bytes       LUAC_DATA     = {0x19,byte(0x93),'\r','\n',0x1a,'\n'};
    const lua_integer LUAC_INT      = 0x5678;
    const lua_number  LUAC_NUM      = 370.5;

    struct header{
        bytes       signature;       // magic number 0x1B4C7561
        byte        version;         // major version*16+minor version
        byte        format;          // 0
        bytes       luac_data;       // 0x 19 93 0D 0A 1A 0A
        byte        cintSize;        // 4
        byte        sizetSize;       // 8
        byte        instructionSize; // 4
        byte        luaIntegerSize;  // 8
        byte        luaNumberSize;   // 8
        lua_integer luacInt;         // check big-endian or little-endian    0x5678
        lua_number  luacNum;         // check float format 370.5

    };

    const byte TAG_NIL       = 0x00;
    const byte TAG_BOOLEAN   = 0x01;
    const byte TAG_NUMBER    = 0x03;
    const byte TAG_INTEGER   = 0x13;
    const byte TAG_SHORT_STR = 0x04;
    const byte TAG_LONG_STR  = 0x14;

    enum lua_const_type:uint8_t{
        NIL=0,
        BOOLEAN,
        NUMBER,
        INTEGER,
        SHORT_STR,
        LONG_STR
    };

    struct lua_const_base{
        lua_const_base() = default;
        lua_const_base(lua_const_type t): type(t){}
        lua_const_type type;
    };

    struct const_nil{ };

    using nil = const_nil;

    template <typename T> struct lua_const : public lua_const_base{
        lua_const(T d, lua_const_type t): lua_const_base(t), data(d){}
        T data;
    };

    template <> struct lua_const<const_nil> : public lua_const_base{
        lua_const(lua_const_type t): lua_const_base(t){}
    };
    
    static string to_string(lua_const_base* c){
        switch(c->type){
            case NIL: return "nil";
            case BOOLEAN: return static_cast<lua_const<byte>*>(c)->data == 0x1 ? "true" : "false";
            case INTEGER: return std::to_string(static_cast<lua_const<lua_integer>*>(c)->data);
            case NUMBER: return std::to_string(static_cast<lua_const<lua_number>*>(c)->data);
            case SHORT_STR: return static_cast<lua_const<string>*>(c)->data;
            case LONG_STR: return static_cast<lua_const<string>*>(c)->data;
            default: return {};
        }
    }

    struct upvalue{
        byte in_stack;
        byte idx;
    };

    struct loc_var{
        string   var_name;
        uint32_t start_pc; // index of start instruction
        uint32_t end_pc;   // index of end instruction
    };

    struct prototype{

        string               source;            // file name
        cint                 line_defined;      // start line
        cint                 last_line_defined; // end line
        byte                 num_params;        // num of solid params
        byte                 is_vararg;         // contain variable params
        byte                 max_stack_size;    // num of used register
        vec<lua_instruct>    codes;             // instruction table
        vec<lua_const_base*> constants;         // constant table
        vec<upvalue>         upvalues;          // upvalue table
        vec<prototype>       prototypes;        // sub-functions 
        vec<cint>            line_info;         // each instruction corresponds to a line_info
        vec<loc_var>         loc_vars;          // local variables
        vec<string>          upvalue_names;     // names of upvalues
        // int                      size_code;         // instruction table size
        // int                      size_constants;    // constant table size
        // int                      size_upvalues;     // upvalue table size
        // int                      size_prototypes;   // num of sub-functions
        // int                      size_loc_vars;     // num of local variables
    };

    struct binary_chunk{
        /*header        h;*/
        byte      size_up_values;
        prototype main_func;

    private:
        void check_header(binary_chunk_reader& reader);
    public:
        void undump(binary_chunk_reader& reader);
    };

    void print_proto(prototype& proto);

}
