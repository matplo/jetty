#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include <cstring>
#include <cfloat>

#include <jetty/mljets/blasdrv.h>

class cblas_mkl_t cblas;

namespace {

#ifdef EINSUM_VERBOSE
    bool einsum_verbose = false;
#endif // EINSUM_VERBOSE

    float builtin_sdot(size_t n, const float *x, long int incx,
                       const float *y, long int incy)
    {
        return cblas._sdot(n, x, incx, y, incy);
    }

    void builtin_scopy(size_t n, const float *x, float *y)
    {
        cblas._scopy(n, x, 1, y, 1);
    }

    void builtin_sscal(size_t n, float sa, float *sx, long int incx)
    {
        cblas._sscal(n, sa, sx, incx);
    }

    void builtin_sgemv(bool nota, size_t n, const float *a,
                       const float *x, float *y)
    {
        cblas._sgemv(cblas_mkl_t::CblasColMajor,
                     nota ?
                     cblas_mkl_t::CblasNoTrans :
                     cblas_mkl_t::CblasTrans,
                     n, n, 1.0F, a, n, x, 1, 0.0F, y, 1);
    }

    void builtin_ssymv(size_t n, const float *a, const float *x,
                       float *y)
    {
        cblas._ssymv(cblas_mkl_t::CblasColMajor,
                     cblas_mkl_t::CblasUpper,
                     n, 1.0F, a, n, x, 1, 0.0F, y, 1);
    }

    void builtin_sger(size_t n, const float *x, const float *y,
                      float *a)
    {
        cblas._sger(cblas_mkl_t::CblasColMajor,
                    n, n, 1.0F, x, 1, y, 1, a, n);
    }

    void builtin_sgemm(bool transa, bool transb, size_t n,
                       const float *a, const float *b, float *c)
    {
        cblas._sgemm(cblas_mkl_t::CblasColMajor,
                     transa ?
                     cblas_mkl_t::CblasTrans :
                     cblas_mkl_t::CblasNoTrans,
                     transb ?
                     cblas_mkl_t::CblasTrans :
                     cblas_mkl_t::CblasNoTrans,
                     n, n, n, 1.0F, a, n, b, n, 0.0F, c, n);
    }

    void builtin_ssymm(bool sider, size_t n, const float *a,
                       const float *b, float *c)
    {
        cblas._ssymm(cblas_mkl_t::CblasColMajor,
                     sider ?
                     cblas_mkl_t::CblasRight :
                     cblas_mkl_t::CblasLeft,
                     cblas_mkl_t::CblasUpper,
                     n, n, 1.0F, a, n, b, n, 0.0F, c, n);
    }

    void builtin_vsmul(size_t n, const float a[], const float b[],
                       float r[])
    {
        cblas._vsmul(n, a, b, r);
    }

