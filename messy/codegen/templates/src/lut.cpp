#include <lut.hpp>

/**
 * @brief Constructor for the LUT class.
 *
 * Initializes the lookup table by copying the key and value arrays and storing the size.
 *
 * @param k A pointer to the array of keys.
 * @param v A pointer to the array of corresponding values.
 * @param sz The number of key-value pairs.
 */
LUT::LUT(const double *k, const double *v, int sz) {
    key = new double[sz]; ///< Dynamically allocate memory for keys.
    val = new double[sz]; ///< Dynamically allocate memory for values.
    int i;

    // Initialize the key-value pairs
    for (i = 0; i < sz; i++) {
        this->key[i] = k[i]; ///< Copy each key into the LUT.
        this->val[i] = v[i]; ///< Copy each value into the LUT.
    }
    this->size = sz; ///< Store the size of the lookup table.
}

/**
 * @brief Destructor for the LUT class.
 *
 * Frees the memory allocated for keys and values.
 */
LUT::~LUT() {
    delete[] key;
    delete[] val;
}

/**
 * @brief Retrieves the interpolated value for a given query key.
 * 
 * Performs a linear interpolation between the two closest key-value pairs if the query 
 * key falls between them.
 * 
 * @param query The key for which the corresponding value is to be interpolated.
 * @return The interpolated value based on the query key.
 */
double LUT::get_val(double query) {
    int i;
    double dx;
    double dy;

    // Find i, such that key[i] <= query < key[i+1]
    for (i = 0; i < size - 1; i++)
    {
        if (key[i + 1] > query) {
            break;
        }
    }

    // Interpolate
    dx = key[i + 1] - key[i];
    dy = val[i + 1] - val[i];
    return val[i] + (query - key[i]) * dy / dx;
}