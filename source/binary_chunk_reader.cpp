#include "binary_chunk_reader.h"

#include <valarray>


#include "sl_lua.h"

namespace sl_lua{

    binary_chunk_reader::binary_chunk_reader(): read_idx(0){ }

    byte binary_chunk_reader::next(){
        if(read_idx >= data.size()){ THROW_RUNTIME_ERROR("reader out of range"); }
        read_idx += 1;
        return data[read_idx - 1];
    }

    byte binary_chunk_reader::read_byte(){ return next(); }

    bytes binary_chunk_reader::read_bytes(size_t n){
        bytes b(n);
        for(size_t i = 0; i < n; ++i){ b[i] = next(); }
        return b;
    }

    uint32_t binary_chunk_reader::read_lua_uint32(){
        bytes    b      = read_bytes(4);
        uint32_t result = 0;
#if LITTLE_ENDIAN
        for(size_t i = 0; i < 4; ++i){ result += (uint32_t)b[i] << i * 8; }
#else
        for(int i=3;i>=0;--i){
            result+=b[i]<<(3-i)*8;
        }
#endif
        return result;
    }

    uint64_t binary_chunk_reader::read_lua_uint64(){
        bytes    b      = read_bytes(8);
        uint64_t result = 0;
#if LITTLE_ENDIAN
        for(size_t i = 0; i < 8; ++i){ result += (uint64_t)b[i] << i * 8; }
#else
        for(int i=7;i>=0;--i){
            result+=b[i]<<(7-i)*8;
        }
#endif
        return result;
    }

    // template <typename T> void print_byte(T t, size_t n){
    //     byte* b = new byte[n];
    //     std::cout << std::endl;
    //     for(size_t i = 0; i < n; ++i){
    //         b[i] = ((byte*)&t)[i];
    //         printf("%02x  ", b[i]);
    //     }
    //     std::cout << std::endl;
    // }

    lua_integer binary_chunk_reader::read_lua_integer(){
#if LUA_INTEGER_SIZE==4
        uint32_t result=read_lua_uint32();
        return ((lua_integer*)&result)[0];
#else
        uint64_t result = read_lua_uint64();
        return ((lua_integer*)&result)[0];
#endif
    }

    lua_number binary_chunk_reader::read_lua_number(){
#if LUA_INTEGER_SIZE==4
        uint32_t result=read_lua_uint32();
        return ((lua_number*)&result)[0];
#else
        uint64_t result = read_lua_uint64();
        return ((lua_number*)&result)[0];
#endif
    }

    cint binary_chunk_reader::read_lua_cint(){
#if CINT_SIZE==4
        uint32_t result = read_lua_uint32();
        return ((cint*)&result)[0];
#else
        uint64_t result = read_lua_uint64();
        return ((cint*)&result)[0];
#endif
    }

    csize_t binary_chunk_reader::read_lua_csizet(){
#if CSIZET_SIZE==4
        csize_t result=read_lua_uint32();
        return result;
#else
        csize_t result = read_lua_uint64();
        return result;
#endif
    }

    lua_instruct binary_chunk_reader::read_lua_instruct(){ return read_lua_uint32(); }

    string binary_chunk_reader::read_string(){
        size_t size   = 0;
        byte   byte_1 = read_byte();
        if(byte_1 == 0) return "";
        if(byte_1 == 0xff){ size = read_lua_csizet() - 1; }
        else{ size = byte_1 - 1; }
        char* result = new char[size + 1];
        for(size_t i = 0; i < size; ++i){ result[i] = char(read_byte()); }
        result[size] = '\0';
        return result;
    }

    vec<lua_instruct> binary_chunk_reader::read_codes(){
        cint              code_size = read_lua_cint();
        vec<lua_instruct> codes(code_size);
        for(size_t i = 0; i < code_size; ++i){ codes[i] = read_lua_instruct(); }
        return codes;
    }

    vec<lua_const_base*> binary_chunk_reader::read_constants(){
        cint                 const_size = read_lua_cint();
        vec<lua_const_base*> consts(const_size);
        for(size_t i = 0; i < const_size; ++i){ consts[i] = read_const(); }
        return consts;
    }

    // return lua_const<lua_number>(read_lua_number(), NUMBER);

    lua_const_base* binary_chunk_reader::read_const(){
        byte const_type = read_byte();
        switch(const_type){
            case TAG_NIL: return new lua_const<const_nil>(NIL);
            case TAG_BOOLEAN: return new lua_const<byte>(read_byte(), BOOLEAN);
            case TAG_NUMBER: return new lua_const<lua_number>(read_lua_number(), NUMBER);
            case TAG_INTEGER: return new lua_const<lua_integer>(read_lua_integer(), INTEGER);
            case TAG_SHORT_STR: return new lua_const<string>(read_string(), SHORT_STR);
            case TAG_LONG_STR: return new lua_const<string>(read_string(), LONG_STR);
            default: THROW_RUNTIME_ERROR("invalid const type!");
        }
        return nullptr;
    }

    vec<upvalue> binary_chunk_reader::read_upvalues(){
        cint         upvalue_size = read_lua_cint();
        vec<upvalue> upvalues(upvalue_size);
        for(size_t i = 0; i < upvalue_size; ++i){ upvalues[i] = read_upvalue(); }
        return upvalues;
    }

    upvalue binary_chunk_reader::read_upvalue(){
        upvalue u;
        u.in_stack = read_byte();
        u.idx      = read_byte();
        return u;
    }

    vec<prototype> binary_chunk_reader::read_protos(string parent_source){
        cint           protos_size = read_lua_cint();
        vec<prototype> protos(protos_size);
        for(size_t i = 0; i < protos_size; ++i){ read_proto(protos[i],parent_source); }
        return protos;
    }

    vec<cint> binary_chunk_reader::read_line_info(){
        cint      line_info_size = read_lua_cint();
        vec<cint> line_info(line_info_size);
        for(size_t i = 0; i < line_info_size; ++i){ line_info[i] = read_lua_cint(); }
        return line_info;
    }

    vec<loc_var> binary_chunk_reader::read_loc_vars(){
        cint         loc_vars_size = read_lua_cint();
        vec<loc_var> loc_vars(loc_vars_size);
        for(size_t i = 0; i < loc_vars_size; ++i){ loc_vars[i] = read_loc_var(); }
        return loc_vars;
    }

    loc_var binary_chunk_reader::read_loc_var(){
        loc_var var;
        var.var_name = read_string();
        var.start_pc = read_lua_cint();
        var.end_pc   = read_lua_cint();
        return var;
    }

    vec<string> binary_chunk_reader::read_upvalue_names(){
        cint        upvalue_names_size = read_lua_cint();
        vec<string> upvalue_names(upvalue_names_size);
        for(size_t i = 0; i < upvalue_names_size; ++i){ upvalue_names[i] = read_string(); }
        return upvalue_names;
    }

    void binary_chunk_reader::read_proto(prototype& p, string parent_source){
        p.source            = read_string();
        p.line_defined      = read_lua_cint();
        p.last_line_defined = read_lua_cint();
        p.num_params        = read_byte();
        p.is_vararg         = read_byte();
        p.max_stack_size    = read_byte();
        p.codes             = read_codes();
        p.constants         = read_constants();
        p.upvalues          = read_upvalues();
        p.prototypes        = read_protos(p.source);
        p.line_info         = read_line_info();
        p.loc_vars          = read_loc_vars();
        p.upvalue_names     = read_upvalue_names();
        if(p.source == "") p.source = parent_source;
    }


}
