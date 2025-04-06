#ifndef ALFAN_CONVERT_UTILS_H
#define ALFAN_CONVERT_UTILS_H

#include <cmath>

/*
 * KONSTANTA PERHITUNGAN 
 * Maksimal sudut dibagi dengan maksimal value dari servo
*/
const float CONST_MX28 = 360.0 / 4095.0;    // 0.088
const float CONST_XL320 = 300.0 / 1023.0;   // 0.293


/* Digunakan untuk konversi dari
   derajat ke value ataupun
   sebaliknya 
*/
namespace alfan {
    class ConvertUtils {
        public:
        /*
         * @brief Fungsi untuk mengkonversi derajat ke value (MX-28)
         * @param degree Derajat yang akan dikonversi
         * @return Value yang dihasilkan
        */
        static int degreeToValueMX28(float degree) {
            return degree / CONST_MX28;
        }
    
        /*
         * @brief Fungsi untuk mengkonversi derajat ke value (XL-320)
         * @param degree Derajat yang akan dikonversi
         * @return Value yang dihasilkan
        */
        static int degreeToValueXL320(float degree) {
            return degree / CONST_XL320;
        }
    
        /*
         * @brief Fungsi untuk mengkonversi value ke derajat (MX-28)
         * @param value Value yang akan dikonversi
         * @return Derajat yang dihasilkan
        */
        static int valueToDegreeMX28(int value) {
            return abs(floor(value * CONST_MX28));
        }
    
        /*
         * @brief Fungsi untuk mengkonversi value ke derajat (XL-320)
         * @param value Value yang akan dikonversi
         * @return Derajat yang dihasilkan
        */
        static int valueToDegreeXL320(int value) {
            return abs(floor(value * CONST_XL320));
        }
    };
}

#endif