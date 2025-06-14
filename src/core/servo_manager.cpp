#include <alfan_gerak_tari/core/servo_manager.h>
#include <alfan_gerak_tari/utils/convert_utils.h>
#include <alfan_gerak_tari/globals.h>
#include <rclcpp/rclcpp.hpp>


DynamixelWorkbench ServoManager::dxl_wb;
bool ServoManager::is_init = false;
int32_t ServoManager::goal_positions[45] = {0};
int32_t ServoManager::moving_speeds[45] = {0};
int32_t ServoManager::latest_position_in_degree[45] = {0};
uint32_t ServoManager::degree_difference[45] = {0};
uint8_t ServoManager::mx28_ids[15] = {1, 2, 3, 4, 5, 6, 
                                      11, 12, 13, 14, 15, 16,
                                      17,
                                      21, 31};

uint8_t ServoManager::xl320_ids[13] = {22, 23, 24, 25, 26,
                                       32, 33, 34, 35, 36,
                                       41, 42, 43};

// uint8_t ServoManager::mx28_ids[2] = { 21, 31};

// uint8_t ServoManager::xl320_ids[10] = {22, 23, 24, 25, 26,
//                                        32, 33, 34, 35, 36};

bool ServoManager::init() {
    const char* log;
    if (is_init) return true;

    bool result = false;
    result = dxl_wb.init(usb_port, baud_rate, &log);
    if (result == true) {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Dynamixel Workbench Initialization Success");
        setupSyncWriteHandler();
        is_init = true;
    } else {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Failed to init: %s", log);
        return false;
    }

    // printf("protocol ver: %f\n", dxl_wb.getProtocolVersion());

    setCurrentPositionAsLatestPosition();
    return true;
}

std::vector<uint8_t> ServoManager::getRelatedServoIdsByGroup(Group group) {
    std::vector<uint8_t> related_ids;

    if (group == Group::ALL) {
        related_ids = {  1,  2,  3,  4,  5,  6,
                        11, 12, 13, 14, 15, 16, 17,
                        21, 22, 23, 24, 25, 26,
                        31, 32, 33, 34, 35, 36,
                        41, 42, 43};
    }
    else if (group == Group::TANGAN) {
        related_ids = {21, 22, 23, 24, 25, 26,
                       31, 32, 33, 34, 35, 36};
    }
    else if (group == Group::KAKI) {
        related_ids = { 1,  2,  3,  4,  5,  6,
                       11, 12, 13, 14, 15, 16};
    }
    else if (group == Group::KEPALA) {
        related_ids = {41, 42, 43};
    }
    
    return related_ids;
}

