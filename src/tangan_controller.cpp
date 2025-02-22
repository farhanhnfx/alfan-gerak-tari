#include <program_rekam_gerak/tangan_controller.h>


TanganController::TanganController() {
    init();
    setupSyncWriteHandler();
}

void TanganController::init() {
    const char* log;
    bool result = false;
        
    result = dxl_wb.init(usb_port, baud_rate, &log);
    result = true; // Dummy buat testing bentar
    if (result == false) {
        printf("[ERROR] Gagal Inisialisasi Dynamixel: %s\n", log);
        return;
    } else {
        printf("[INFO] Inisialisasi Dynamixel Berhasil! (Baudrate: %d)\n", baud_rate);
    }

    latestPositionInDegree[21] = Default[21];
    latestPositionInDegree[22] = Default[22];
    latestPositionInDegree[23] = Default[23];
    latestPositionInDegree[24] = Default[24];
    latestPositionInDegree[25] = Default[25];
    latestPositionInDegree[26] = Default[26];

    latestPositionInDegree[31] = Default[31];
    latestPositionInDegree[32] = Default[32];
    latestPositionInDegree[33] = Default[33];
    latestPositionInDegree[34] = Default[34];
    latestPositionInDegree[35] = Default[35];
    latestPositionInDegree[36] = Default[36];
}

void TanganController::setupSyncWriteHandler() {
    const char* log;

    printf("[INFO] Setup Sync Write Handler\n");

    /* Index Handler akan bertambah ketika kita menambahkan Sync Write Handler
        kita nanti perlu tau index handler untuk masing-masing register */

    // Index Handler = 0 | MX-28 Profile Velocity | Address = 112 | Length = 4
    if (dxl_wb.addSyncWriteHandler(R_PROFILE_VELOCITY_MX28, 4, &log) == false) {
        printf("[ERROR] Gagal menambahkan Sync Write Handler untuk Profile Velocity MX-28: %s\n", log);
    }

    // Index Handler = 1 | MX-28 Goal Position | Address = 116 | Length = 4
    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_MX28, 4, &log) == false) {
        printf("[ERROR] Gagal menambahkan Sync Write Handler untuk Goal Position MX-28: %s\n", log);
    }

    // Index Handler = 2 | XL-320 Goal Position | Address = 30 | Length = 2
    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_XL320, 2, &log) == false) {
        printf("[ERROR] Gagal menambahkan Sync Write Handler untuk Goal Position XL-320: %s\n", log);
    }

    // Index Handler = 3 | XL-320 Moving Speed | Address = 32 | Length = 2
    if (dxl_wb.addSyncWriteHandler(R_MOVING_SPEED_XL320, 2, &log) == false) {
        printf("[ERROR] Gagal menambahkan Sync Write Handler untuk Moving Speed XL-320: %s\n", log);
    }
}

void TanganController::bacaGerak(int gerakCounter, float speed) {
    processMotionFrame(gerakCounter, speed);
}

void TanganController::transmit() {
    sendMovementCommands();
}

void TanganController::processMotionFrame(int gerakCounter, float speed) {
    map<uint8_t, int32_t> dataMotionFrame = FileManager::parseFileTxt(gerakCounter); // Kalau file dalam TXT
                                                                                    // Jangan lupa ganti ke parseFileJson jika dalam JSON

    for (const auto& [id, goalPositionValue] : dataMotionFrame) {
        goal_positions[id] = goalPositionValue;
            
        /* Maksimal Value Goal Position XL-320 = 1023
            Sedangkan Maksimum MX-28 = 4095 */
        if (goalPositionValue > 1023) {
            goal_positions[id] = 1023;
        } else if (goalPositionValue < 0) {
            goal_positions[id] = 0;
        }
            
        calculateSpeeds(id, goal_positions[id], speed);
    }
}

void TanganController::calculateSpeeds(int id, int goalPosition, float speed) {
    int degree = 0;
    float secondToReach = 0;
    if (id == 21 || id == 31) {
        degree = ConvertUtils::valueToDegreeMX28(goalPosition);
        /* TO DO: mencari perhitungan moving_speeds[id] */
        degreeDifference[id] = abs(latestPositionInDegree[id] - degree);
        // if (degreeDifference[id] > 5)
        moving_speeds[id] = degreeDifference[id] / (speed * CONVERSION_FACTOR_MX28);

        secondToReach = (degreeDifference[id]/360.0) / ((moving_speeds[id] * 0.229) / 60.0); // for debug purpose only
    }
    else {
        degree = ConvertUtils::valueToDegreeXL320(goalPosition);
        /* TO DO: mencari perhitungan moving_speeds[id] */
        degreeDifference[id] = abs(latestPositionInDegree[id] - degree);
        // if (degreeDifference[id] > 5)
        moving_speeds[id] = degreeDifference[id] / (speed * CONVERSION_FACTOR_XL320);

        secondToReach = (degreeDifference[id]/360.0) / ((moving_speeds[id] * 0.111) / 60.0); // for debug purpose only
    }

    if (moving_speeds[id] > 1023) {
        moving_speeds[id] = 1023;
    }
    printf("ID: %d\tGoal Position: %d\t(%d derajat)\tPrev Degree: %d\t Diff: %d\tSpeed: %d in %f second\n", id, goalPosition, degree, latestPositionInDegree[id], degreeDifference[id], moving_speeds[id], secondToReach);
    // printf("ID: %d\tGoal Position: %d\t(%d derajat)\tPrev Degree: %d\tDiff: %d\n", id, goalPosition, degree, latestPositionInDegree[id], degreeDifference[id]);

    updateLatestPositionInDegree(id, goalPosition);
}