    bool test_avx(bool *has_fma3 = NULL, bool *has_avx512f = NULL)
    {
        unsigned int eax = 1;
        unsigned int cpuid[4];

        __asm__ __volatile__ (
            "xchg   %%ebx, %%edi\n\t"
            "cpuid\n\t"
            "xchg   %%ebx, %%edi\n"
            : "=a" (cpuid[0]), "=D" (cpuid[1]), "=c" (cpuid[2]),
              "=d" (cpuid[3])
            : "0" (eax));

        unsigned int ecx = 0;
        unsigned int edx;

        __asm__ __volatile__ (
            "xgetbv\n"
            : "=a" (eax), "=d"(edx)
            : "c" (ecx));

        static const unsigned int mask_osxsave_avx =
            (1 << 27) | (1 << 28);
        static const unsigned int mask_ymm_state =
            (1 << 1) | (1 << 2);

        const bool has_avx =
            (cpuid[2] & mask_osxsave_avx) == mask_osxsave_avx &&
            (eax & mask_ymm_state) == mask_ymm_state;

        if (has_fma3 != NULL) {
            static const unsigned int mask_fma3 = 1 << 12;

            *has_fma3 = (cpuid[2] & mask_fma3) == mask_fma3;
        }

        if (has_avx512f != NULL) {
            eax = 7;
            unsigned int ecx = 0;

            __asm__ __volatile__ (
                "xchg   %%ebx, %%edi\n\t"
                "cpuid\n\t"
                "xchg   %%ebx, %%edi\n"
                : "=a" (cpuid[0]), "=D" (cpuid[1]), "=c" (cpuid[2]),
                  "=d" (cpuid[3])
                : "0" (eax), "2" (ecx));

            static const unsigned int mask_avx512f = 1 << 16;

            *has_avx512f = (cpuid[1] & mask_avx512f) == mask_avx512f;
        }

        return has_avx;
    }

#define INNER3F_INIT_SSE                        \
    "xorps          %%xmm4, %%xmm4\n\t"         \
    "xorps          %%xmm5, %%xmm5\n\t"         \
    "xorps          %%xmm6, %%xmm6\n\t"         \
    "xorps          %%xmm7, %%xmm7\n"

#define INNER3F_INIT_AVX                        \
    "vxorps         %%xmm4, %%xmm4, %%xmm4\n\t" \
    "vxorps         %%xmm5, %%xmm5, %%xmm5\n\t" \
    "vxorps         %%xmm6, %%xmm6, %%xmm6\n\t" \
    "vxorps         %%xmm7, %%xmm7, %%xmm7\n"

#define INNER3F_INIT_AVX512F                     \
    "vxorps         %%xmm4, %%xmm4, %%xmm4\n\t"  \
    "vxorps         %%xmm5, %%xmm5, %%xmm5\n\t"  \
    "vxorps         %%xmm6, %%xmm6, %%xmm6\n\t"  \
    "vxorps         %%xmm7, %%xmm7, %%xmm7\n\t"  \
    "vxorps         %%xmm4, %%xmm4, %%xmm8\n\t"  \
    "vxorps         %%xmm5, %%xmm5, %%xmm9\n\t"  \
    "vxorps         %%xmm6, %%xmm6, %%xmm10\n\t" \
    "vxorps         %%xmm7, %%xmm7, %%xmm11\n"

#define INNER3F_BEGIN_LOOP                      \
    "1:\n\t"

#define INNER3F_LOAD_MUL_SSE                    \
    "movups         (%4,%0,4), %%xmm8\n\t"      \
    "movups         (%5,%0,4), %%xmm12\n\t"     \
    "movups         16(%4,%0,4), %%xmm9\n\t"    \
    "movups         16(%5,%0,4), %%xmm13\n\t"   \
    "movups         32(%4,%0,4), %%xmm10\n\t"   \
    "movups         32(%5,%0,4), %%xmm14\n\t"   \
    "movups         48(%4,%0,4), %%xmm11\n\t"   \
    "movups         48(%5,%0,4), %%xmm15\n\t"   \
    "mulps          %%xmm12, %%xmm8\n\t"        \
    "mulps          %%xmm13, %%xmm9\n\t"        \
    "mulps          %%xmm14, %%xmm10\n\t"       \
    "mulps          %%xmm15, %%xmm11\n\t"

#define INNER3F_LOAD_MUL_AVX                       \
    "vmovups        (%4,%0,4), %%ymm8\n\t"         \
    "vmovups        (%5,%0,4), %%ymm12\n\t"        \
    "vmovups        32(%4,%0,4), %%ymm9\n\t"       \
    "vmovups        32(%5,%0,4), %%ymm13\n\t"      \
    "vmovups        64(%4,%0,4), %%ymm10\n\t"      \
    "vmovups        64(%5,%0,4), %%ymm14\n\t"      \
    "vmovups        96(%4,%0,4), %%ymm11\n\t"      \
    "vmovups        96(%5,%0,4), %%ymm15\n\t"      \
    "vmulps         %%ymm12, %%ymm8, %%ymm8\n\t"   \
    "vmulps         %%ymm13, %%ymm9, %%ymm9\n\t"   \
    "vmulps         %%ymm14, %%ymm10, %%ymm10\n\t" \
    "vmulps         %%ymm15, %%ymm11, %%ymm11\n\t"

#define INNER3F_LOAD_MUL_AVX512F                   \
    "vmovups        (%4,%0,4), %%zmm12\n\t"        \
    "vmovups        (%5,%0,4), %%zmm20\n\t"        \
    "vmovups        64(%4,%0,4), %%zmm13\n\t"      \
    "vmovups        64(%5,%0,4), %%zmm21\n\t"      \
    "vmovups        128(%4,%0,4), %%zmm14\n\t"     \
    "vmovups        128(%5,%0,4), %%zmm22\n\t"     \
    "vmovups        192(%4,%0,4), %%zmm15\n\t"     \
    "vmovups        192(%5,%0,4), %%zmm23\n\t"     \
    "vmovups        256(%4,%0,4), %%zmm16\n\t"     \
    "vmovups        256(%5,%0,4), %%zmm24\n\t"     \
    "vmovups        320(%4,%0,4), %%zmm17\n\t"     \
    "vmovups        320(%5,%0,4), %%zmm25\n\t"     \
    "vmovups        384(%4,%0,4), %%zmm18\n\t"     \
    "vmovups        384(%5,%0,4), %%zmm26\n\t"     \
    "vmovups        448(%4,%0,4), %%zmm19\n\t"     \
    "vmovups        448(%5,%0,4), %%zmm27\n\t"     \
    "vmulps         %%zmm20, %%zmm12, %%zmm12\n\t" \
    "vmulps         %%zmm21, %%zmm13, %%zmm13\n\t" \
    "vmulps         %%zmm22, %%zmm14, %%zmm14\n\t" \
    "vmulps         %%zmm23, %%zmm15, %%zmm15\n\t" \
    "vmulps         %%zmm24, %%zmm16, %%zmm16\n\t" \
    "vmulps         %%zmm25, %%zmm17, %%zmm17\n\t" \
    "vmulps         %%zmm26, %%zmm18, %%zmm18\n\t" \
    "vmulps         %%zmm27, %%zmm19, %%zmm19\n\t"

#define INNER3F_MADD_SSE                        \
    "mulps          (%6,%0,4), %%xmm8\n\t"      \
    "mulps          16(%6,%0,4), %%xmm9\n\t"    \
    "mulps          32(%6,%0,4), %%xmm10\n\t"   \
    "mulps          48(%6,%0,4), %%xmm11\n\t"   \
    "addps          %%xmm8, %%xmm4\n\t"         \
    "addps          %%xmm9, %%xmm5\n\t"         \
    "addps          %%xmm10, %%xmm6\n\t"        \
    "addps          %%xmm11, %%xmm7\n\t"

#define INNER3F_MADD_AVX                                \
    "vmulps         (%6,%0,4), %%ymm8, %%ymm8\n\t"      \
    "vmulps         32(%6,%0,4), %%ymm9, %%ymm9\n\t"    \
    "vmulps         64(%6,%0,4), %%ymm10, %%ymm10\n\t"  \
    "vmulps         96(%6,%0,4), %%ymm11, %%ymm11\n\t"  \
    "vaddps         %%ymm8, %%ymm4, %%ymm4\n\t"         \
    "vaddps         %%ymm9, %%ymm5, %%ymm5\n\t"         \
    "vaddps         %%ymm10, %%ymm6, %%ymm6\n\t"        \
    "vaddps         %%ymm11, %%ymm7, %%ymm7\n\t"

#define INNER3F_MADD_AVX_FMA3                           \
    "vfmadd231ps    (%6,%0,4), %%ymm8, %%ymm4\n\t"      \
    "vfmadd231ps    32(%6,%0,4), %%ymm9, %%ymm5\n\t"    \
    "vfmadd231ps    64(%6,%0,4), %%ymm10, %%ymm6\n\t"   \
    "vfmadd231ps    96(%6,%0,4), %%ymm11, %%ymm7\n\t"

#define INNER3F_MADD_AVX512F                            \
    "vfmadd231ps    (%6,%0,4), %%zmm12, %%zmm4\n\t"     \
    "vfmadd231ps    64(%6,%0,4), %%zmm13, %%zmm5\n\t"   \
    "vfmadd231ps    128(%6,%0,4), %%zmm14, %%zmm6\n\t"  \
    "vfmadd231ps    192(%6,%0,4), %%zmm15, %%zmm7\n\t"  \
    "vfmadd231ps    256(%6,%0,4), %%zmm16, %%zmm8\n\t"  \
    "vfmadd231ps    320(%6,%0,4), %%zmm17, %%zmm9\n\t"  \
    "vfmadd231ps    384(%6,%0,4), %%zmm18, %%zmm10\n\t" \
    "vfmadd231ps    448(%6,%0,4), %%zmm19, %%zmm11\n\t"

#define INNER3F_END_LOOP_SSE                    \
    "addq           $16, %0\n\t"                \
    "subq           $16, %1\n\t"                \
    "jnz            1b\n\t"

#define INNER3F_END_LOOP_AVX                    \
    "addq           $32, %0\n\t"                \
    "subq           $32, %1\n\t"                \
    "jnz            1b\n\t"

#define INNER3F_END_LOOP_AVX512F                \
    "addq           $128, %0\n\t"               \
    "subq           $128, %1\n\t"               \
    "jnz            1b\n\t"

#define INNER3F_HADD_STORE_SSE                  \
    "addps          %%xmm5, %%xmm4\n\t"         \
    "addps          %%xmm7, %%xmm6\n\t"         \
    "addps          %%xmm6, %%xmm4\n\t"         \
    "haddps         %%xmm4, %%xmm4\n\t"         \
    "haddps         %%xmm4, %%xmm4\n\t"         \
    "movss          %%xmm4, (%7)"

#define INNER3F_HADD_STORE_AVX                      \
    "vaddps         %%ymm5, %%ymm4, %%ymm4\n\t"     \
    "vaddps         %%ymm7, %%ymm6, %%ymm6\n\t"     \
    "vaddps         %%ymm6, %%ymm4, %%ymm4\n\t"     \
    "vextractf128   $1, %%ymm4, %%xmm5\n\t"         \
    "vaddps         %%xmm5, %%xmm4, %%xmm4\n\t"     \
    "vhaddps        %%xmm4, %%xmm4, %%xmm4\n\t"     \
    "vhaddps        %%xmm4, %%xmm4, %%xmm4\n\t"     \
    "vmovss         %%xmm4, (%7)\n\t"               \
    "vzeroupper"

#define INNER3F_HADD_STORE_AVX512F                  \
    "vaddps         %%zmm5, %%zmm4, %%zmm4\n\t"     \
    "vaddps         %%zmm7, %%zmm6, %%zmm6\n\t"     \
    "vaddps         %%zmm9, %%zmm8, %%zmm8\n\t"     \
    "vaddps         %%zmm11, %%zmm10, %%zmm10\n\t"  \
    "vaddps         %%zmm6, %%zmm4, %%zmm4\n\t"     \
    "vaddps         %%zmm10, %%zmm8, %%zmm8\n\t"    \
    "vaddps         %%zmm8, %%zmm4, %%zmm4\n\t"     \
    "vextractf64x4  $1, %%zmm4, %%ymm5\n\t"         \
    "vaddps         %%ymm5, %%ymm4, %%ymm4\n\t"     \
    "vextractf128   $1, %%ymm4, %%xmm5\n\t"         \
    "vaddps         %%xmm5, %%xmm4, %%xmm4\n\t"     \
    "vhaddps        %%xmm4, %%xmm4, %%xmm4\n\t"     \
    "vhaddps        %%xmm4, %%xmm4, %%xmm4\n\t"     \
    "vmovss         %%xmm4, (%7)"

#define INNER3F_OPERAND_CLOBBER                                     \
    : "=r" (i), "=r" (n)                                            \
    : "0" (i), "1" (n), "r" (a), "r" (b), "r" (c), "r" (dot)        \
    : "cc", "%xmm4", "%xmm5", "%xmm6", "%xmm7", "%xmm8", "%xmm9",   \
      "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15",   \
      "memory"