void ServoManager::ping(uint8_t id) {
    const char* log;
    if (dxl_wb.ping(id, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Ping ID %d failed: %s", id, log);
    } else {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Ping ID %d success", id);
    }
}

void ServoManager::ping(Group group) {
    std::vector<uint8_t> related_ids = getRelatedServoIdsByGroup(group);

    for (size_t i = 0; i < related_ids.size(); i++) {
        ping(related_ids.at(i));
    }
}

void ServoManager::pingMX28() {
    if (!is_init) return;
    const char* log;
    for (int i = 0; i < (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])); i++) {
        if (dxl_wb.ping(mx28_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Ping MX-28 ID %d failed: %s", mx28_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Ping MX-28 ID %d success", mx28_ids[i]);
        }
    }

    setCurrentPositionAsLatestPosition();
}
void ServoManager::pingXL320() {
    if (!is_init) return;
    const char* log;
    for (int i = 0; i < (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])); i++) {
        if (dxl_wb.ping(xl320_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Ping XL-320 ID %d failed: %s", xl320_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Ping XL-320 ID %d success", xl320_ids[i]);
        }
    }

    setCurrentPositionAsLatestPosition();
}
void ServoManager::setTorqueOn() {
    if (!is_init) return;
    const char* log;
    for (int i = 0; i < (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])); i++) {
        if (dxl_wb.torqueOn(mx28_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque On MX-28 ID %d failed: %s", mx28_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque On MX-28 ID %d success", mx28_ids[i]);
        }
    }

    for (int i = 0; i < (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])); i++) {
        if (dxl_wb.torqueOn(xl320_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque On XL-320 ID %d failed: %s", xl320_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque On XL-320 ID %d success", xl320_ids[i]);
        }
    }
}
void ServoManager::setTorqueOff() {
    if (!is_init) return;
    const char* log;
    for (int i = 0; i < (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])); i++) {
        if (dxl_wb.torqueOff(mx28_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque Off MX-28 ID %d failed: %s", mx28_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque Off MX-28 ID %d success", mx28_ids[i]);
        }
    }

    for (int i = 0; i < (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])); i++) {
        if (dxl_wb.torqueOff(xl320_ids[i], &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque Off XL-320 ID %d failed: %s", xl320_ids[i], log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque Off XL-320 ID %d success", xl320_ids[i]);
        }
    }
}
void ServoManager::setTorqueOn(uint8_t id) {
    const char* log;
    if (dxl_wb.torqueOn(id, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d failed: %s", id, log);
    } else {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d success", id);
    }
}
void ServoManager::setTorqueOff(uint8_t id) {
    const char* log;
    if (dxl_wb.torqueOff(id, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque Off ID %d failed: %s", id, log);
    } else {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque Off ID %d success", id);
    }
}

void ServoManager::setTorqueOn(Group group) {
    std::vector<uint8_t> related_ids = getRelatedServoIdsByGroup(group);

    const char* log;
    for (size_t i = 0; i < related_ids.size(); i++) {
        if (dxl_wb.torqueOn(related_ids.at(i), &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d failed: %s", related_ids.at(i), log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d success", related_ids.at(i));
        }
    }
    
}

void ServoManager::setTorqueOff(Group group) {
    std::vector<uint8_t> related_ids = getRelatedServoIdsByGroup(group);

    const char* log;
    for (size_t i = 0; i < related_ids.size(); i++) {
        if (dxl_wb.torqueOff(related_ids.at(i), &log) == false) {
            RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d failed: %s", related_ids.at(i), log);
        } else {
            RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Set Torque On ID %d success", related_ids.at(i));
        }
    }
    
}

void ServoManager::setCurrentPositionAsLatestPosition() {
    if (!is_init) return;
    const char* log;
    /*
     * Ambil present position
    */
    int32_t present_value;

    for (int i = 0; i < (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])); i++) {
        dxl_wb.getPresentPositionData(mx28_ids[i], &present_value, &log);
        latest_position_in_degree[mx28_ids[i]] = alfan::ConvertUtils::valueToDegreeMX28(present_value);
    }

    for (int i = 0; i < (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])); i++) {
        dxl_wb.getPresentPositionData(xl320_ids[i], &present_value, &log);
        latest_position_in_degree[xl320_ids[i]] = alfan::ConvertUtils::valueToDegreeXL320(present_value);
    }
}

void ServoManager::setupSyncWriteHandler() {
    const char* log;

    RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "Setup Sync Write Handler");

    /* Index Handler akan bertambah ketika kita menambahkan Sync Write Handler
        kita nanti perlu tau index handler untuk masing-masing register */

    // Index Handler = 0 | MX-28 Profile Velocity | Address = 112 | Length = 4
    if (dxl_wb.addSyncWriteHandler(R_PROFILE_VELOCITY_MX28, 4, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Failed to add Sync Write Handler for Profile Velocity MX-28: %s", log);
    }

    // Index Handler = 1 | MX-28 Goal Position | Address = 116 | Length = 4
    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_MX28, 4, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Failed to add Sync Write Handler for Goal Position MX-28: %s", log);
    }

    // Index Handler = 2 | XL-320 Goal Position | Address = 30 | Length = 2
    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_XL320, 2, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Failed to add Sync Write Handler for Goal Position XL-320: %s", log);
    }

    // Index Handler = 3 | XL-320 Moving Speed | Address = 32 | Length = 2
    if (dxl_wb.addSyncWriteHandler(R_MOVING_SPEED_XL320, 2, &log) == false) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Failed to add Sync Write Handler for Moving Speed XL-320: %s", log);
    }
}

