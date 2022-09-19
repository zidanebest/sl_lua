#include "stdio.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "sl_lua.h"

#include "binary_chunk_reader.h"

void read_lua_binary(const char* name,sl_lua::bytes& binary_data){
    std::ifstream ifstream(name, std::ios::binary);
    if(ifstream.is_open()){
        ifstream.seekg(0, std::ios::end);
        size_t end_pos = ifstream.tellg();
        ifstream.seekg(0);
        binary_data.resize(end_pos);
        for(size_t i = 0; i < end_pos; ++i){
            char data[1];
            ifstream.read(data, 1);
            binary_data[i] = static_cast<unsigned char>(*data);
        }
        ifstream.close();
        
        int i = 0;
        for(int byte_elem : binary_data){
            printf("%02x  ", byte_elem);
            ++i;
            if(i % 8 == 0){ std::cout << std::endl; }
        }
    }
    else{ std::cerr << "fail to open file!" << std::endl; }
}



int main(int argc, char* argv[])
{
    char*            name;
    if(argc > 1){ name = argv[1]; }
    else{ name = "luac.out"; }
    
    sl_lua::binary_chunk_reader reader;
    read_lua_binary(name,reader.get_data());
    sl_lua::binary_chunk chunk;
    chunk.undump(reader);
    
    sl_lua::print_proto(chunk.main_func);

}
