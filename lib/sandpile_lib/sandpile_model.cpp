#include "sandpile_model.h"

void SandPileModel::proccess() {
    if (!loadGrains()) {
        return;
    }
    size_t i;
    for (i = 0; (i < max_iter_) && !iteration(); ++i) {
        if (freq_ != 0 && (i % freq_ == 0)) {
            storeGrainsInBMP(i);
        }
    }
    storeGrainsInBMP(i);
}

bool SandPileModel::loadGrains() {
    std::ifstream in(input_fname_);
    if (!in.is_open()) {
        std::cerr << "Unable to open a file\n";
        return false;
    }

    uint64_t max_x, max_y;
    max_x = max_y = std::numeric_limits<uint64_t>::min();
    uint64_t min_x, min_y;
    min_x = min_y = std::numeric_limits<uint64_t>::max();

    std::vector<Grain> grains;
    size_t x, y, cnt;
    while (in >> x) {
        if (!(in >> y) || !(in >> cnt)) {
            std::cerr << "Invalid file data\n";
            return false;
        }
        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);

        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);

        grains.emplace_back(x, y, cnt);
    }

    uint64_t length = std::max((max_x - min_x + 1ULL), (max_y - min_y + 1ULL));
    uint64_t delta = std::ceil(length / 2.0);  // отступ в клетке root_ (с каждой из сторон)
    lattice_size_ = length + delta * 2;

    x_borders_.first = lattice_size_;
    y_borders_.first = lattice_size_;

    root_ = std::make_shared<LatticeBlock>(lattice_size_);
    left_upper_ = root_;
    right_bottom_ = root_;
    fillRoot(grains, delta, min_x, min_y);
    return true;
}

void SandPileModel::fillRoot(std::vector<Grain>& grains, size_t delta, uint64_t min_x, uint64_t min_y) {
    uint64_t x, y;
    std::pair<size_t, size_t> max_pair = {0, 0};
    std::pair<size_t, size_t> min_pair = {std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()};
    for (auto coord : grains) {
        x = coord.x - min_x + delta;
        y = coord.y - min_y + delta;

        root_->data[x][y] = coord.cnt;

        x_borders_ = {std::min(x_borders_.first, x), std::max(x_borders_.second, x + 1)};
        y_borders_ = {std::min(y_borders_.first, y), std::max(y_borders_.second, y + 1)};

        max_pair = {std::max(max_pair.first, x + 1), std::max(max_pair.second, y + 1)};
        min_pair = {std::min(min_pair.first, x), std::min(min_pair.second, y)};
    }
    root_->left_upper_coord_ = min_pair;
    root_->right_bottom_coord_ = {max_pair.first + 1, max_pair.second + 1};
}

bool SandPileModel::iteration() {
    bool stable = true;
    for (auto ptr_i = left_upper_; ptr_i != nullptr; ptr_i = ptr_i->bottom) {
        for (auto ptr_j = ptr_i; ptr_j != nullptr; ptr_j = ptr_j->right) {
            stable &= blockIteration(ptr_j);
        }
    }
    return stable;
}

struct scatterElement {
    scatterElement(SandPileModel::latticePtr& block, size_t x, size_t y) : block(block), x(x), y(y) {}

    SandPileModel::latticePtr block;
    size_t x;
    size_t y;
};

bool SandPileModel::blockIteration(latticePtr& ptr) {
    bool stable = true;

    std::vector<scatterElement> scatter_blocks{};

    for (auto x = ptr->left_upper_coord_.first; x != ptr->right_bottom_coord_.first; ++x) {
        for (auto y = ptr->left_upper_coord_.second; y != ptr->right_bottom_coord_.second; ++y) {
            if (ptr->data[x][y] > 3) {
                // stable = false;
                ptr->data[x][y] -= 4;
                scatter_blocks.emplace_back(ptr, x, y);
            }
        }
    }

    for (auto el : scatter_blocks) {
        scatter(el.block, {el.x, el.y});
    }

    return scatter_blocks.empty();
}

