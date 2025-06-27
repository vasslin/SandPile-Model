#include <fstream>
#include <iostream>
#include <limits>

#include "lib/parser_lib/parser.cpp"
#include "lib/sandpile_lib/sandpile_model.h"

int main(int argc, char** argv) {
    Data data;
    if (!parseData(data, argc, argv)) {
        return 0;
    }
    SandPileModel sp_model{data.input_path, data.output_dir, data.frequency, data.max_iter};
    sp_model.proccess();
}
