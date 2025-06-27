#include "parser.h"

bool parseData(Data& data, int argc, char** argv) {
    size_t cnt;
    auto check_string_arg = [&](const char* str_short, const char* str_long, std::string Data::* str) mutable -> bool {
        if (!std::strcmp(argv[cnt], str_short) || !std::strcmp(argv[cnt], str_long)) {
            if (cnt + 1 < argc) {
                data.*str = argv[cnt + 1];
                ++cnt;
                return true;
            }
        }
        return false;
    };

    auto check_num_arg = [&](const char* str_short, const char* str_long, size_t Data::* num) mutable -> bool {
        if (!std::strcmp(argv[cnt], str_short) || !std::strcmp(argv[cnt], str_long)) {
            if (cnt + 1 < argc) {
                data.*num = std::strtoull(argv[cnt + 1], nullptr, 10);
                ++cnt;
                return true;
            }
        }
        return false;
    };

    for (cnt = 1; cnt < argc; ++cnt) {
        if (check_string_arg(constants::kInputShort, constants::kInputLong, &Data::input_path)) {
            continue;
        }
        if (check_string_arg(constants::kOutputLong, constants::kOutputShort, &Data::output_dir)) {
            continue;
        }
        if (check_num_arg(constants::kFrequencyShort, constants::kFrequencyLong, &Data::frequency)) {
            continue;
        }
        if (check_num_arg(constants::kMaxIterShort, constants::kMaxIterLong, &Data::max_iter)) {
            continue;
        }
    }
    return true;
}