void SandPileModel::scatter(latticePtr& block, const std::pair<size_t, size_t>& coord) {
    if (coord.first == 0) {
        if (block->top.lock() == nullptr) {
            addTopGrain();
        }
        ++(block->top.lock()->data[block->size - 1][coord.second]);
    } else {
        block->left_upper_coord_.first = std::min(block->left_upper_coord_.first, coord.first - 1);
        ++(block->data[coord.first - 1][coord.second]);

        if (block->top.lock() == nullptr) {
            x_borders_.first = std::min(x_borders_.first, coord.first - 1);
        }
    }

    if (coord.first == block->size - 1) {
        if (block->bottom == nullptr) {
            addBottomGrain();
        }
        ++(block->bottom->data[0][coord.second]);
    } else {
        block->right_bottom_coord_.first = std::max(block->right_bottom_coord_.first, coord.first + 2);
        ++(block->data[coord.first + 1][coord.second]);
        if (block->bottom == nullptr) {
            x_borders_.second = std::max(x_borders_.second, coord.first + 2);
        }
    }

    if (coord.second == 0) {
        if (block->left.lock() == nullptr) {
            addLeftGrain();
        }
        ++(block->left.lock()->data[coord.first][block->size - 1]);
    } else {
        block->left_upper_coord_.second = std::min(block->left_upper_coord_.second, coord.second - 1);
        ++(block->data[coord.first][coord.second - 1]);
        if (block->left.lock() == nullptr) {
            y_borders_.first = std::min(y_borders_.first, coord.second - 1);
        }
    }

    if (coord.second == block->size - 1) {
        if (block->right == nullptr) {
            addRightGrain();
        }
        ++(block->right->data[coord.first][0]);
    } else {
        block->right_bottom_coord_.second = std::max(block->right_bottom_coord_.second, coord.second + 2);
        ++(block->data[coord.first][coord.second + 1]);
        if (block->right == nullptr) {
            y_borders_.second = std::max(y_borders_.second, coord.second + 2);
        }
    }
}

// добавляет верхний ряд нод. в каждой ноде data будет заполнена на один нижний ряд
void SandPileModel::addTopGrain() {
    latticePtr prev_node = nullptr;
    auto sz = left_upper_->size;

    std::shared_ptr<LatticeBlock> new_left_upper = nullptr;
    for (auto ptr = left_upper_; ptr != nullptr; ptr = ptr->right) {
        auto unode_ptr =
            std::make_shared<LatticeBlock>(sz, std::pair<size_t, size_t>{sz, sz}, std::pair<size_t, size_t>{sz - 1, 0});
        unode_ptr->left = prev_node;
        if (prev_node != nullptr) {
            prev_node->right = unode_ptr;
        }
        unode_ptr->bottom = ptr;
        ptr->top = unode_ptr;
        prev_node = unode_ptr;

        if (new_left_upper == nullptr) {
            new_left_upper = unode_ptr;
        }
    }
    left_upper_ = new_left_upper;
    x_borders_.first = lattice_size_ - 1;
}

void SandPileModel::addLeftGrain() {
    latticePtr prev_node = nullptr;
    auto sz = left_upper_->size;

    std::shared_ptr<LatticeBlock> new_left_upper = nullptr;
    for (auto ptr = left_upper_; ptr != nullptr; ptr = ptr->bottom) {
        auto lnode_ptr =
            std::make_shared<LatticeBlock>(sz, std::pair<size_t, size_t>{sz, sz}, std::pair<size_t, size_t>{0, sz - 1});

        lnode_ptr->top = prev_node;
        if (prev_node != nullptr) {
            prev_node->bottom = lnode_ptr;
        }
        ptr->left = lnode_ptr;

        lnode_ptr->right = ptr;
        prev_node = lnode_ptr;

        if (new_left_upper == nullptr) {
            new_left_upper = lnode_ptr;
        }
    }
    left_upper_ = new_left_upper;
    y_borders_.first = lattice_size_ - 1;
}

void SandPileModel::addRightGrain() {
    latticePtr prev_node = nullptr;
    auto sz = right_bottom_->size;

    for (auto ptr = right_bottom_; ptr != nullptr; ptr = ptr->top.lock()) {
        auto rnode_ptr =
            std::make_shared<LatticeBlock>(sz, std::pair<size_t, size_t>{sz, 1}, std::pair<size_t, size_t>{0, 0});

        rnode_ptr->bottom = prev_node;
        if (prev_node != nullptr) {
            prev_node->top = rnode_ptr;
        }
        ptr->right = rnode_ptr;
        rnode_ptr->left = ptr;
        prev_node = rnode_ptr;
    }
    right_bottom_ = right_bottom_->right;
    y_borders_.second = 1;
}

void SandPileModel::addBottomGrain() {
    latticePtr prev_node = nullptr;
    auto sz = left_upper_->size;
    for (auto ptr = right_bottom_; ptr != nullptr; ptr = ptr->left.lock()) {
        auto unode_ptr =
            std::make_shared<LatticeBlock>(sz, std::pair<size_t, size_t>{1, sz}, std::pair<size_t, size_t>{0, 0});
        unode_ptr->right = prev_node;

        if (prev_node != nullptr) {
            prev_node->left = unode_ptr;
        }
        unode_ptr->top = ptr;
        ptr->bottom = unode_ptr;
        prev_node = unode_ptr;
    }
    right_bottom_ = right_bottom_->bottom;
    x_borders_.second = 1;
}