void TanganController::updateLatestPositionInDegree(int id, int goalPosition) {
    if (id == 21 || id == 31) {
        latestPositionInDegree[id] = ConvertUtils::valueToDegreeMX28(goalPosition);
    }
    else {
        latestPositionInDegree[id] = ConvertUtils::valueToDegreeXL320(goalPosition);
    }
}

void TanganController::sendMovementCommands() {
    int mx28_goal_positions[] = {goal_positions[21], goal_positions[31]};
    int xl320_goal_positions[] = {goal_positions[22], goal_positions[23], goal_positions[24], goal_positions[25], goal_positions[26], 
                                  goal_positions[32], goal_positions[33], goal_positions[34], goal_positions[35], goal_positions[36]};
        
    int mx28_moving_speeds[] = {moving_speeds[21], moving_speeds[31]};
    int xl320_moving_speeds[] = {moving_speeds[22], moving_speeds[23], moving_speeds[24], moving_speeds[25], moving_speeds[26], 
                                 moving_speeds[32], moving_speeds[33], moving_speeds[34], moving_speeds[35], moving_speeds[36]};
    bool result = false;
    const char* log;

    printf("[INFO] Mengirimkan Perintah Gerak\n------------------------------------------------\n");

    /* SEMENTARA DI-COMMENT TERLEBIH DAHULU UNTUK DUMMY
        KARENA BELUM ADA SERVO YANG BISA DICOBA UNTUK DIGERAKKAN */

    // /* Lakukan Sync Write untuk servo terkait 
    //    Ingat index handler untuk masing-masing register tadi */
        
    // // Index Handler = 0 untuk Profile Velocity MX-28
    // result = dxl_wb.syncWrite(0, mx28_ids, sizeof(mx28_ids)/sizeof(mx28_ids[0]), mx28_moving_speeds, 1, &log);
    // if (!result) {
    //     printf("[ERROR] Sync Write untuk Profile Velocity MX-28 gagal: %s\n", log);
    // } else {
    //     printf("[INFO] Sync Write untuk Profile Velocity MX-28 berhasil untuk %lu servo\n", sizeof(mx28_ids)/sizeof(mx28_ids[0]));
    // }

    // // Index Handler = 1 untuk Goal Position MX-28
    // result = dxl_wb.syncWrite(1, mx28_ids, sizeof(mx28_ids)/sizeof(mx28_ids[0]), mx28_goal_positions, 1, &log);
    // if (!result) {
    //     printf("[ERROR] Sync Write untuk Goal Position MX-28 gagal: %s\n", log);
    // } else {
    //     printf("[INFO] Sync Write untuk Goal Position MX-28 berhasil untuk %lu servo\n", sizeof(mx28_ids)/sizeof(mx28_ids[0]));
    // }

    // // Index Handler = 2 untuk Goal Position XL-320
    // result = dxl_wb.syncWrite(2, xl320_ids, sizeof(xl320_ids)/sizeof(xl320_ids[0]), xl320_goal_positions, 1, &log);
    // if (!result) {
    //     printf("[ERROR] Sync Write untuk Goal Position XL-320 gagal: %s\n", log);
    // } else {
    //     printf("[INFO] Sync Write untuk Goal Position XL-320 berhasil untuk %lu servo\n", sizeof(xl320_ids)/sizeof(xl320_ids[0]));
    // }

    // // Index Handler = 3 untuk Moving Speed XL-320
    // result = dxl_wb.syncWrite(3, xl320_ids, sizeof(xl320_ids)/sizeof(xl320_ids[0]), xl320_moving_speeds, 1, &log);
    // if (!result) {
    //     printf("[ERROR] Sync Write untuk Moving Speed XL-320 gagal: %s\n", log);
    // } else {
    //     printf("[INFO] Sync Write untuk Moving Speed XL-320 berhasil untuk %lu servo\n", sizeof(xl320_ids)/sizeof(xl320_ids[0]));
    // }
}