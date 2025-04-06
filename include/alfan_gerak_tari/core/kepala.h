#ifndef ALFAN_KEPALA_H
#define ALFAN_KEPALA_H

/*
 * Custom Structure
 * untuk menampung Goal Position masing-masing ID 41, 42, 43, dan speed-nya
 * akan diproses di gerak tari
*/
struct KepalaStruct {
    int gp_degree_41;
    int gp_degree_42;
    int gp_degree_43;
    int speed;
};

class Kepala {
    public:
    /*
     * @brief Mengatur posisi servo kepala
     * 
     * @param gp_kepala41 Posisi servo kepala 41
     * @param gp_kepala42 Posisi servo kepala 42
     * @param gp_kepala43 Posisi servo kepala 43
     * @param speed Kecepatan gerakan servo (misal 5 maka diharapkan servo dapat mencapai goal position dalam 500 milidetik)
     * @note Posisi servo kepala 41, 42, dan 43 dalam derajat
    */
    void setPosition(int gp_kepala41, int gp_kepala42, int gp_kepala43, int speed);
    /*
     * @brief Mengatur posisi servo kepala
     * @param kepala Data perubahan posisi kepala beserta speed-nya (melalui custom Struct)
    */
    void setPosition(KepalaStruct kepala);
};

#endif