    void inner3f_sse_16(size_t n, float *a, float *b, float *c,
                        float *dot)
    {
        register size_t i = 0;

        __asm__ __volatile__ (
            INNER3F_INIT_SSE
            INNER3F_BEGIN_LOOP
            INNER3F_LOAD_MUL_SSE
            INNER3F_MADD_SSE
            INNER3F_END_LOOP_SSE
            INNER3F_HADD_STORE_SSE
            INNER3F_OPERAND_CLOBBER);
    }

    void inner3f_avx_32(size_t n, float *a, float *b, float *c,
                        float *dot)
    {
        register size_t i = 0;

        __asm__ __volatile__ (
            INNER3F_INIT_AVX
            INNER3F_BEGIN_LOOP
            INNER3F_LOAD_MUL_AVX
            INNER3F_MADD_AVX
            INNER3F_END_LOOP_AVX
            INNER3F_HADD_STORE_AVX
            INNER3F_OPERAND_CLOBBER);
    }

    void inner3f_avx_fma3_32(size_t n, float *a, float *b, float *c,
                             float *dot)
    {
        register size_t i = 0;

        __asm__ __volatile__ (
            INNER3F_INIT_AVX
            INNER3F_BEGIN_LOOP
            INNER3F_LOAD_MUL_AVX
            INNER3F_MADD_AVX_FMA3
            INNER3F_END_LOOP_AVX
            INNER3F_HADD_STORE_AVX
            INNER3F_OPERAND_CLOBBER);
    }

