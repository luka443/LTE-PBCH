# Decoding 4G LTE Physical Broadcast Channel
I'm using LTE signal stored here - [Data/LTE_TM3p1_10MZ](Data/LTE_TM3p1_10MZ_18p22dBFS.txt)
## Opis projektu
This C++ project analyzes the Physical Broadcast Channel (PBCH) spectrum in Long Term Evolution (LTE) systems. PBCH carries essential information for mobile stations to synchronize with and gain access to the network. The program conducts several critical operations on the signal:
```
1.Loading signal samples from a text file.
2.Observing the signal's spectrum using Fast Fourier Transform (FFT) and visualizes the outcomes.
3.Optional operations such as low-pass filtering, correcting the ADC sampling frequency, and introducing signal disturbances.
4.Spectrum analysis to identify and correct frequency shifts.
```
## Prerequisites Installation
Running this project requires the matplotlibcpp and FFTW3 libraries. Installation instructions for these libraries on Linux systems are provided below.

### FFTW3 Installation
```
Download fftw3 from [fftw3](https://www.fftw.org/)
```
```
extract .tar to folder
```
```
sudo ./configure --enable-threads --enable-avx
```
```
make
```
```
make install
```
