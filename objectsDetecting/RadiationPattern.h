#ifndef RADIATION_PATTERN
#define RADIATION_PATTERN

#include <iostream>
#include <vector>
#include <complex>

// 3d Point struct
class Point3
{
    public:    // variables

    double m_x = 0;    // x coordinate
    double m_y = 0;    // y coordinate
    double m_z = 0;    // z coordinate

    public:    // methods

    // Constructor
    Point3(double x, double y, double z = 0);

    //Default construcor
    Point3(){};

    // Get z coordinate
    void GetZCoord(double r);

    // Get distanse between two points
    double GetR(Point3 p);
};

// Receiver structure
struct Receiver
{
    private:    // variables

    Point3 m_coord;    // coordinates of receiver

    public:    // methods

    // condtructor
    Receiver(Point3 p);

    // Find amplitude from emmiter
    std::complex<double> FindAmpl(Point3 p, double l);
};

// Antenna structure
struct Antenna
{
    private:    // variables

    std::vector<Receiver> m_receivers;    // array with receivers
    double                m_d = 0;        // antenna period

    public:    // methods

    // Constructor
    Antenna(std::vector<std::vector<bool>> receiversLocation, double d);

    // Get Amplitude of signal
    double GetSigAmpl(Point3 p, double l);
};

// Function for creating radiation pattern
double* g_getDiagram(std::vector<std::vector<bool>> locations, double l, double k, double r, int N);

#endif // RADIATION PATTERN