    void inner3f_avx512f_128(size_t n, float *a, float *b, float *c,
                             float *dot)
    {
        register size_t i = 0;

        __asm__ __volatile__ (
            INNER3F_INIT_AVX512F
            INNER3F_BEGIN_LOOP
            INNER3F_LOAD_MUL_AVX512F
            INNER3F_MADD_AVX512F
            INNER3F_END_LOOP_AVX512F
            INNER3F_HADD_STORE_AVX512F
            INNER3F_OPERAND_CLOBBER);
    }

    float builtin_inner3f(size_t n, float *a, float *b, float *c)
    {
        static bool has_fma3;
        static bool has_avx512f;
        static bool has_avx = test_avx(&has_fma3, &has_avx512f);
        float dot = 0;
        size_t i;

        if (has_avx512f) {
            size_t n1 = n & (size_t)(-128);

            if (n1 != 0) {
                inner3f_avx512f_128(n1, a, b, c, &dot);
            }
            i = n1;
            n1 = (n - n1) & (size_t)(-32);

            if (n1 != 0) {
                float dot_32 = 0;

                inner3f_avx_fma3_32(n1, a + i, b + i, c + i, &dot_32);
                dot += dot_32;
            }
            i += n1;
        }
        else if (has_avx) {
            const size_t n1 = n & (size_t)(-32);

            if (n1 != 0) {
                if (has_fma3) {
                    inner3f_avx_fma3_32(n1, a, b, c, &dot);
                }
                else {
                    inner3f_avx_32(n1, a, b, c, &dot);
                }
            }
            i = n1;
        }
        else {
            size_t nu = (4 - ((size_t)c >> 2)) & 0x3;

            for (i = 0; i < nu; i++) {
                dot += a[i] * b[i] * c[i];
            }

            const size_t n1 = (n - nu) & (size_t)(-16);
            float dot_a;

            if (n1 != 0) {
                inner3f_sse_16(n1, a + nu, b + nu, c + nu, &dot_a);
                dot += dot_a;
            }
            i = n1;
        }

        for (; i < n; i++) {
            dot += a[i] * b[i] * c[i];
        }

        return dot;
    }

#define ERROR_ILLEGAL_CONTRACTION                          \
    fprintf(stderr, "%s:%d: error: illegal contraction "   \
            "%s,%s->%s\n", __FILE__, __LINE__,             \
            iu.c_str(), iv.c_str(), iw.c_str())

#define ERROR_UNIMPLEMENTED_CONTRACTION                    \
    fprintf(stderr, "%s:%d: error: unimplemented "         \
            "contraction %s,%s->%s\n", __FILE__, __LINE__, \
            iu.c_str(), iv.c_str(), iw.c_str())

#define ERROR_UNIMPLEMENTED_CONTRACTION_LONG                    \
    {                                                           \
        fprintf(stderr, "%s:%d: error: unimplemented "          \
                "contraction ", __FILE__, __LINE__);            \
        for (std::vector<size_t>::const_iterator iterator =     \
                 p_descending.begin();                          \
             iterator != p_descending.end(); iterator++) {      \
            fprintf(stderr, "%s%s",                             \
                    input_index_remaining[*iterator].c_str(),   \
                    iterator + 1 == p_descending.end() ?        \
                    "->\n" : ",");                              \
        }                                                       \
    }

