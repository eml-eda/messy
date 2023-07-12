#include "lut.h"


LUT::LUT(const double* k, const double* v, int sz)
{
    key = new double[sz];
    val = new double[sz];
    int i;
    
    // Init
    for (i = 0; i < sz; i++)
    {
        this->key[i] = k[i];
        this->val[i] = v[i];
    }
    this->size = sz;
}


LUT::~LUT()
{
    delete[] key;
    delete[] val;
}


double LUT::get_val(double query)
{
    int i;
    double dx;
    double dy;
    
    // Find i, such that key[i] <= query < key[i+1]
    for (i = 0; i < size - 1; i++)  // TODO: Use Binary Search here instead
    {
        if (key[i+1] > query)
        {
            break;
        }
    }
    
    // Interpolate
    dx = key[i+1] - key[i];
    dy = val[i+1] - val[i];
    return val[i] + (query - key[i]) * dy / dx;
}
