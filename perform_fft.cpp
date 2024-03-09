#include "perform_fft.h"
#include <fftw3.h>
std::vector<std::complex<double>> perform_fft(const std::vector<std::complex<double>>& x) {
    int N = x.size();
    fftw_complex *in, *out;
    fftw_plan p;
    std::vector<std::complex<double>> y(N);

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        in[i][0] = x[i].real();
        in[i][1] = x[i].imag();
    }

    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    for (int i = 0; i < N; ++i) {
        y[i] = std::complex<double>(out[i][0], out[i][1]);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    return y;
}
