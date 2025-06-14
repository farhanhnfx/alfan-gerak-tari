#include <alfan_gerak_tari/core/kaki.h>
#include <alfan_gerak_tari/core/servo_manager.h>
#include <alfan_gerak_tari/globals.h>
#include <alfan_gerak_tari/utils/convert_utils.h>

void Kaki::setPosition(std::vector<double> leg_position, Leg leg, float speed) {
    
    Kinematika kin;  // Instance dari kelas Kinematika
    
    // Hitung inverse kinematics (hasil sudut dalam radian)
    Eigen::VectorXd angles = kin.computeIK(leg_position, leg);

    // Radian to value
    angles = angles * (180/M_PI) * (4095/360);
    
    // Set posisi servo kaki
    if (leg == Leg::RIGHT) { // ID 1-6
        for (int i = 0; i < 6; i++) {
            bool isReversedID = ((i+1) == 1) || ((i+1) == 2) || ((i+1) == 5) || ((i+1) == 11) || ((i+1) == 12) || ((i+1) == 13) || ((i+1) == 14);

            if (isReversedID) {
                ServoManager::goal_positions[i+1] = alfan::ConvertUtils::degreeToValueMX28(Default[i+1]) - (int32_t)angles[i];
            } else {
                ServoManager::goal_positions[i+1] = alfan::ConvertUtils::degreeToValueMX28(Default[i+1] ) + (int32_t)angles[i];
            }
            ServoManager::calculateSpeed(i+1, ServoManager::goal_positions[i+1], speed);
        }

    } else {  // ID 11-16
        for (int i = 0; i < 6; i++) {
            bool isReversedID = ((i+11) == 1) || ((i+11) == 2) || ((i+11) == 5) || ((i+11) == 11) || ((i+11) == 12) || ((i+11) == 13) || ((i+11) == 14);

            if (isReversedID) {
                ServoManager::goal_positions[i+11] = alfan::ConvertUtils::degreeToValueMX28(Default[i+11]) - (int32_t)angles[i];
            } else {
                ServoManager::goal_positions[i+11] = alfan::ConvertUtils::degreeToValueMX28(Default[i+11]) + (int32_t)angles[i];
            }
            ServoManager::calculateSpeed(i+11, ServoManager::goal_positions[i+11], speed);
        }
    }
}

void Kaki::setPosition(double x, double y, double z, Leg leg, float speed){
    setPosition(std::vector<double> {x, y, z}, leg, speed);
}

void Kaki::setPosition(KakiStruct kaki){
    setPosition(kaki.kaki_kanan_x, kaki.kaki_kanan_y, kaki.kaki_kanan_z, Leg::RIGHT, kaki.speeds);
    setPosition(kaki.kaki_kiri_x, kaki.kaki_kiri_y, kaki.kaki_kiri_z, Leg::LEFT, kaki.speeds);
}