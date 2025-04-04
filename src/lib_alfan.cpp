#include <program_rekam_gerak/lib_alfan.h>


unordered_map<uint8_t, int32_t> Default = {
    {1, 0},
    {21, KANAN21}, {22, KANAN22}, {23, KANAN23}, {24, KANAN24}, {25, KANAN25}, {26, KANAN26},
    {31, KIRI31}, {32, KIRI32}, {33, KIRI33}, {34, KIRI34}, {35, KIRI35}, {36, KIRI36}
};



/* Kebutuhan Rekam Gerak */
string fileDataTxt = ""; // Contain the data to be written to the file`
int counter_rekam_gerak = 0;
int32_t selisihPresentDefault = 0;

bool errorBaca = false;

string file_path_txt = FILE_PATH_TXT;

// Terminal state
termios originalTermios;



/*
 * @brief Fungsi untuk generate nama file (Rekam Gerak)
 * @return string nama file yang dihasilkan
 * @note Nama file dihasilkan dengan format "GRK<counter>.txt"
*/
string FileManager::generateFilename() {
    string filenameTxt;
    string filenameJson;

    do {
        // Generate the filename with the counter
        filenameTxt = file_path_txt + FILE_BASENAME + to_string(counter_rekam_gerak);
        counter_rekam_gerak++;
    } while (fs::exists(filenameTxt + FILE_EXTENSION_TXT));  // Check if the file already exists

    return FILE_BASENAME + to_string(counter_rekam_gerak-1);
}

void FileManager::createFile(string filename, string dataTxt) {
    // Error check
    if (errorBaca) {
        cerr << "Error detected. File not created.\n";
        errorBaca = false;
        fileDataTxt = ""; // Reset the file data
        counter_rekam_gerak--;
        return;
    }

    // ofstream fileTxt(string(FILE_PATH_TXT) + filename + FILE_EXTENSION_TXT);
    ofstream fileTxt(file_path_txt + filename + FILE_EXTENSION_TXT);

    // ofstream fileJson(string(FILE_PATH_JSON) + filename + FILE_EXTENSION_JSON);

    if (fileTxt.is_open()) {
        fileTxt << dataTxt;
        fileTxt.close();
        cout << "File created and written successfully.\n";

        fileDataTxt = ""; // Reset the file data
    } else {
        cerr << "Unable to create the TXT file.\n";
    }
}

void FileManager::setSubfolder(string subfolder) {
    file_path_txt = string(FILE_PATH_TXT) + "/" + subfolder + "/";
    if (fs::exists(file_path_txt)) {
        printf("Melanjutkan rekaman Gerak Tari (%s)\n", subfolder.c_str());
    }
    else {
        printf("Menyiapkan folder Gerak Tari baru (%s)\n", subfolder.c_str());
        fs::create_directory(file_path_txt);
    }
}

void FileManager::setNewFullPathTxt(string full_path) {
    file_path_txt = full_path;
    if (full_path.back() != '/') {
        file_path_txt.push_back('/');
    }
}

void FileManager::writeFileData(uint8_t id, int32_t selisihPresentDefault) {
    // buat TXT file-nya
    fileDataTxt += "A";
    fileDataTxt += to_string(id);
    fileDataTxt += ";";
    fileDataTxt += to_string(selisihPresentDefault);
    fileDataTxt += "B\n";
}

map<uint8_t, int32_t> FileManager::parseFileTxt(int counterGerak) {
    map<uint8_t, int32_t> parsedData;
    // string filePath = string(FILE_PATH_TXT) + FILE_BASENAME + to_string(counterGerak) + FILE_EXTENSION_TXT;
    string filePath = file_path_txt + FILE_BASENAME + to_string(counterGerak) + FILE_EXTENSION_TXT;
    ifstream fileTxt(filePath);

    if (fileTxt.is_open()) {
        string line;
        while (getline(fileTxt, line)) {
            if (!line.empty()) {
                // Parse the line
                size_t idStart = line.find('A') + 1;  // Position after 'A'
                size_t idEnd = line.find(';');        // Position of ';'
                size_t valueEnd = line.find('B');     // Position of 'B'

                if (idStart != string::npos && idEnd != string::npos && valueEnd != string::npos) {
                    uint8_t id = stoi(line.substr(idStart, idEnd - idStart));           // Extract id
                    int32_t deltaPositionInByte = stoi(line.substr(idEnd + 1, valueEnd - idEnd - 1)); // Extract value
                    int32_t value;// = deltaPosition + Default[id];
                    if (id == 21 || id == 31) {
                        // value = RekamGerakHelper::byteToDegreeMX28(deltaPositionInByte) + Default[id];    // Convert to degree
                        value = deltaPositionInByte + ConvertUtils::degreeToValueMX28(Default[id]);   // In Byte
                    }
                    else {
                        // value = RekamGerakHelper::byteToDegreeXL320(deltaPositionInByte) + Default[id];  // Convert to degree
                        value = deltaPositionInByte + ConvertUtils::degreeToValueXL320(Default[id]);     // In Byte
                    }
                    parsedData[id] = value;                                         // Store in map

                    // goalPosition[id] = value;
                }
            }
        }
        fileTxt.close();
    } else {
        cerr << "Unable to open file: " << filePath << endl;
    }

    return parsedData;
}