void ServoManager::sendMovementCommands() {
    if (!is_init) return;
    int mx28_goal_positions[] = {goal_positions[1], goal_positions[2], goal_positions[3], goal_positions[4], goal_positions[5], goal_positions[6],
                                 goal_positions[11], goal_positions[12], goal_positions[13], goal_positions[14], goal_positions[15], goal_positions[16],
                                 goal_positions[17],
                                 goal_positions[21], 
                                 goal_positions[31]};
   
    int xl320_goal_positions[] = {goal_positions[22], goal_positions[23], goal_positions[24], goal_positions[25], goal_positions[26],
                                  goal_positions[32], goal_positions[33], goal_positions[34], goal_positions[35], goal_positions[36],
                                 goal_positions[41], goal_positions[42], goal_positions[43]
                                 };
           
    int mx28_moving_speeds[] = {moving_speeds[1], moving_speeds[2], moving_speeds[3], moving_speeds[4], moving_speeds[5], moving_speeds[6],
                                moving_speeds[11], moving_speeds[12], moving_speeds[13], moving_speeds[14], moving_speeds[15], moving_speeds[16],
                                moving_speeds[17],
                                moving_speeds[21], 
                                moving_speeds[31]};
   
    int xl320_moving_speeds[] = {moving_speeds[22], moving_speeds[23], moving_speeds[24], moving_speeds[25], moving_speeds[26],
                                 moving_speeds[32], moving_speeds[33], moving_speeds[34], moving_speeds[35], moving_speeds[36],
                                 moving_speeds[41], moving_speeds[42], moving_speeds[43]
                                 };
    bool result = false;
    const char* log;
   
    // printf("[INFO] Mengirimkan Perintah Gerak\n------------------------------------------------\n");
   
    /* SEMENTARA DI-COMMENT TERLEBIH DAHULU UNTUK DUMMY
        KARENA BELUM ADA SERVO YANG BISA DICOBA UNTUK DIGERAKKAN */
   
    /* Lakukan Sync Write untuk servo terkait 
        Ingat index handler untuk masing-masing register tadi */
           
    // Index Handler = 0 untuk Profile Velocity MX-28
    result = dxl_wb.syncWrite(0, mx28_ids, (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])), mx28_moving_speeds, 1, &log);
    if (!result) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Profile Velocity MX-28 gagal: %s", log);
    } else {
        // printf("[INFO] Sync Write untuk Profile Velocity MX-28 berhasil untuk %lu servo\n", (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])));
    }
   
    // Index Handler = 1 untuk Goal Position MX-28
    result = dxl_wb.syncWrite(1, mx28_ids, (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])), mx28_goal_positions, 1, &log);
    if (!result) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Goal Position MX-28 gagal: %s", log);
    } else {
        // printf("[INFO] Sync Write untuk Goal Position MX-28 berhasil untuk %lu servo\n", (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])));
    }
   
    // Index Handler = 3 untuk Moving Speed XL-320
    result = dxl_wb.syncWrite(3, xl320_ids, (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])), xl320_moving_speeds, 1, &log);
    if (!result) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Moving Speed XL-320 gagal: %s", log);
    } else {
        // printf("[INFO] Sync Write untuk Moving Speed XL-320 berhasil untuk %lu servo\n", (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])));
    }
   
    // Index Handler = 2 untuk Goal Position XL-320
    result = dxl_wb.syncWrite(2, xl320_ids, (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])), xl320_goal_positions, 1, &log);
    if (!result) {
        RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Goal Position XL-320 gagal: %s", log);
    } else {
        // printf("[INFO] Sync Write untuk Goal Position XL-320 berhasil untuk %lu servo\n", (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])));
    }
   
}

void ServoManager::sendMovementCommandsByGroup(Group group) {
    if (group == Group::ALL) {
        sendMovementCommands();
        return;
    }

    std::vector<uint8_t> related_ids = getRelatedServoIdsByGroup(group);

    std::vector<uint8_t> related_mx28_ids;
    std::vector<uint8_t> related_xl320_ids;
    std::vector<int> related_mx28_gp;
    std::vector<int> related_xl320_gp;
    std::vector<int> related_mx28_ms;
    std::vector<int> related_xl320_ms;

    for (size_t i = 0; i < related_ids.size(); i++) {
        int related_id = related_ids.at(i);
        if (related_id <= 16 || related_id == 21 || related_id == 31) {
            related_mx28_ids.push_back(related_id);
            related_mx28_gp.push_back(goal_positions[related_id]);
            related_mx28_ms.push_back(moving_speeds[related_id]);
        } else {
            related_xl320_ids.push_back(related_id);
            related_xl320_gp.push_back(goal_positions[related_id]);
            related_xl320_ms.push_back(moving_speeds[related_id]);
        }
    }

    bool result = false;
    const char* log;

    /* Lakukan Sync Write untuk servo terkait 
    Ingat index handler untuk masing-masing register tadi */

    // Index Handler = 0 untuk Profile Velocity MX-28
    result = dxl_wb.syncWrite(0, related_mx28_ids.data(), related_mx28_ids.size(), related_mx28_ms.data(), 1, &log);
    if (!result) {
    RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Profile Velocity MX-28 gagal: %s", log);
    } else {
    // printf("[INFO] Sync Write untuk Profile Velocity MX-28 berhasil untuk %lu servo\n", (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])));
    }

    // Index Handler = 1 untuk Goal Position MX-28
    result = dxl_wb.syncWrite(1, related_mx28_ids.data(), related_mx28_ids.size(), related_mx28_gp.data(), 1, &log);
    if (!result) {
    RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Goal Position MX-28 gagal: %s", log);
    } else {
    // printf("[INFO] Sync Write untuk Goal Position MX-28 berhasil untuk %lu servo\n", (int) (sizeof(mx28_ids)/sizeof(mx28_ids[0])));
    }

    // Index Handler = 3 untuk Moving Speed XL-320
    result = dxl_wb.syncWrite(3, related_xl320_ids.data(), related_xl320_ids.size(), related_xl320_ms.data(), 1, &log);
    if (!result) {
    RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Moving Speed XL-320 gagal: %s", log);
    } else {
    // printf("[INFO] Sync Write untuk Moving Speed XL-320 berhasil untuk %lu servo\n", (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])));
    }

    // Index Handler = 2 untuk Goal Position XL-320
    result = dxl_wb.syncWrite(2, related_xl320_ids.data(), related_xl320_ids.size(), related_xl320_gp.data(), 1, &log);
    if (!result) {
    RCLCPP_ERROR(rclcpp::get_logger("ServoManager"), "Sync Write untuk Goal Position XL-320 gagal: %s", log);
    } else {
    // printf("[INFO] Sync Write untuk Goal Position XL-320 berhasil untuk %lu servo\n", (int) (sizeof(xl320_ids)/sizeof(xl320_ids[0])));
    }

}