    // (Two tensor) rank 1, 2 -> 2 contraction
    void contract_2_1_2_2(size_t n,
                          std::vector<float> u, std::string iu,
                          std::vector<float> v, std::string iv,
                          std::vector<float> &w, std::string iw)
    {
        builtin_scopy(std::pow(n, 2), &v[0], &w[0]);
        if (iu[0] == iv[0]) {
            // a,ab->ab
            for (size_t i = 0; i < n; i++) {
                builtin_sscal(n, u[i], &w[i * n], 1);
            }
        }
        else if (iu[0] == iv[1]) {
            // b,ab->ab
            for (size_t i = 0; i < n; i++) {
                builtin_sscal(n, u[i], &w[i], n);
            }
        }
    }

    // (Two tensor) rank 2, 2 -> 1 contraction
    void contract_2_2_2_1(size_t n,
                          std::vector<float> u, std::string iu,
                          std::vector<float> v, std::string iv,
                          std::vector<float> &w, std::string iw)
    {
        if (iu[0] == iv[0] && iu[1] == iv[1]) {
            if (iv[0] == iw[0]) {
                // ab,ab->a
                for (size_t i = 0; i < n; i++) {
                    w[i] = builtin_sdot(n, &u[i * n], 1,
                                        &v[i * n], 1);
                }
            }
            else if (iv[1] == iw[0]) {
                // ab,ab->b
                for (size_t i = 0; i < n; i++) {
                    w[i] = builtin_sdot(n, &u[i], n, &v[i], n);
                }
            }
            else {
                ERROR_UNIMPLEMENTED_CONTRACTION;
            }
        }
        else {
            ERROR_UNIMPLEMENTED_CONTRACTION;
        }
    }

    // (Two tensor) rank 2, 2 -> 2 contraction
    void contract_2_2_2_2(size_t n,
                          std::vector<float> u, bool su,
                          std::string iu,
                          std::vector<float> v, bool sv,
                          std::string iv,
                          std::vector<float> &w, std::string iw)
    {
        // ab,ab->ab
        if (iu[0] == iv[0] && iv[0] == iw[0] &&
            iu[1] == iv[1] && iv[1] == iw[1]) {
            builtin_vsmul(u.size(), &u[0], &v[0], &w[0]);
        }
        else if ((iu[0] == iv[0] && iu[1] != iv[1]) ||
                 (iu[0] == iv[1] && iu[1] != iv[0]) ||
                 (iu[1] == iv[0] && iu[0] != iv[1]) ||
                 (iu[1] == iv[1] && iu[0] != iv[0])) {
            const bool ntu = iu[0] == iv[0] || iu[0] == iv[1];
            const bool ntv = iu[0] == iv[1] || iu[1] == iv[1];
            const bool tw =
                ntu ? iu[1] == iw[0] :
                ntv ? iv[0] == iw[1] : true;

#ifdef EINSUM_VERBOSE
            if (einsum_verbose) {
                fprintf(stderr, "%s:%d: %s %s %s %d %d %d\n",
                        __FILE__, __LINE__, iu.c_str(), iv.c_str(),
                        iw.c_str(), ntu, ntv, tw);
            }
#endif // EINSUM_VERBOSE

            // Note that for symmetric matrices, only su = false and
            // sv = true occurs. And BLAS xSYMM only handles cases
            // where u is transposed the same way as w, i.e. ntu !=
            // tw.
            if (sv && ntu != tw) {
                builtin_ssymm(ntu, n, &v[0], &u[0], &w[0]);
            }
            else if (tw) {
                builtin_sgemm(ntv, ntu, n, &v[0], &u[0], &w[0]);
            }
            else {
                builtin_sgemm(!ntu, !ntv, n, &u[0], &v[0], &w[0]);
            }
        }
        else {
            ERROR_UNIMPLEMENTED_CONTRACTION;
        }
    }