void FileManager::deleteLatestRecord() {
    if (counter_rekam_gerak > 0) {
        counter_rekam_gerak--;
        string filePathTxt = file_path_txt + FILE_BASENAME + to_string(counter_rekam_gerak) + FILE_EXTENSION_TXT;
        if (fs::exists(filePathTxt)) {
            fs::remove(filePathTxt);
            cout << "File deleted successfully.\n";
        }
        else {
            cerr << "File not found.\n";
        }
    }
    else {
        cerr << "No record to delete.\n";
    }
}



void RekamGerakHelper::init() {
    bool result = dxl_wb.init(usb_port, baud_rate, &debuglog);
    if (result == false) {
      printf("%s\n", debuglog);
      printf("Failed to init\n");
      // return 0;
      exit(1);
    } else {
      printf("Succeed to init (%d)\n", baud_rate);
    }
}

void RekamGerakHelper::readMX28(int id) {
    int defaultInByte = ConvertUtils::degreeToValueMX28(Default[id]);
    // TO DO: Read the present position from the servo
    int32_t presentPosition = defaultInByte;
    uint16_t model_number = 0;
    if (dxl_wb.ping(id, &model_number, &debuglog) == false) {
      printf("%s\n", debuglog);
      printf("Failed to ping\n");
      errorBaca = true;
    }
    // bool result = dxl_wb_mx28.itemRead(id, "Present_Position", &presentPosition, &debuglog);
    bool result = dxl_wb.getPresentPositionData(id, &presentPosition, &debuglog);
    if (result == false) {
      printf("%s\n", debuglog);
      printf("Failed to get present position\n");
      errorBaca = true;
    }

    selisihPresentDefault = presentPosition - defaultInByte;


    FileManager::writeFileData(id, selisihPresentDefault);

    debugRekam(id, presentPosition);
}
void RekamGerakHelper::readXL320(int id) {
    int defaultIntByte = ConvertUtils::degreeToValueXL320(Default[id]);
    // TO DO: Read the present position from the servo
    int32_t presentPosition = defaultIntByte;
    uint16_t model_number = 0;
    if (dxl_wb.ping(id, &model_number, &debuglog) == false) {
      printf("%s\n", debuglog);
      printf("Failed to ping\n");
      errorBaca = true;
    }
    bool result = dxl_wb.getPresentPositionData(id, &presentPosition, &debuglog);
    if (result == false) {
      printf("%s\n", debuglog);
      printf("Failed to get present position\n");
      errorBaca = true;
    }

    selisihPresentDefault = presentPosition - defaultIntByte;

    FileManager::writeFileData(id, selisihPresentDefault);

    RekamGerakHelper::debugRekam(id, presentPosition);
}

