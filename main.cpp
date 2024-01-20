//
// Created by Łukasz on 20.01.2024.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>

// Funkcja do obserwacji widma sygnału
void plot_freq(const std::vector<std::complex<double>>& signal, double fs) {
    // Tutaj można umieścić kod do analizy widma sygnału
    // Na przykład wykorzystując bibliotekę FFT lub inny algorytm

    // Przykład: Wypisanie próbek sygnału
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
    std::ifstream inputFile("D:\\LTE-5G-C-\\LTE_TM3p1_10MZ_18p22dBFS.txt"); // Change directory!!
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

    // Obserwacja widma sygnału
    if (do_plots) {
        plot_freq(signal, fs);
    }

    // Tutaj można kontynuować przetwarzanie sygnału w języku C++

    return 0;
}

