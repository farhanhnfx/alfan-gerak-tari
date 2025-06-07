#ifndef ALFAN_KAKI_H
#define ALFAN_KAKI_H

#include <alfan_gerak_tari/core/kinematika.h>

struct KakiStruct {
    double kaki_kanan_x;
    double kaki_kanan_y;
    double kaki_kanan_z;
    
    double kaki_kiri_x;
    double kaki_kiri_y;
    double kaki_kiri_z;

    double speeds;
};

class Kaki {
    public:
    void setPosition(std::vector<double> leg_position, Leg leg, float speed);
    void setPosition(double x, double y, double z, Leg leg, float speed);
    void setPosition(KakiStruct kaki);
};

#endif