std::pair<uint32_t, uint32_t> SandPileModel::getPictureSize() {
    uint32_t x_size = (lattice_size_ - x_borders_.first) + x_borders_.second;
    uint32_t y_size = (lattice_size_ - y_borders_.first) + y_borders_.second;

    if (!(left_upper_->top.lock()) && !(left_upper_->bottom)) {
        x_size = x_borders_.second - x_borders_.first;
    }
    if (!(left_upper_->left.lock()) && !(left_upper_->right)) {
        y_size = y_borders_.second - y_borders_.first;
    }

    size_t x_blocks_cnt = 0;
    size_t y_blocks_cnt = 0;
    for (auto ptr = left_upper_->right; ptr != nullptr && ptr->right != nullptr; ptr = ptr->right) {
        ++y_blocks_cnt;
    }
    y_size += lattice_size_ * y_blocks_cnt;

    for (auto ptr = left_upper_->bottom; ptr != nullptr && ptr->bottom != nullptr; ptr = ptr->bottom) {
        ++x_blocks_cnt;
    }
    x_size += lattice_size_ * x_blocks_cnt;

    return {x_size, y_size};
}

bool SandPileModel::storeGrainsInBMP(size_t i) {
    std::ofstream out(output_fname_ + std::to_string(i) + ".bmp");
    if (!out.is_open()) {
        std::cerr << "Unable to open output file\n";
        return false;
    }
    auto sz = getPictureSize();
    uint32_t padding_bits = (sz.second % 8 == 0) ? 0 : (8 - (sz.second % 8));
    BMPWriter bmp_writer = {out, sz.second + padding_bits, sz.first};
    bmp_writer.writeHeaders();
    for (auto ptr = left_upper_; ptr != nullptr; ptr = ptr->bottom) {
        loadLatticeBlockLine(ptr, bmp_writer, padding_bits, sz.second);
    }
    out.close();
    return true;
}

// compresses 64 bits data in old_data into 8 bits data in new_data according to the rule:
// new_data[i] = (old_data[i] < 4?) old_data[i] : 4
// if sizes of old_data and new_data are not sz - undefined behaviour
void compressInto4Bits(char* old_data, char* new_data, size_t sz) {
    for (size_t i = 0; i < sz; i += 2) {
        if ((sz & 1) && (i == sz - 1)) {
            new_data[i / 2] = ((old_data[i] & 0xF) << 4) & 0xFF;
        } else {
            new_data[i / 2] = (((old_data[i] & 0xF) << 4) | (old_data[i + 1] & 0xF)) & 0xFF;
        }
    }
}

void SandPileModel::loadLatticeBlockLine(std::shared_ptr<LatticeBlock>& block, BMPWriter& bmp_writer,
                                         size_t padding_bits, size_t line_size) {
    size_t y_start;
    size_t y_end;
    std::pair<size_t, size_t> hborders = {0, lattice_size_};
    if (block->top.lock() == nullptr) {
        hborders.first = x_borders_.first;
    }
    if (!(block->bottom)) {
        hborders.second = x_borders_.second;
    }

    char* bit_line_data = new char[line_size + padding_bits];

    for (size_t x = hborders.first; x != hborders.second; ++x) {
        size_t i = 0;
        for (auto ptr = block; ptr != nullptr; ptr = ptr->right) {
            std::pair<size_t, size_t> wborders = {0, lattice_size_};
            if (ptr->left.lock() == nullptr) {
                wborders.first = y_borders_.first;
            }
            if (ptr->right == nullptr) {
                wborders.second = y_borders_.second;
            }
            for (auto j = wborders.first; j != wborders.second; ++j, ++i) {
                bit_line_data[i] = (ptr->data[x][j] > 3) ? 4 : ptr->data[x][j];
            }
        }
        if (padding_bits) {
            for (; i < (line_size + padding_bits); ++i) {
                bit_line_data[i] = 0;
            }
        }

        auto bit4_size = (size_t)(std::ceil((line_size + padding_bits) / 2.));
        char* bit4_data = new char[bit4_size];

        compressInto4Bits(bit_line_data, bit4_data, line_size + padding_bits);
        bmp_writer.writePixels(bit4_data, bit4_size);
        delete[] bit4_data;
    }
    delete[] bit_line_data;
}
