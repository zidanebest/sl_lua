#pragma once
#include "binary_chunk.h"


namespace sl_lua{

    template <typename T> bool vec_equal(const vec<T>& vec1, const vec<T>& vec2){
        if(vec1.size() != vec2.size()) return false;
        for(size_t i = 0; i < vec1.size(); ++i){ if(vec1[i] != vec2[i]) return false; }
        return true;
    }

    class binary_chunk_reader{
    public:
        binary_chunk_reader();

        byte         read_byte();
        bytes        read_bytes(size_t n);
        lua_integer  read_lua_integer();
        lua_number   read_lua_number();
        cint         read_lua_cint();
        csize_t      read_lua_csizet();
        lua_instruct read_lua_instruct();

        string               read_string();
        vec<lua_instruct>    read_codes();
        vec<lua_const_base*> read_constants();
        lua_const_base*      read_const();
        vec<upvalue>         read_upvalues();
        upvalue              read_upvalue();
        vec<prototype>       read_protos(string parent_source);
        vec<cint>            read_line_info();
        vec<loc_var>         read_loc_vars();
        loc_var              read_loc_var();
        vec<string>          read_upvalue_names();
        void                 read_proto(prototype& p, string parent_source = "");


        inline bytes& get_data(){ return data; }
    private:
        byte     next();
        uint32_t read_lua_uint32();
        uint64_t read_lua_uint64();

        bytes  data;
        size_t read_idx;
    };


}