void ServoManager::calculateSpeed(int id, int goalPosition, float speed) {
    int degree = 0;
    // float secondToReach = 0.0f;
    if (id <= 17 || id == 21 || id == 31) {
        degree = alfan::ConvertUtils::valueToDegreeMX28(goalPosition);
        degree_difference[id] = abs(latest_position_in_degree[id] - degree);
        moving_speeds[id] = degree_difference[id] / (speed * CONVERSION_FACTOR_MX28);

        // secondToReach = (degree_difference[id]/360.0) / ((moving_speeds[id] * 0.229) / 60.0); // for debug purpose only

        if (moving_speeds[id] > 230) {
            moving_speeds[id] = 150;
        }
        else if (moving_speeds[id] <= 0) {
            moving_speeds[id] = 1;
        }
    }
    else {
        degree = alfan::ConvertUtils::valueToDegreeXL320(goalPosition);
        degree_difference[id] = abs(latest_position_in_degree[id] - degree);
        moving_speeds[id] = degree_difference[id] / (speed * CONVERSION_FACTOR_XL320);

        // secondToReach = (degree_difference[id]/360.0) / ((moving_speeds[id] * 0.111) / 60.0); // for debug purpose only
        
        if (moving_speeds[id] > 1023) {
            moving_speeds[id] = 1023;
        }
        else if (moving_speeds[id] <= 0) {
            moving_speeds[id] = 1;
        }
    }

    // printf("ID: %d\tGoal Position: %d\t(%d derajat)\tPrev Degree: %d\t Diff: %d\tSpeed: %d in %f second\n", id, goalPosition, degree, ServoManager::latest_position_in_degree[id], ServoManager::degree_difference[id], ServoManager::moving_speeds[id], secondToReach);

    updateLatestPositionInDegree(id, goalPosition);
}

void ServoManager::updateLatestPositionInDegree(int id, int goalPosition) {
    if (id <= 17 || id == 21 || id == 31) {
        latest_position_in_degree[id] = alfan::ConvertUtils::valueToDegreeMX28(goalPosition);
    }
    else {
        latest_position_in_degree[id] = alfan::ConvertUtils::valueToDegreeXL320(goalPosition);
    }
}

void ServoManager::toDefaultPose(int millisec) {
    // TO DO
    std::vector<uint8_t> related_ids = getRelatedServoIdsByGroup(Group::ALL);
    for (size_t i = 0; i < related_ids.size(); i++) {
        int related_id = related_ids.at(i);
        if (related_id <= 17 || related_id == 21 || related_id == 31) {
            goal_positions[related_id] = alfan::ConvertUtils::degreeToValueMX28(Default[related_id]);
        } else {
            goal_positions[related_id] = alfan::ConvertUtils::degreeToValueXL320(Default[related_id]);
        }
        calculateSpeed(related_id, goal_positions[related_id], millisec/100);
    }

    sendMovementCommands();
    std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
    
    setCurrentPositionAsLatestPosition();

    for (int i = 1; i <= 6; i++) {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "GP %d : %d, MS: %d", i, goal_positions[i], moving_speeds[i]);
    }
    for (int i = 11; i <= 17; i++) {
        RCLCPP_INFO(rclcpp::get_logger("ServoManager"), "GP %d : %d, MS: %d", i, goal_positions[i], moving_speeds[i]);
    }
}
