//
// Created by Łukasz on 20.01.2024.
//
#include "matplotlibcpp.h"
#include <fftw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>

namespace plt = matplotlibcpp;

// Funkcja do obserwacji widma sygnału
void plot_freq(const std::vector<std::complex<double>>& x, double fs) {
    int N = x.size(); // Length of the signal

    // Allocate input and output arrays for FFTW
    fftw_complex *in, *out;
    fftw_plan p;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // Copy our std::vector into the input array
    for (int i = 0; i < N; ++i) {
        in[i][0] = x[i].real();
        in[i][1] = x[i].imag();
    }

    // Create plan and execute FFT
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    // Calculate magnitude and center the spectrum
    std::vector<double> mag(N);
    int halfN = N / 2;
    for (int i = 0; i < N; ++i) {
        int idx = (i + halfN) % N; // Centering
        mag[idx] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / N;
    }

    // Convert magnitude to dB
    std::vector<double> mag_dB(N);
    for (int i = 0; i < N; ++i) {
        mag_dB[i] = 20 * log10(mag[i]);
    }

    // Prepare frequency axis in MHz
    std::vector<double> f(N);
    double freq_resolution = fs / N;
    for (int i = 0; i < N; ++i) {
        f[i] = (i - N / 2) * freq_resolution / 1e6;
    }

    // Plot
    plt::plot(f, mag_dB);
    plt::xlabel("Częstotliwość [MHz]");
    plt::ylabel("Amplituda [dB]");
    plt::grid(true);
    plt::xlim(f.front(), f.back());
    plt::show();

    // Cleanup
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

int main() {
    // Zmienne kontrolujące przetwarzanie sygnału
    int do_LPfilter = 0; // 0/1 - filtr dolnoprzepustowy, przepuszcza tylko używane nośniki PBCH
    int do_disturbing = 0; // 0/1 - zakłócanie sygnału
    int do_ADCcorrect = 0; // 0/1 - korekcja częstotliwości próbkowania ADC
    int do_plots = 1; // 0/1 - wyświetlanie wykresów

    // Wczytanie danych z pliku .txt
    std::ifstream inputFile("Data/LTE_TM3p1_10MZ_18p22dBFS.txt"); // Change directory!!
    if (!inputFile) {
        std::cerr << "Blad wczytywania pliku." << std::endl;
        return 1;
    }

    std::vector<std::complex<double>> signal; // Przechowuje próbki sygnału

    double realPart, imagPart;
    while (inputFile >> realPart >> imagPart) {
        signal.emplace_back(realPart, imagPart);
    }

    inputFile.close();

    double fs = 30.72e6; // Częstotliwość próbkowania
    double fs_lte = 7.68e6; // Częstotliwość LTE
    double THR = 15; // Parametry sygnału
    int REPEAT = 0;

    int N = signal.size();
    double df_lte ;
    for (int i = 0; i < N-1; i++) {

        signal[i] = signal[i] * std::exp(std::complex<double>(0, -2 * M_PI * i * 5 * df_lte / fs));
    }

    
    // Obserwacja widma sygnału
    plot_freq(signal, fs);

    // Tutaj można kontynuować przetwarzanie sygnału w języku C++

    return 0;
}

