#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>

// Funkcja realizująca bit-reversal
void bit_reverse(std::vector<std::complex<double>>& data) {
    int N = data.size();
    int bit_len = std::log2(N);

    for (int i = 0; i < N; ++i) {
        int j = 0;
        for (int bit = 0; bit < bit_len; ++bit) {
            j |= ((i >> bit) & 1) << (bit_len - 1 - bit);
        }
        if (j > i) std::swap(data[i], data[j]);
    }
}

// Funkcja wykonująca algorytm FFT
void fft(std::vector<std::complex<double>>& data, int inverse = 1) {
    int N = data.size();
    int bit_len = std::log2(N);

    // Przygotowanie tablicy "butterfly"
    std::vector<std::complex<double>> omega(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        double angle = 2.0 * M_PI * i / N * inverse;
        omega[i] = std::polar(1.0, angle);
    }

    // Wykonanie bit-reversal
    bit_reverse(data);

    // Wykonanie kolejnych etapów algorytmu FFT
    for (int level = 1; level <= bit_len; ++level) {
        int step = 1 << level;
        int half_step = step / 2;
        for (int j = 0; j < N; j += step) {
            for (int k = 0; k < half_step; ++k) {
                std::complex<double> even = data[j + k];
                std::complex<double> odd = data[j + k + half_step] * omega[k * (N / step)];
                data[j + k] = even + odd;
                data[j + k + half_step] = even - odd;
            }
        }
    }

    // Jeśli wykonujemy odwrotną FFT, należy podzielić wynik przez N
    if (inverse == -1) {
        for (int i = 0; i < N; ++i) {
            data[i] /= N;
        }
    }
}

// Funkcja rysująca wykres
void plot_freq(std::vector<std::complex<double>>& signal, double fs) {
    int N = signal.size();

    // Obliczenie FFT
    fft(signal);

    // Oś częstotliwości w MHz
    std::vector<double> f(N);
    for (int i = 0; i < N; ++i) {
        f[i] = (fs / N * (i - N / 2)) / 1e+6;
    }

    // Inicjalizacja okna SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "Wykres DFT");

    // Tworzenie linii dla wykresu
    std::vector<sf::Vertex> points(N);
    for (int i = 0; i < N; ++i) {
        double magnitude = std::abs(signal[i]);
        points[i] = sf::Vertex(sf::Vector2f(400 + f[i] * 20, 300 - 20 * std::log10(magnitude)), sf::Color::Blue);
    }

    // Pętla główna okna
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Wyczyszczenie okna
        window.clear(sf::Color::White);

        // Rysowanie wykresu
        window.draw(&points[0], points.size(), sf::LinesStrip);

        // Wyświetlenie okna
        window.display();
    }
}

int main() {
    // Zmienne kontrolujące przetwarzanie sygnału
    int do_LPfilter = 0; // 0/1 - filtr dolnoprzepustowy, przepuszcza tylko używane nośniki PBCH
    int do_disturbing = 0; // 0/1 - zakłócanie sygnału
    int do_ADCcorrect = 0; // 0/1 - korekcja częstotliwości próbkowania ADC
    int do_plots = 1; // 0/1 - wyświetlanie wykresów

    // Wczytanie danych z pliku .txt
    std::ifstream inputFile("Data/LTE_TM3p1_10MZ_18p22dBFS.txt"); // Change directory if needed!!
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

    // Parametry LTE
    int LTEmode = 0; // Tryb LTE
    double LTEtab[7][6] = {
            {1.4, 3, 5, 10, 15, 20},     // Szerokość kanału [MHz]
            {6, 15, 25, 50, 75, 100},    // Liczba bloków zasobów (RBs)
            {72, 180, 300, 600, 900, 1200}, // Liczba używanych podnośnych
            {128, 256, 512, 1024, 1536, 2048}, // Rozmiar DFT/FFT
            {15, 15, 15, 15, 15, 15},    // Odstęp międzynośny [kHz]
            {1.92, 3.84, 7.68, 15.36, 23.04, 30.72}, // Częstotliwość próbkowania [MHz]
            {960, 1920, 3840, 7680, 11520, 15360}    // Liczba próbek na slot
    };

    double CPtab[3][2] = {
            {5.21, 4.69},    // NORMAL CP: 15 kHz, 7 symboli/slot
            {16.67, 16.67},  // EXTENDED CP: 15 kHz, 6 symboli/slot
            {33.33, 33.33}   // EXTENDED CP: 7.5 kHz, 6 symboli/slot
    };

    // Ustalenie trybu LTE na podstawie częstotliwości próbkowania
    if (fs_lte == 1.92e6) LTEmode = 1;
    else if (fs_lte == 3.84e6) LTEmode = 2;
    else if (fs_lte == 7.68e6) LTEmode = 3;
    else if (fs_lte == 15.36e6) LTEmode = 4;
    else if (fs_lte == 23.04e6) LTEmode = 5;
    else if (fs_lte == 30.72e6) LTEmode = 6;

    // Wartości parametrów związanych z trybem LTE
    int CPmode = 1; // Tryb długości prefiksu cyklicznego
    int Nfft = LTEtab[3][LTEmode - 1]; // Długość FFT
    fs_lte = LTEtab[5][LTEmode - 1] * 1e6; // Częstotliwość próbkowania [Hz]
    double df_lte = LTEtab[4][LTEmode] * 1e3; // Odstęp międzynośny [Hz]

    for (int i = 0; i < N - 1; i++) {
        signal[i] = signal[i] * std::exp(std::complex<double>(0, -2 * M_PI * i * 5 * df_lte / fs));
    }

    plot_freq(signal, fs);

    // Tutaj można kontynuować przetwarzanie sygnału w języku C++
    return 0;
}
