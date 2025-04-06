#include <alfan_gerak_tari/core/tangan.h>
#include <alfan_gerak_tari/core/servo_manager.h>
#include <alfan_gerak_tari/io/file_manager.h>
#include <map>

void Tangan::bacaGerak(int gerakCounter, float speed) {
    processMotionFrame(gerakCounter, speed);
}

void Tangan::processMotionFrame(int gerakCounter, float speed) {
    map<uint8_t, int32_t> dataMotionFrame = FileManager::parseFile(gerakCounter);

    for (const auto& [id, goalPositionValue] : dataMotionFrame) {
        ServoManager::goal_positions[id] = goalPositionValue;
            
        /* Maksimal Value Goal Position XL-320 = 1023
            Sedangkan Maksimum MX-28 = 4095 */
        if (id == 21 || id == 31) {
            if (goalPositionValue > 4095) {
                ServoManager::goal_positions[id] = 4095;
            } else if (goalPositionValue < 0) {
                ServoManager::goal_positions[id] = 0;
            }
        }
        else {
            if (goalPositionValue > 1023) {
                ServoManager::goal_positions[id] = 1023;
            } else if (goalPositionValue < 0) {
                ServoManager::goal_positions[id] = 0;
            }
        }
            
        ServoManager::calculateSpeed(id, ServoManager::goal_positions[id], speed);
    }
}