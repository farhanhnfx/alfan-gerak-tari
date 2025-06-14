#ifndef ALFAN_PERUT_H
#define ALFAN_PERUT_H

/*
 * Custom Structure
 * untuk menampung Goal Position masing-masing ID 17 dan speed-nya
 * akan diproses di gerak tari
*/
struct PerutStruct {
    int gp_degree_17;
    float speed;
};

class Perut {
    public:
    /*
     * @brief Mengatur posisi servo perut
     * 
     * @param gp_perut17 Posisi servo perut 17
     * @param speed Kecepatan gerakan servo (misal 5 maka diharapkan servo dapat mencapai goal position dalam 500 milidetik)
     * @note Posisi servo perut 17 dalam derajat
    */
    void setPosition(int gp_perut17, float speed);
    /*
     * @brief Mengatur posisi servo perut
     * @param perut Data perubahan posisi perut beserta speed-nya (melalui custom Struct)
    */
    void setPosition(PerutStruct perut);
};

#endif