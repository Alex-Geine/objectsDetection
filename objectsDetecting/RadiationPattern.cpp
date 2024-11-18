#include "RadiationPattern.h"

#define Pi_2 (M_PI * 2)

// Constructor
Point3::Point3(double x, double y, double z) : m_x(x), m_y(y), m_z(z)
{
}

// Get z coordinate
void Point3::GetZCoord(double r)
{
    m_z = sqrt(r * r - m_x * m_x - m_y * m_y );

    //std::cout << m_x << " " << m_y << " " << m_z << std::endl;

    return;
}

// Get distanse between two points
double Point3::GetR(Point3 p)
{
    //std::cout << "m_x/p_x: " << m_x << "|" << p.m_x
    //          << ", m_y/p_y: " << m_y << "|" << p.m_y
    //          << ", m_z/p_z: " << m_z << "|" << p.m_z << " ";
    return sqrt((m_x - p.m_x) * (m_x - p.m_x) + (m_y - p.m_y) * (m_y - p.m_y) + (m_z - p.m_z) * (m_z - p.m_z));
}

// condtructor
Receiver::Receiver(Point3 p) : m_coord(p)
{
    //std::cout << "m_coord. p_x: " << m_coord.m_x
    //          << "p_y: " << m_coord.m_y
    //          << "p_z: " << m_coord.m_z << std::endl; 
}

// Find amplitude from emmiter
std::complex<double> Receiver::FindAmpl(Point3 p, double l)
{    
    double r = p.GetR(m_coord);

    //std::cout << "r: " << r << " ";

    std::complex<double> phase(0.0, r * Pi_2 / l);

    
    //std::cout << ", abs: " << std::abs(res) << std::endl;
    //std::cout << "exp: " << exp(-phase) / r << std::endl;
    return exp(-phase) / r;
}

// Constructor
Antenna::Antenna(std::vector<std::vector<bool>> receiversLocation, double d) : m_d(d)
{
    uint64_t str = receiversLocation.size();
    uint64_t col = receiversLocation[0].size();

    double left = - (double)col / 2. * m_d + m_d / 2.;
    double bot  = - (double)str / 2. * m_d + m_d / 2.;

    double x = 0;
    double y = 0;
    double z = 0;

    //std::cout << "antenna constructor: " << std::endl;

    //std::cout << "bot: " << bot << ", left: " << left << std::endl;
    //std::cout << "m_d: " << m_d << ", str: " << str << ", col: " << col << std::endl;

    for (uint64_t i = 0; i < str; ++i)
    {
        for (uint64_t j = 0; j < col; ++j)
            if (receiversLocation[i][j])
            {
                x = bot + (double)i * m_d;
                y = left + (double)j * m_d;
                //std::cout << "x: " << x << ", y: " << y << std::endl;
                //std::cout << "i: " << i << ", j: " << j << ", x: " << bot + i * m_d << ", y:" << left + j * m_d << std::endl;
                m_receivers.push_back(Receiver(Point3(x, y)));
            }
        //std::cout << std::endl;
    }
   // std::cout << std::endl;
}

// Get Amplitude of signal
double Antenna::GetSigAmpl(Point3 p, double l)
{
    std::complex<double> sum{0,0};
    for (uint64_t i = 0; i < m_receivers.size(); ++i)
        sum += m_receivers.at(i).FindAmpl(p, l);

    //std::cout << "Sum: " << sum << std::endl;
    return std::abs(sum);
}

bool check_cirle(double x, double y, double r)
{
    if (x * x + y * y > r * r)
        return false;

    return true;
}

// Function for creating radiation pattern
// [in] locations - 2d array with antennas locations (true/false)
// [in] l - wave length
// [in] k - paramener for distance between receivers (d = k * l)
// [in] r - radius of area
// [in] N - number of points to compute
// return 1d array with amplitude values
double* g_getDiagram(std::vector<std::vector<bool>> locations, double l, double k, double r, int n)
{
    double* ampl = new double[n * n];

    // Init antenna
    Antenna antenna(locations, l * k);

    double dx = 2. * r / ((double)n - 1.);

    double left = -r;  // Left coordinate of square
    double bot  = left;     // Bot coordinate of square

    //std::cout << "dx: " << dx << ", left: " << left << ", bot: " << bot << std::endl;

    double x = 0;
    double y = 0;

    for (int raw = 0; raw < n; ++raw)
    {
        x = left + raw * dx;
        //std::cout << "x: " << x << std::endl;

        for (int col = 0; col < n; ++col)
        {
            y = bot + dx * col;
            //std::cout << "y: " << y << std::endl;

            if (check_cirle(x, y, r))
            {
                Point3 p (x, y);
                p.GetZCoord(r);
                //std::cout << "p_x: " << p.m_x << ", p_y: " << p.m_y << ", p_z: " << p.m_z << std::endl;
                ampl[raw * n + col] = antenna.GetSigAmpl(p, l);
            }
            else
            {
                ampl[raw * n + col] = 0.;
            }
            //td::cout << std::endl;
        }
        //std::cout << std::endl;
    }
/*
    std::cout << "amplitudes: " << std::endl;
    
    for (int raw = 0; raw < n; ++raw)
    {
        for (int col = 0; col < n; ++col)
            std::cout << ampl[raw * n + col] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
*/
    return ampl;
}
