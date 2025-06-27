
#include "lattice_block.h"

LatticeBlock::LatticeBlock(size_t sz, std::pair<size_t, size_t> right_bottom, std::pair<size_t, size_t> left_upper)
    : size(sz), bottom(nullptr), right(nullptr), right_bottom_coord_(right_bottom), left_upper_coord_(left_upper) {
        data = new uint64_t*[sz];

    for (size_t i = 0; i < sz; ++i) {
        data[i] = new uint64_t[sz];
        for (size_t j = 0; j < sz; ++j) {
            data[i][j] = 0;
        }
    }
}

LatticeBlock::LatticeBlock(const LatticeBlock& other)
    : size(other.size),
      data(new uint64_t*[other.size]),
      top(other.top),
      bottom(other.bottom),
      left(other.left),
      right(other.right),
      right_bottom_coord_(other.right_bottom_coord_),
      left_upper_coord_(other.left_upper_coord_)

{
    for (size_t i = 0; i < size; ++i) {
        data[i] = new uint64_t[size];
        std::memcpy(data[i], other.data[i], size);
    }
}

LatticeBlock LatticeBlock::operator=(const LatticeBlock& other) {
    if (*this == other) {
        return *this;
    }
    if (size != other.size) {
        deleteData();
    }
    size = other.size;
    top = other.top;
    bottom = other.bottom;
    left = other.left;
    right = other.right;
    right_bottom_coord_ = other.right_bottom_coord_;
    left_upper_coord_ = other.left_upper_coord_;

    data = new uint64_t*[other.size];
    for (size_t i = 0; i < size; ++i) {
        data[i] = new uint64_t[size];
        std::memcpy(data[i], other.data[i], size);
    }
    return *this;
}

LatticeBlock::LatticeBlock(LatticeBlock&& other)
    : size(other.size),
      data(other.data),
      top(other.top),
      bottom(std::exchange(other.bottom, nullptr)),
      left(other.left),
      right(std::exchange(other.right, nullptr)),
      left_upper_coord_(std::exchange(other.left_upper_coord_, {0, 0})),
      right_bottom_coord_(std::exchange(other.right_bottom_coord_, {0, 0}))

{
    other.data = nullptr;
    other.size = 0;
    other.left.reset();
    other.top.reset();
}

LatticeBlock LatticeBlock::operator=(LatticeBlock&& other) {
    if (*this == other) {
        return *this;
    }
    deleteData();
    data = std::exchange(other.data, nullptr);
    size = std::exchange(other.size, 0);

    left_upper_coord_ = std::exchange(other.left_upper_coord_, {0, 0});
    right_bottom_coord_ = std::exchange(other.right_bottom_coord_, {0, 0});

    right = std::exchange(other.right, nullptr);
    bottom = std::exchange(other.bottom, nullptr);

    left = other.left;
    top = other.top;
    other.left.reset();
    other.top.reset();

    return *this;
}

LatticeBlock::~LatticeBlock() {
    deleteData();
}

void LatticeBlock::deleteData() {
    for (size_t i = 0; i < size; ++i) {
        delete[] data[i];
    }
    delete[] data;
}

bool LatticeBlock::operator==(const LatticeBlock& other) {
    if (size != other.size || left.lock() != other.left.lock() || right != other.right ||
        top.lock() != other.top.lock() || bottom != other.bottom) {
        return false;
    }
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (data[i][j] != other.data[i][j]) {
                return false;
            }
        }
    }
    return true;
}