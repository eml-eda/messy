#ifndef LUT_H
#define LUT_H

class LUT
{
    public:
        LUT(const double* k, const double* v, int sz); // Constructor
        ~LUT(); // Destructor
        double get_val(double query);
    
    private:
        double* key;
        double* val;
        int size;

};

#endif
