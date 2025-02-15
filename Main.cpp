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
#include "perform_fft.h"

namespace plt = matplotlibcpp;

// Funkcja do obserwacji widma sygnału
void plot_freq(const std::vector<std::complex<double>>& x, double fs) {
    int N = x.size(); // Length of the signal

    // Perform FFT
    std::vector<std::complex<double>> fft_result = perform_fft(x);

    // Calculate magnitude and center the spectrum
    std::vector<double> mag(N);
    int halfN = N / 2;
    for (int i = 0; i < N; ++i) {
        int idx = (i + halfN) % N; // Centering
        mag[idx] = std::abs(fft_result[i]) / N;
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


    int Nfft = 2048; // Example FFT size
    int Nfftup = static_cast<int>(Nfft * fs / fs_lte);
    int Ncut = 16 * Nfftup;
    std::vector<std::complex<double>> s_cut(signal.begin(), signal.begin() + std::min<int>(Ncut, signal.size()));

    // Perform FFT on the cut signal
    std::vector<std::complex<double>> S_cut = perform_fft(s_cut);

    double df = fs / Nfftup;
    std::vector<double> absS_cut(S_cut.size());
    std::transform(S_cut.begin(), S_cut.end(), absS_cut.begin(), [](const std::complex<double>& c) { return std::abs(c); });

    int kcentr = Nfftup / 2;
    int Kmax = 20; // Search range for minimum magnitude
    auto min_it = std::min_element(absS_cut.begin() + kcentr - Kmax, absS_cut.begin() + kcentr + Kmax);
    int indx = std::distance(absS_cut.begin(), min_it);
    int SHIFT = indx - kcentr;

    // Apply the calculated shift to the entire signal
    for (size_t i = 0; i < signal.size(); ++i) {
        double phaseShift = -2 * M_PI * i * SHIFT * df / fs;
        signal[i] *= std::exp(std::complex<double>(0, phaseShift));
    }

    // TODO: here you can insert the code for low-pass filtering if needed

    // Plot the frequency spectrum of the shifted signal
  //  if (true) { // Replace true with a condition to control plotting
   //     plot_freq(signal, fs);
    //}

    // continuation ...

    return 0;
}