    // Two tensor contraction
    void contract_2(size_t n,
                    std::vector<float> u, bool su, std::string iu,
                    std::vector<float> v, bool sv, std::string iv,
                    std::vector<float> &w, std::string iw)
    {
        switch (iu.size()) {
        case 1:
            switch (iv.size()) {
            case 1:
                switch (iw.size()) {
                case 0: // a,a->
                    w.front() = builtin_sdot(n, &u[0], 1, &v[0], 1);
                    break;
                case 1: // a,a->a
                    builtin_vsmul(u.size(), &u[0], &v[0], &w[0]);
                    break;
                case 2: // a,b->ab
                    memset(&w[0], 0, std::pow(n, 2) * sizeof(float));
                    builtin_sger(n, &v[0], &u[0], &w[0]);
                    break;
                default:
                    ERROR_ILLEGAL_CONTRACTION;
                }
                break;
            case 2:
                switch (iw.size()) {
                case 1: // a,ab->b; b,ab->a
                    if (sv) {
                        builtin_ssymv(n, &v[0], &u[0], &w[0]);
                    }
                    else {
                        builtin_sgemv(iv[0] != iw[0], n, &v[0],
                                      &u[0], &w[0]);
                    }
                    break;
                case 2: // a,ab->ab; b,ab->ab
                    contract_2_1_2_2(n, u, iu, v, iv, w, iw);
                    break;
                default:
                    ERROR_ILLEGAL_CONTRACTION;
                }
                break;
            default:
                ERROR_UNIMPLEMENTED_CONTRACTION;
            }
            break;
        case 2:
            switch (iv.size()) {
            case 1:
                switch (iw.size()) {
                case 1: // ab,a->b
                    if (su) {
                        builtin_ssymv(n, &u[0], &v[0], &w[0]);
                    }
                    else {
                        builtin_sgemv(iu[0] != iw[0], n, &u[0],
                                      &v[0], &w[0]);
                    }
                    break;
                case 2: // ab,b
                    contract_2_1_2_2(n, v, iv, u, iu, w, iw);
                    break;
                default:
                    ERROR_UNIMPLEMENTED_CONTRACTION;
                }
                break;
            case 2:
                switch (iw.size()) {
                case 0:
                    // ab,ab->
                    w[0] = builtin_sdot(std::pow(n, 2), &u[0], 1,
                                        &v[0], 1);
                    break;
                case 1:
                    contract_2_2_2_1(n, u, iu, v, iv, w, iw);
                    break;
                case 2:
                    contract_2_2_2_2(n, u, su, iu, v, sv, iv, w, iw);
                    break;
                default:
                    ERROR_ILLEGAL_CONTRACTION;
                }
                break;
            default:
                ERROR_UNIMPLEMENTED_CONTRACTION;
            }
            break;
        default:
            ERROR_UNIMPLEMENTED_CONTRACTION;
        }
    }

