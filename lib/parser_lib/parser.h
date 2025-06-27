#pragma once

#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace constants {

static inline const char* kInputShort = "-i";
static inline const char* kInputLong = "--input";

static inline const char* kOutputShort = "-o";
static inline const char* kOutputLong = "--output";

static inline const char* kMaxIterShort = "-m";
static inline const char* kMaxIterLong = "--max-iter";

static inline const char* kFrequencyShort = "-f";
static inline const char* kFrequencyLong = "--freq";

static inline const char* kNameOfFile = "/result";
}  // namespace constants

struct Data {
    std::string input_path{};
    std::string output_dir{};
    size_t max_iter = std::numeric_limits<size_t>::max();
    size_t frequency = 0;
};

bool parseData(Data& data, int argc, char** argv);
