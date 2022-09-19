#include "binary_chunk.h"

#include "binary_chunk_reader.h"

namespace sl_lua{

    void binary_chunk::check_header(binary_chunk_reader& reader){
        if(!vec_equal(LUA_SIGNATURE, reader.read_bytes(4))){ THROW_RUNTIME_ERROR("not a precompiled chunk!"); }
        if(LUAC_VERSION != reader.read_byte()){ THROW_RUNTIME_ERROR("version mismatch!"); }
        if(LUAC_FORMAT != reader.read_byte()){ THROW_RUNTIME_ERROR("format mismatch!"); }
        if(!vec_equal(LUAC_DATA, reader.read_bytes(6))){ THROW_RUNTIME_ERROR("corrupted!"); }
        if(CINT_SIZE != reader.read_byte()){ THROW_RUNTIME_ERROR("int size mismatch!"); }
        if(CSIZET_SIZE != reader.read_byte()){ THROW_RUNTIME_ERROR("size_t size mismatch!"); }
        if(INSTRUCTION_SIZE != reader.read_byte()){ THROW_RUNTIME_ERROR("instruction size mismatch!"); }
        if(LUA_INTEGER_SIZE != reader.read_byte()){ THROW_RUNTIME_ERROR("lua integer size mismatch!"); }
        if(LUA_NUMBER_SIZE != reader.read_byte()){ THROW_RUNTIME_ERROR("lua number size mismatch!"); }
        if(LUAC_INT != reader.read_lua_integer()){ THROW_RUNTIME_ERROR("endianess mismatch"); }
        if(LUAC_NUM != reader.read_lua_number()){ THROW_RUNTIME_ERROR("float mismatch!"); }
    }

    void binary_chunk::undump(binary_chunk_reader& reader){
        check_header(reader);
        size_up_values = reader.read_byte();
        reader.read_proto(main_func);
    }

    
    void print_header(prototype& proto){
        const char* func_type     = proto.line_defined > 0 ? "function" : "main";
        const char* variable_flag = proto.is_vararg ? "+" : "";
        printf("\n%s <%s:%d,%d> (%d instructions)\n", func_type, proto.source.data(), proto.line_defined,
               proto.last_line_defined, proto.codes.size());
        printf("%d%s params, %d slots, %d upvalues, ",
               proto.num_params, variable_flag, proto.max_stack_size, proto.upvalues.size());
        printf("%d locals, %d constants, %d functions\n",
               proto.loc_vars.size(), proto.constants.size(), proto.prototypes.size());
    }

    void print_code(prototype& proto){
        for(size_t i = 0; i < proto.codes.size(); i++){
            printf("\t%d\t[%d]\t0x%08X\n", i + 1, proto.line_info[i], proto.codes[i]);
        }
    }

    void print_detail(prototype& proto){
        printf("constants (%d):\n", proto.constants.size());
        int i = 1;
        for(auto& constant : proto.constants){ printf("\t%d\t%s\n", i++, to_string(constant).data()); }

        i = 0;
        printf("locals (%d):\n", proto.loc_vars.size());
        for(auto& loc_var : proto.loc_vars){
            printf("\t%d\t%s\t%d\t%d\n", i++,
                   loc_var.var_name.data(), loc_var.start_pc + 1, loc_var.end_pc + 1);
        }

        i = 0;
        printf("upvalues (%d):\n", proto.upvalues.size());
        for(auto& upval : proto.upvalues){
            const char* name = proto.upvalue_names.size() > 0 ? proto.upvalue_names[i].data() : "-";
            printf("\t%d\t%s\t%d\t%d\n", i++,
                   name, upval.in_stack, upval.idx);
        }
    }

    void print_proto(prototype& proto){
        print_header(proto);
        print_code(proto);
        print_detail(proto);
        for(auto& sub_protos : proto.prototypes){ print_proto(sub_protos); }
    }
}
