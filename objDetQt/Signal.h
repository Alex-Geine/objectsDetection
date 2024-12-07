#ifndef SIGNAL_H
#define SIGNAL_H

#include <iostream>
#include <complex>
#include <math.h>
#include <time.h>
#include <cstdint>

#define  N                      1024                         // Base dimension of signal
#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT              -1                           // Direct transform.
#define  FT_INVERSE             1                            // Inverse transform.

// Base class for signals representation
class Signal
{
    private:    // variables

    uint64_t               m_colomns   = 0;          //!< Number of colomns in matrix
    uint64_t               m_strings   = 0;          //!< Number of strings in matrix
    std::complex<double>** m_dataArray = nullptr;    //!< Array with points

    private:    // methods

    // Delete data array function
    void DeleteDataArray();

    public:

    // Delault constructor
    Signal();

    // Constructor with zero data
    Signal(uint64_t colomns, uint64_t strings);

    // Default destructor
    ~Signal();

    // Copy constructor
    Signal(const Signal& sig);

    // Move constructor
    Signal(Signal&& sig);

    // Copy operator
    Signal operator=(const Signal& sig);

    // Move operator
    Signal operator=(Signal&& sig);

    // Get number of colomns
    uint64_t GetNumberOfColomns();

    // Get number of strings
    uint64_t GetNumberOfStrings();

    // Get energy of signal
    double GetEnergy();

    // Set number of colomns
    void SetNumberOfColomns(uint64_t colomns);

    // Set number of strings
    void SetNumberOfStrings(uint64_t strings);

    // Get data array
    std::complex<double>** GetDataArray();

    // Get picture array
    uint8_t* GetPicture();

    // Get picture array
    bool GetPicture(uint8_t* pic, bool isInverse);

    // Get square error
    double GetSquareError(Signal& sig);

    // Get pixel error
    double GetPixelError(Signal& sig);
};

// Class witch representation Gauss signal
class GaussSignal : public Signal
{
    private:    // variables

    private:    // methods

    // Gauss function
    double Gauss(double x, double y, double ampl, double x0, double y0, double xSigma, double ySigma);

    public:

    // Delault constructor
    GaussSignal();

    // Default destructor
    ~GaussSignal();

    // Copy constructor
    GaussSignal(const GaussSignal& sig);

    // Move constructor
    GaussSignal(GaussSignal&& sig);

    // Copy operator
    GaussSignal operator=(const GaussSignal& sig);

    // Move operator
    GaussSignal operator=(GaussSignal&& sig);

    // Constructor
    GaussSignal( uint64_t numberOfGauss,
                 double* x0Array, double* y0Array, double* amplArray,
                 double* sigmaXArray, double* sigmaYArray );

    // Constructor
    GaussSignal( uint64_t n, uint64_t numberOfGauss,
                 double* x0Array, double* y0Array, double* amplArray,
                 double* sigmaXArray, double* sigmaYArray );
};

// Class whitch representation test signal (square)
class TestSignal : public Signal
{
    private:    // variables

    public:     // methods

    // Delault constructor
    TestSignal();

    // Default destructor
    ~TestSignal();

    // Copy constructor
    TestSignal(const TestSignal& sig);

    // Move constructor
    TestSignal(TestSignal&& sig);

    // Copy operator
    TestSignal operator=(const TestSignal& sig);

    // Move operator
    TestSignal operator=(TestSignal&& sig);

    // Constructor
    TestSignal(double weight);

    // Constructor
    TestSignal(double weight, uint64_t n);
};

class RealSignal : public Signal
{
    private:    // variables

    uint64_t        m_actualColomns = 0;          //!< Number of colomns of resized signal
    uint64_t        m_actualStrings = 0;          //!< Number of strings of resized signal

    private:    // methods

    // Cheking powers of two
    uint64_t PowersOfTwo(uint64_t num);

    public:     // methods

    // Delault constructor
    RealSignal();

    // Default destructor
    ~RealSignal();

    // Copy constructor
    RealSignal(const RealSignal& sig);

    // Move constructor
    RealSignal(RealSignal&& sig);

    // Copy operator
    RealSignal operator=(const RealSignal& sig);

    // Move operator
    RealSignal operator=(RealSignal&& sig);

    // Constructor
    RealSignal(uint8_t* dataArray, uint64_t colomns, uint64_t strings);

    // Get number of actual colomns
    uint64_t GetActualNumberOfColomns();

    // Get number of actual strings
    uint64_t GetActualNumberOfStrings();

    // Resize
    void Resize();
};

// Multidimensional FFT Direct
bool g_mfftDir(std::complex<double>** inData, std::complex<double>** outData, uint64_t strings, uint64_t colomns, uint64_t flag);

// Multidimensional FFT Inverce
bool g_mfftInv(std::complex<double>** inData, std::complex<double>** outData, uint64_t strings, uint64_t colomns, bool flag);


// FFT
bool g_fft(std::complex<double>* inData, std::complex<double>* outData, uint64_t size, uint64_t flag);

// Noize function
bool g_noizeSignal(Signal& sig, double Db);

// Filtration function
Signal* g_squareFiltration(Signal& sig, double Db, uint64_t& x, uint64_t& y);

// Interpolation function
Signal* g_linInterpol(Signal& sig, uint64_t width, uint64_t height);

// Interpolation function with real interpolation
Signal* g_linInterpolReal(Signal& sig, uint64_t width, uint64_t height);

#endif    // SIGNAL_H