    // 6 tensor rank 4 contraction
    void contract_6_4(size_t n,
                      std::vector<float> u0, std::string iu0,
                      std::vector<float> u1, std::string iu1,
                      std::vector<float> u2, std::string iu2,
                      std::vector<float> u3, std::string iu3,
                      std::vector<float> u4, std::string iu4,
                      std::vector<float> u5, std::string iu5,
                      std::vector<float> &w, std::string iw)
    {
        std::vector<std::vector<float> *> u = {
            &u0, &u1, &u2, &u3, &u4, &u5
        };
        std::vector<const std::string *> iu = {
            &iu0, &iu1, &iu2, &iu3, &iu4, &iu5
        };
        std::vector<std::vector<float> *> ui[4];
        std::vector<const std::string *> iui[4];

        for (size_t i = 0; i < u.size(); i++) {
            for (int j = 3; j >= 0; j--) {
                if ((*iu[i])[1] - 'a' >= j ||
                    (*iu[i])[0] - 'a' >= j) {
                    ui[j].push_back(u[i]);
                    iui[j].push_back(iu[i]);
                    break;
                }
            }
        }
        // All cases must have 3 innermost operands, all with the 2nd
        // index the one that is stored contiguously
        if (ui[0].size() != 0 || ui[1].size() != 1 ||
            ui[2].size() != 2 || ui[3].size() != 3 ||
            (*iui[3][0])[1] != 'd' ||
            (*iui[3][1])[1] != 'd' ||
            (*iui[3][2])[1] != 'd') {
            w[0] = NAN;
            return;
        }

        size_t i[4];
        double s0 = 0;
        std::vector<float> buffer(n, NAN);

        for (i[0] = 0; i[0] < n; i[0]++) {
            float s1 = 0;

            for (i[1] = 0; i[1] < n; i[1]++) {
                float s2 = 0;

                for (i[2] = 0; i[2] < n; i[2]++) {
                    float s3 = builtin_inner3f(
                        n,
                        &(*ui[3][0])[i[(*iui[3][0])[0] - 'a'] * n],
                        &(*ui[3][1])[i[(*iui[3][1])[0] - 'a'] * n],
                        &(*ui[3][2])[i[(*iui[3][2])[0] - 'a'] * n]);

                    s3 *=
                        (*ui[2][0])[i[(*iui[2][0])[1] - 'a'] +
                                    i[(*iui[2][0])[0] - 'a'] * n] *
                        (*ui[2][1])[i[(*iui[2][1])[1] - 'a'] +
                                    i[(*iui[2][1])[0] - 'a'] * n];
                    s2 += s3;
                }
                s2 *= (*ui[1][0])[i[(*iui[1][0])[1] - 'a'] +
                                  i[(*iui[1][0])[0] - 'a'] * n];
                s1 += s2;
            }
            s0 += s1;
        }
        w[0] = s0;
    }

    std::vector<std::string> string_split(std::string ins, char sep)
    {
        std::vector<std::string> r;
#if 0
        std::istringstream in(ins);
        std::string s;

        while (std::getline(in, s, sep)) {
            r.push_back(s);
        }
#else
        for (size_t i = 0; i < ins.size(); i++) {
            for (size_t j = 1; j < 3; j++) {
                if ((i + j < ins.size() && ins[i + j] == ',') ||
                    i + j == ins.size()) {
                    r.push_back(ins.substr(i, j));
                    i += j;
                    break;
                }
            }
        }
#endif

        return r;
    }