void RekamGerakHelper::debugRekam(uint8_t id, int32_t presentPosition) {
    string changeDirection = "";
      
    switch(id){
    case 21:
        if (selisihPresentDefault > 0) changeDirection += "KE DEPAN";
        else changeDirection += "KE BELAKANG";
        break;
    case 22:
        if (selisihPresentDefault > 0) changeDirection += "NAIK";
        else changeDirection += "TURUN";
        break;
    case 23:
        if (selisihPresentDefault > 0) changeDirection += "KE KANAN";
        else changeDirection += "KE KIRI";
        break;
    case 24:
        if (selisihPresentDefault > 0)changeDirection += "NAIK";
        else changeDirection += "TURUN";
        break;
    case 25:
        if (selisihPresentDefault > 0) changeDirection += "KE KANAN";
        else changeDirection += "KE KIRI";
        break;
    case 26:
        if (selisihPresentDefault > 0) changeDirection += "NAIK";
        else changeDirection += "TURUN";
        break;
      
    case 31:
        if (selisihPresentDefault > 0) changeDirection += "KE BELAKANG";
        else changeDirection += "KE DEPAN";
        break;
    case 32:
        if (selisihPresentDefault > 0) changeDirection += "TURUN ";
        else changeDirection += "NAIK";
        break;
    case 33:
        if (selisihPresentDefault > 0) changeDirection += "KE KANAN";
        else changeDirection += "KE KIRI";
        break;
    case 34:
        if (selisihPresentDefault > 0) changeDirection += "TURUN";
        else changeDirection += "NAIK";
        break;
    case 35:
        if (selisihPresentDefault > 0) changeDirection += "KE KANAN";
        else changeDirection += "KE KIRI";
        break;
    case 36:
        if (selisihPresentDefault > 0) changeDirection += "TURUN";
        else changeDirection += "NAIK";
        break;
    }

    if (id == 21) {
        cout << "------------------------------------------------------------------------------" << endl;
        cout << "Create file GERAK: " + to_string(counter_rekam_gerak - 1) + "\n";
    }
      
    cout << "ID: " << to_string(id) << "\t";
    cout << "ΔSudut: " << selisihPresentDefault << " (DEC)\t";
    if (id == 21 || id == 31) {
        cout << ConvertUtils::valueToDegreeMX28(selisihPresentDefault) << " (DEG) ";
    } else {
        cout << ConvertUtils::valueToDegreeXL320(selisihPresentDefault) << " (DEG) ";
    }
    cout << changeDirection << "\t\t";
    cout << "Present Position: " << presentPosition << " (DEC)" << endl;
}


bool TerminalHelper::is_running = false;
int TerminalHelper::key_pressed = 0;
void TerminalHelper::setNonBlockingInput() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ICANON;  // Disable canonical mode
    term.c_lflag &= ~ECHO;   // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void TerminalHelper::saveOriginalTerminal() {
    tcgetattr(STDIN_FILENO, &originalTermios);
}

void TerminalHelper::resetTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

void TerminalHelper::buildTerminalLoop(void (*function)()) {
    saveOriginalTerminal();
    is_running = true;

    atexit(resetTerminal);
    
    ((void (*)())function)();
    printf("\n");

    while (is_running) {
        key_pressed = getKeyPress();
        if (key_pressed != -1) {
            cout << "Key pressed: " << char(key_pressed) << endl;
            if (key_pressed == 'q') {
                printf("Quit...\n");
                is_running = false;
                break;
            }

            ((void (*)())function)();
            printf("\n");
        }
    }

    resetTerminal();
    key_pressed = 0;
}
int TerminalHelper::getKeyPress() {
    setNonBlockingInput();
    int ch = -1;
    struct timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
        ch = getchar();
    }
    return ch;
}


ServoConnector::ServoConnector() {
    init();
}

void ServoConnector::init() {
    const char* log;
    if (is_init) return;

    if (dxl_wb.init(usb_port, baud_rate, &log)) {
        cout << "[INFO] Dynamixel Workbench Initialization Success\n";
        is_init = true;
    } else {
        cout << "[ERROR] Failed to init: " << log << endl;
    }
}

void ServoConnector::setupSyncWriteHandler() {
    const char* log;

    cout << "[INFO] Setup Sync Write Handler\n";

    if (dxl_wb.addSyncWriteHandler(R_PROFILE_VELOCITY_MX28, 4, &log) == false) {
        cout << "[ERROR] Failed to add Sync Write Handler for Profile Velocity MX-28: " << log << endl;
    }

    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_MX28, 4, &log) == false) {
        cout << "[ERROR] Failed to add Sync Write Handler for Goal Position MX-28: " << log << endl;
    }

    if (dxl_wb.addSyncWriteHandler(R_GOAL_POSITION_XL320, 2, &log) == false) {
        cout << "[ERROR] Failed to add Sync Write Handler for Goal Position XL-320: " << log << endl;
    }

    if (dxl_wb.addSyncWriteHandler(R_MOVING_SPEED_XL320, 2, &log) == false) {
        cout << "[ERROR] Failed to add Sync Write Handler for Moving Speed XL-320: " << log << endl;
    }
}

