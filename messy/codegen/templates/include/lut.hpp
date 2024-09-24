#ifndef LUT_H
#define LUT_H

/**
 * @class LUT
 * @brief A class that implements a lookup table (LUT) for querying values based on keys.
 */
class LUT
{
    public:
        /**
         * @brief Constructor for the LUT class.
         * 
         * Initializes the lookup table with the given key and value arrays.
         * 
         * @param k A pointer to an array of keys.
         * @param v A pointer to an array of corresponding values.
         * @param sz The number of key-value pairs in the lookup table.
         */
        LUT(const double* k, const double* v, int sz);

        /**
         * @brief Destructor for the LUT class.
         * 
         * Cleans up any dynamically allocated memory used by the LUT.
         */
        ~LUT();

        /**
         * @brief Retrieves the value corresponding to a given query key.
         * 
         * @param query The key for which the corresponding value is to be found.
         * @return The value associated with the query key, or a default value if the key is not found.
         */
        double get_val(double query);
    
    private:
        double* key; ///< Pointer to the array of keys.
        double* val; ///< Pointer to the array of values.
        int size; ///< The size of the lookup table (number of key-value pairs).
};

#endif
