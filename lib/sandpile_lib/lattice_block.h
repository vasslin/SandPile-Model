#include <cstring>
#include <iostream>
#include <memory>

class LatticeBlock {
   public:
    friend class SandPileModel;

    static inline size_t cnt = 0;

    LatticeBlock() = default;
    LatticeBlock(size_t sz, std::pair<size_t, size_t> right_bottom = {0, 0},
                 std::pair<size_t, size_t> left_upper = {0, 0});
    ~LatticeBlock();
    LatticeBlock(const LatticeBlock& other);
    LatticeBlock operator=(const LatticeBlock& other);
    LatticeBlock(LatticeBlock&& other);
    LatticeBlock operator=(LatticeBlock&& other);
    bool operator==(const LatticeBlock& other);

   private:
    void deleteData();

    uint64_t** data = nullptr;
    size_t size = 0;

    std::pair<size_t, size_t> left_upper_coord_ = {0, 0};  // границы хранящихся данных
    std::pair<size_t, size_t> right_bottom_coord_ = {0, 0};

    std::weak_ptr<LatticeBlock> top;
    std::shared_ptr<LatticeBlock> bottom = nullptr;
    std::weak_ptr<LatticeBlock> left;
    std::shared_ptr<LatticeBlock> right = nullptr;
};