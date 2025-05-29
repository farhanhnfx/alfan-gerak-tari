#include <alfan_gerak_tari/core/kepala.h>
#include <alfan_gerak_tari/core/servo_manager.h>
#include <alfan_gerak_tari/globals.h>
#include <alfan_gerak_tari/utils/convert_utils.h>

void Kepala::setPosition(int gp_kepala41, int gp_kepala42, int gp_kepala43, float speed) {
    // Set posisi servo kepala
    ServoManager::goal_positions[41] = alfan::ConvertUtils::degreeToValueXL320(gp_kepala41 + Default[41]);
    ServoManager::goal_positions[42] = alfan::ConvertUtils::degreeToValueXL320(gp_kepala42 + Default[42]);
    ServoManager::goal_positions[43] = alfan::ConvertUtils::degreeToValueXL320(gp_kepala43 + Default[43]);

    // Hitung kecepatan gerakan servo
    ServoManager::calculateSpeed(41, ServoManager::goal_positions[41], speed);
    ServoManager::calculateSpeed(42, ServoManager::goal_positions[42], speed);
    ServoManager::calculateSpeed(43, ServoManager::goal_positions[43], speed);
}

void Kepala::setPosition(KepalaStruct kepala) {
    setPosition(kepala.gp_degree_41, kepala.gp_degree_42, kepala.gp_degree_43, kepala.speed);
}