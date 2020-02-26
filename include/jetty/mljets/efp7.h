#ifndef PYJETTY_MLJETS_EFP7_HH
#define PYJETTY_MLJETS_EFP7_HH

#include <vector>
#include <string>

namespace efp7 {

extern std::vector<std::vector<size_t> > order_7;

extern std::vector<std::string> contraction_7;

extern size_t path_7[489][14][6];

extern bool is_symmetric_7[489][14][6];

extern std::vector<std::vector<size_t> > cache_index_7;

size_t efp7_npath(const size_t index);

size_t efp7(std::vector<size_t> &order, std::string &contraction,
            std::vector<std::vector<size_t> > &path, const size_t index);

size_t efp7(std::vector<size_t> &order, std::string &contraction,
            std::vector<std::vector<size_t> > &path,
            std::vector<std::vector<bool> > &is_symmetric,
            std::vector<size_t> &cache_index,
            const size_t index);
}

#endif
