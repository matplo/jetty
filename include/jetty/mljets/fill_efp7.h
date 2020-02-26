#ifndef JETTY_MLJETS_FILL_EFP7_H
#define JETTY_MLJETS_FILL_EFP7_H

#include <jetty/mljets/efp7.h>

#define FILL_EFP7(efp, particle, jet, eta, delta_r)                 \
    for (UInt_t i = 0; i < _branch_n ## jet; i++) {                 \
        std::vector<size_t> index_in_cone;                          \
        std::vector<std::vector<float> >                            \
            zi(1, std::vector<float>());                            \
        float sum_pt = 0;                                           \
                                                                    \
        for (size_t j = 0; j < particle.size(); j++) {              \
            if (std::pow(particle[j].pseudorapidity() -             \
                         _branch_ ## jet ## _ ## eta[i], 2) +       \
                std::pow(angular_range_reduce(                      \
                    particle[j].phi_std() -                         \
                    _branch_ ## jet ## _phi[i]), 2) <               \
                delta_r * delta_r) {                                \
                zi.back().push_back(particle[j].perp());            \
                sum_pt += particle[j].perp();                       \
                index_in_cone.push_back(j);                         \
            }                                                       \
        }                                                           \
                                                                    \
        std::vector<std::vector<float> > thetaii;                   \
                                                                    \
        thetaii.push_back(std::vector<float>(                       \
            std::pow(index_in_cone.size(), 2), 1));                 \
        thetaii.push_back(std::vector<float>(                       \
            std::pow(index_in_cone.size(), 2), NAN));               \
                                                                    \
        for (size_t j = 0; j < index_in_cone.size(); j++) {         \
            zi.back()[j] /= sum_pt;                                 \
            thetaii.back()[j * index_in_cone.size() + j] = 0;       \
            for (size_t k = 0; k < j; k++) {                        \
                thetaii.back()[j * index_in_cone.size() + k] =      \
                    thetaii.back()[k * index_in_cone.size() + j] =  \
                    particle[index_in_cone[j]].delta_R(             \
                        particle[index_in_cone[k]]);                \
                /* fprintf(stdout, "%s:%d: %f\n", __FILE__,         \
                 * __LINE__, thetaii.back()[j *                     \
                 * index_in_cone.size() + k]); */                   \
            }                                                       \
        }                                                           \
                                                                    \
        for (size_t j = 2; j <= 7; j++) {                           \
            thetaii.push_back(thetaii[1]);                          \
            for (std::vector<float>::iterator iterator =            \
                     thetaii.back().begin();                        \
                 iterator != thetaii.back().end(); iterator++) {    \
                *iterator = std::pow(*iterator, j);                 \
            }                                                       \
        }                                                           \
                                                                    \
        std::vector<std::vector<float> > cache;                     \
                                                                    \
        for (size_t k = 0; k < 489; k++) {                          \
            /* Skip the EFP calculation for jets with more tna 200  \
             * particles and when a rank (2, 2) contraction is      \
             * involved */                                          \
            if (index_in_cone.size() >= 200 &&                      \
                (k == 105 || k == 106 || k == 236 || k == 239)) {   \
                _branch_ ## jet ## _ ## efp[i][k] = NAN;            \
                continue;                                           \
            }                                                       \
                                                                    \
            std::vector<size_t> order;                              \
            std::string contraction;                                \
            std::vector<std::vector<size_t> > path;                 \
            std::vector<std::vector<bool> > is_symmetric;           \
            std::vector<size_t> cache_index;                        \
                                                                    \
            efp7::efp7(order, contraction, path, is_symmetric,      \
                 cache_index, k);                                   \
                                                                    \
            /* FIXME: Move this into efp7()? */                     \
            const size_t count_2 =                                  \
                k < 7 ? 1 : k < 19 ? 2 : k < 63 ? 3 :               \
                k < 96 ? 4 : k < 105 ? 5 : k < 107 ? 6 :            \
                k < 157 ? 4 : k < 216 ? 5 : k < 236 ? 6 :           \
                k < 240 ? 7 : k < 305 ? 5 : k < 366 ? 6 :           \
                k < 385 ? 7 : k < 433 ? 6 : 7;                      \
            const size_t count_1 =                                  \
                k < 7 ? 2 : k < 30 ? 3 : k < 107 ? 4 :              \
                k < 240 ? 5 : k < 385 ? 6 : k < 466 ? 7 : 8;        \
            std::vector<std::vector<std::vector<float> >::          \
                        const_iterator> operand_ref;                \
                                                                    \
            for (size_t l = 0; l < count_2; l++) {                  \
                operand_ref.push_back(thetaii.begin() + order[l]);  \
            }                                                       \
            for (size_t l = 0; l < count_1; l++) {                  \
                operand_ref.push_back(zi.begin());                  \
            }                                                       \
                                                                    \
            /* Because higher order EFP have small values, it is    \
             * prescaled by 2^15 to fully utilize the exponent      \
             * range of half precision */                           \
            _branch_ ## jet ## _ ## efp[i][k] = half(32768.0F *     \
                einstein_sum_efp(                                   \
                    cache, contraction, operand_ref, path,          \
                    is_symmetric, cache_index));                    \
            /* if (_branch_ ## jet ## _efp[i][k] > 1) {             \
             *     fprintf(stdout, "%s:%d: %u %u %f\n",             \
             *             __FILE__, __LINE__, i, k,                \
             *             _branch_ ## jet ## _efp[i][k]);          \
             *     for (size_t l = 0; l < zi.back().size();         \
             *          l++) {                                      \
             *         if (zi.back()[l] > 1) {                      \
             *             fprintf(stdout, "%s:%d: %u %u %f\n",     \
             *                     __FILE__, __LINE__, i, l,        \
             *                     zi.back()[l]);                   \
             *         }                                            \
             *     }                                                \
             *     for (size_t l = 0; l < thetaii[1].size();        \
             *          l++) {                                      \
             *         if (thetaii[1][l] > 1) {                     \
             *             fprintf(stdout, "%s:%d: %u %u %f\n",     \
             *                     __FILE__, __LINE__, i, l,        \
             *                     thetaii[1][l]);                  \
             *         }                                            \
             *     }                                                \
             * } */                                                 \
        }                                                           \
    }

#endif