void ServoConnector::sendMovementCommands() {
                                // TO DO: Ditambah dari yg kaki juga
    int mx28_goal_positions[] = {goal_positions[1], goal_positions[2], goal_positions[3], goal_positions[4], goal_positions[5], goal_positions[6],
                                 goal_positions[11], goal_positions[12], goal_positions[13], goal_positions[14], goal_positions[15], goal_positions[16],
                                 goal_positions[21], 
                                 goal_positions[31]};

                                // TO DO: Ditambah dari servo kepala juga
    int xl320_goal_positions[] = {goal_positions[22], goal_positions[23], goal_positions[24], goal_positions[25], goal_positions[26],
                                  goal_positions[32], goal_positions[33], goal_positions[34], goal_positions[35], goal_positions[36],
                                  goal_positions[41], goal_positions[42], goal_positions[43]};
        
    int mx28_moving_speeds[] = {moving_speeds[1], moving_speeds[2], moving_speeds[3], moving_speeds[4], moving_speeds[5], moving_speeds[6],
                                moving_speeds[11], moving_speeds[12], moving_speeds[13], moving_speeds[14], moving_speeds[15], moving_speeds[16],
                                moving_speeds[21], 
                                moving_speeds[31]};

    int xl320_moving_speeds[] = {moving_speeds[22], moving_speeds[23], moving_speeds[24], moving_speeds[25], moving_speeds[26],
                                 moving_speeds[32], moving_speeds[33], moving_speeds[34], moving_speeds[35], moving_speeds[36],
                                 moving_speeds[41], moving_speeds[42], moving_speeds[43]};
    bool result = false;
    const char* log;

    // printf("[INFO] Mengirimkan Perintah Gerak\n------------------------------------------------\n");

    /* SEMENTARA DI-COMMENT TERLEBIH DAHULU UNTUK DUMMY
        KARENA BELUM ADA SERVO YANG BISA DICOBA UNTUK DIGERAKKAN */

    /* Lakukan Sync Write untuk servo terkait 
       Ingat index handler untuk masing-masing register tadi */
        
    // Index Handler = 0 untuk Profile Velocity MX-28
    result = dxl_wb.syncWrite(0, mx28_ids, sizeof(mx28_ids)/sizeof(mx28_ids[0]), mx28_moving_speeds, 1, &log);
    if (!result) {
        printf("[ERROR] Sync Write untuk Profile Velocity MX-28 gagal: %s\n", log);
    } else {
        printf("[INFO] Sync Write untuk Profile Velocity MX-28 berhasil untuk %lu servo\n", sizeof(mx28_ids)/sizeof(mx28_ids[0]));
    }

    // Index Handler = 1 untuk Goal Position MX-28
    result = dxl_wb.syncWrite(1, mx28_ids, sizeof(mx28_ids)/sizeof(mx28_ids[0]), mx28_goal_positions, 1, &log);
    if (!result) {
        printf("[ERROR] Sync Write untuk Goal Position MX-28 gagal: %s\n", log);
    } else {
        printf("[INFO] Sync Write untuk Goal Position MX-28 berhasil untuk %lu servo\n", sizeof(mx28_ids)/sizeof(mx28_ids[0]));
    }


    // Index Handler = 2 untuk Goal Position XL-320
    result = dxl_wb.syncWrite(2, xl320_ids, sizeof(xl320_ids)/sizeof(xl320_ids[0]), xl320_goal_positions, 1, &log);
    if (!result) {
        printf("[ERROR] Sync Write untuk Goal Position XL-320 gagal: %s\n", log);
    } else {
        printf("[INFO] Sync Write untuk Goal Position XL-320 berhasil untuk %lu servo\n", sizeof(xl320_ids)/sizeof(xl320_ids[0]));
    }

    // Index Handler = 3 untuk Moving Speed XL-320
    result = dxl_wb.syncWrite(3, xl320_ids, sizeof(xl320_ids)/sizeof(xl320_ids[0]), xl320_moving_speeds, 1, &log);
    if (!result) {
        printf("[ERROR] Sync Write untuk Moving Speed XL-320 gagal: %s\n", log);
    } else {
        printf("[INFO] Sync Write untuk Moving Speed XL-320 berhasil untuk %lu servo\n", sizeof(xl320_ids)/sizeof(xl320_ids[0]));
    }
}