    float einstein_sum_efp(
        std::vector<std::vector<float> > &cache,
        const std::string contraction,
        std::vector<std::vector<std::vector<float> >::const_iterator>
        operand_ref,
        const std::vector<std::vector<size_t> > path,
        const std::vector<std::vector<bool> > is_symmetric =
        std::vector<std::vector<bool> >(),
        const std::vector<size_t> cache_index =
        std::vector<size_t>())
    {
#ifdef EINSUM_VERBOSE
        if (einsum_verbose) {
            const std::string input_index =
                contraction.substr(0, contraction.find("->"));
            const std::string output_index =
                contraction.find("->") == std::string::npos ? "" :
                contraction.substr(contraction.find("->") + 2);

            fprintf(stderr, "%s:%d: %s->%s\n", __FILE__, __LINE__,
                    input_index.c_str(), output_index.c_str());
        }
#endif // EINSUM_VERBOSE
        std::vector<std::string> input_index_remaining =
            string_split(contraction, ',');

        size_t n = 0;

        for (size_t i = operand_ref.size() - 1;
             i < operand_ref.size(); i--) {
            if (input_index_remaining[i].size() == 1) {
                n = operand_ref[i]->size();
                break;
            }
            else if (input_index_remaining[i].size() == 2) {
                // This is correct, but should never actually happen
                n = (size_t)sqrt(operand_ref[i]->size());
                break;
            }
        }

        std::vector<std::vector<float> > buffer;

        // Buffer has to remain at the same memory location
        buffer.reserve(path.size());

        for (std::vector<std::vector<size_t> >::const_iterator
                 iterator_path = path.begin();
             iterator_path != path.end(); iterator_path++) {
            std::set<char> index_contraction;
            std::set<char> index_remaining;

            for (size_t i = 0; i < input_index_remaining.size();
                 i++) {
                if (std::find(iterator_path->begin(),
                              iterator_path->end(), i)
                    != iterator_path->end()) {
                    for (std::string::const_iterator iterator_index =
                             input_index_remaining[i].begin();
                         iterator_index !=
                             input_index_remaining[i].end();
                         iterator_index++) {
                        index_contraction.insert(*iterator_index);
                    }
                }
                else {
                    for (std::string::const_iterator iterator_index =
                             input_index_remaining[i].begin();
                         iterator_index !=
                             input_index_remaining[i].end();
                         iterator_index++) {
                        index_remaining.insert(*iterator_index);
                    }
                }
            }

            std::string index_result;

            std::set_intersection(index_contraction.begin(),
                                  index_contraction.end(),
                                  index_remaining.begin(),
                                  index_remaining.end(),
                                  std::back_inserter(index_result));

            std::vector<size_t> p_descending = *iterator_path;

            std::sort(p_descending.rbegin(), p_descending.rend());

            const std::string iu = p_descending.size() >= 1 ?
                input_index_remaining[p_descending[0]] : "";
            const std::string iv = p_descending.size() >= 2 ?
                input_index_remaining[p_descending[1]] : "";
            const std::string iw = index_result;
            const bool su =
                is_symmetric[iterator_path - path.begin()][0];
            const bool sv =
                is_symmetric[iterator_path - path.begin()][1];
            const std::vector<std::vector<float> >
                empty(1, std::vector<float>());
            const std::vector<std::vector<float> >::const_iterator
                u = p_descending.size() >= 1 ?
                operand_ref[p_descending[0]] : empty.begin();
            const std::vector<std::vector<float> >::const_iterator
                v = p_descending.size() >= 2 ?
                operand_ref[p_descending[1]] : empty.begin();

#ifdef EINSUM_VERBOSE
            if (einsum_verbose) {
                fprintf(stderr, "%s:%d: %lu ", __FILE__, __LINE__,
                        iterator_path - path.begin());
                for (std::vector<size_t>::const_iterator iterator_p =
                         p_descending.begin();
                     iterator_p != p_descending.end();
                     iterator_p++) {
                    fprintf(stderr, "%s%s",
                            input_index_remaining[*iterator_p].
                            c_str(),
                            iterator_p + 1 == p_descending.end() ?
                            "" : ",");
                }
                fprintf(stderr, "->%s\n", index_result.c_str());

                for (size_t j = 0; j < ((iu.size() == 2) ? n : 1);
                     j++) {
                    for (size_t i = 0; i < n; i++) {
                        fprintf(stderr, "%11.8f%s", (*u)[i + j * n],
                                i == n - 1 ? "\n" : ", ");
                    }
                }
                for (size_t j = 0; j < ((iv.size() == 2) ? n : 1);
                     j++) {
                    for (size_t i = 0; i < n; i++) {
                        fprintf(stderr, "%11.8f%s", (*v)[i + j * n],
                                i == n - 1 ? "\n" : ", ");
                    }
                }
            }
#endif // EINSUM_VERBOSE

            std::vector<std::vector<float> >::iterator w;

            // The last step always result in a unique result
            if (cache.empty()) {
                cache.resize(cache_index.back());
            }
            if (cache_index[iterator_path - path.begin()] !=
                cache_index.back()) {
                w = cache.begin() +
                    cache_index[iterator_path - path.begin()];
            }
            else {
                buffer.push_back(std::vector<float>());
                w = buffer.end() - 1;
            }

            if (w->empty()) {
                w->resize(std::pow(n, iw.size()), NAN);
                switch (p_descending.size()) {
                case 2:
                    contract_2(n, *u, su, iu, *v, sv, iv, *w, iw);
                    break;
                case 6:
                    contract_6_4(
                        n, *u, iu, *v, iv,
                        *operand_ref[p_descending[2]],
                        input_index_remaining[p_descending[2]],
                        *operand_ref[p_descending[3]],
                        input_index_remaining[p_descending[3]],
                        *operand_ref[p_descending[4]],
                        input_index_remaining[p_descending[4]],
                        *operand_ref[p_descending[5]],
                        input_index_remaining[p_descending[5]],
                        *w, iw);
                    break;
                default:
                    ERROR_UNIMPLEMENTED_CONTRACTION_LONG;
                }
            }
            if (iw.empty()) {
                return w->front();
            }

            for (std::vector<size_t>::const_iterator iterator_p =
                     p_descending.begin();
                 iterator_p != p_descending.end(); iterator_p++) {
                input_index_remaining.
                    erase(input_index_remaining.begin() +
                          *iterator_p);
                operand_ref.erase(operand_ref.begin() + *iterator_p);
            }
            input_index_remaining.push_back(index_result);
            operand_ref.push_back(w);

#ifdef EINSUM_VERBOSE
            if (einsum_verbose) {
                for (size_t j = 0; j < ((iw.size() == 2) ? n : 1);
                     j++) {
                    for (size_t i = 0; i < n; i++) {
                        fprintf(stderr, "%11.8f%s", (*w)[i + j * n],
                                i == n - 1 ? "\n" : ", ");
                    }
                }
            }
#endif // EINSUM_VERBOSE
        }

        return NAN;
    }

}
