#include <alfan_gerak_tari/core/perut.h>
#include <alfan_gerak_tari/core/servo_manager.h>
#include <alfan_gerak_tari/globals.h>
#include <alfan_gerak_tari/utils/convert_utils.h>

void Perut::setPosition(int gp_perut17, float speed) {
    // Set posisi servo perut
    ServoManager::goal_positions[17] = alfan::ConvertUtils::degreeToValueXL320(gp_perut17 + Default[17]);

    // Hitung kecepatan gerakan servo
    ServoManager::calculateSpeed(17, ServoManager::goal_positions[17], speed);
}

void Perut::setPosition(PerutStruct perut) {
    setPosition(perut.gp_degree_17, perut.speed);
}