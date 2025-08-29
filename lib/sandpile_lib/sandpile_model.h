#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../bmp_lib/bmp_writer.h"
#include "lattice_block.h"

struct Grain {
    Grain(uint64_t x, uint64_t y, uint64_t cnt) : x(x), y(y), cnt(cnt) {}
    uint64_t x;
    uint64_t y;
    uint64_t cnt;
};

class SandPileModel {
   public:
    using latticePtr = std::shared_ptr<LatticeBlock>;
    SandPileModel(const std::string& in, const std::string& out, size_t freq = 0,
                  size_t max_iter = std::numeric_limits<size_t>::max())
        : input_fname_(in),
          output_fname_(out),
          max_iter_(max_iter),
          freq_(freq),
          x_borders_{std::numeric_limits<size_t>::max(), 0},
          y_borders_{std::numeric_limits<size_t>::max(), 0} {}
    ~SandPileModel() = default;

    void proccess();

   private:

    bool blockIteration(latticePtr& block);
    void scatter(latticePtr& block, const std::pair<size_t, size_t>& coord);

    // функции для рассыпания песчинок по соседним клеткам
    void addLeftGrain();
    void addRightGrain();
    void addTopGrain();
    void addBottomGrain();

    // вспомогательные функции:

    // загрузка данных
    bool loadGrains();
    void fillRoot(std::vector<Grain>& grains, size_t delta, uint64_t min_x, uint64_t min_y);

    // итерация (рассыпание песчинок)
    bool iteration();

    // BMP Write methods
    bool storeGrainsInBMP(size_t i);
    void loadLatticeBlockLine(std::shared_ptr<LatticeBlock>& block, BMPWriter& bmp_writer, size_t padding_bits,
                              size_t line_size);
    std::pair<uint32_t, uint32_t> getPictureSize();

    uint64_t lattice_size_;
    size_t freq_;
    size_t max_iter_;
    std::string input_fname_;
    std::string output_fname_;
    latticePtr root_;
    latticePtr left_upper_;    // нода верхнего левого угла
    latticePtr right_bottom_;  // нода нижнего правого угла

    // макс длина крайних блоков
    std::pair<size_t, size_t> x_borders_;
    std::pair<size_t, size_t> y_borders_;
};
