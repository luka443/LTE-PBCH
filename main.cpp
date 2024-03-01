//
// Created by Łukasz on 20.01.2024.
//
#include <fftw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>

// Funkcja do obserwacji widma sygnału
void plot_freq(const std::vector<std::complex<double>>& signal, double fs) {
    int N = signal.size(); // Number of points in FFT
    fftw_complex *in, *out; // Declare input and output arrays for FFTW
    fftw_plan p;

    // Allocate memory for input and output arrays
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // Copy signal into 'in' array
    for (int i = 0; i < N; i++) {
        in[i][0] = std::real(signal[i]); // Real part
        in[i][1] = std::imag(signal[i]); // Imaginary part
    }

    // Create plan for FFT. Note that FFTW_FORWARD could be replaced with FFTW_BACKWARD for inverse FFT
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Execute FFT
    fftw_execute(p);

    // Calculate magnitude spectrum and print it
    for (int i = 0; i < N; i++) {
        double mag = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]); // Magnitude of FFT output
        double freq = i * fs / N; // Frequency bin
        std::cout << "Frequency: " << freq << " Hz, Magnitude: " << mag << std::endl;
    }

    // Cleanup
    fftw_destroy_plan(p);
    fftw_free(in); 
    fftw_free(out);

    for (const auto& sample : signal) {
        std::cout << "Real: " << std::real(sample) << " Imag: " << std::imag(sample) << std::endl;
    }
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

