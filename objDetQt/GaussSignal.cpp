#include "Signal.h"

// class GaussSignal

// Default constructor
GaussSignal::GaussSignal(){};

// Default destructor
GaussSignal::~GaussSignal(){};

// Copy constructor
GaussSignal::GaussSignal(const GaussSignal& sig) : Signal(sig){};

// Move constructor
GaussSignal::GaussSignal(GaussSignal&& sig) : Signal(sig){};

// Copy operator
GaussSignal GaussSignal::operator=(const GaussSignal& sig)
{
    Signal::operator=(sig);

    return *this;
};

// Move operator
GaussSignal GaussSignal::operator=(GaussSignal&& sig)
{
    Signal::operator=(sig);

    return *this;
};

// Constructor
GaussSignal::GaussSignal ( uint64_t numberOfGauss,
                           double* x0Array, double* y0Array,  double* amplArray,
                           double* sigmaXArray, double* sigmaYArray ) : Signal(N, N)
{
    if ((numberOfGauss != 0))
    {
        std::complex<double>** data  = Signal::GetDataArray();
        double                 left  = -0.5;
        double                 right = 0.5;
        double                 top   = right;
        double                 bot   = left;
        double                 dx    = (right - left) / (double)N;
        double                 dy    = (top - bot) / (double)N;
        double                 x     = left;
        double                 y     = bot;

        double sum = 0;

        for (uint64_t i = 0; i < N; ++i)
        {
            y = bot;
            for (uint64_t j = 0; j < N; ++j)
            {
                 for (uint64_t k = 0; k < numberOfGauss; ++k)
                     sum += Gauss(x, y, amplArray[k], x0Array[k], y0Array[k], sigmaXArray[k], sigmaYArray[k]);

                 data[i][j].real(sum);
                 sum = 0;

                 y += dy;
            }

            x += dx;
        }

    }
};

// Constructor
GaussSignal::GaussSignal ( uint64_t n, uint64_t numberOfGauss,
                           double* x0Array, double* y0Array,  double* amplArray,
                           double* sigmaXArray, double* sigmaYArray ) : Signal(n, n)
{
    if ((numberOfGauss != 0))
    {
        std::complex<double>** data  = Signal::GetDataArray();
        double                 left  = -0.5;
        double                 right = 0.5;
        double                 top   = right;
        double                 bot   = left;
        double                 dx    = (right - left) / (double)n;
        double                 dy    = (top - bot) / (double)n;
        double                 x     = left;
        double                 y     = bot;

        double sum = 0;

        for (uint64_t i = 0; i < n; ++i)
        {
            y = bot;
            for (uint64_t j = 0; j < n; ++j)
            {
                 for (uint64_t k = 0; k < numberOfGauss; ++k)
                     sum += Gauss(x, y, amplArray[k], x0Array[k], y0Array[k], sigmaXArray[k], sigmaYArray[k]);

                 data[i][j].real(sum);
                 sum = 0;

                 y += dy;
            }

            x += dx;
        }

    }
};

// Gauss function
double GaussSignal::Gauss(double x, double y, double ampl, double x0, double y0, double xSigma, double ySigma)
{
   return ampl * exp(- ( (x - x0) * (x - x0) / (xSigma * xSigma) + (y - y0) * (y - y0) / (ySigma * ySigma)));
};


