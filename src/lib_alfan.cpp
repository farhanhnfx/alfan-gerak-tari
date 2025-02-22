#include <program_rekam_gerak/lib_alfan.h>


unordered_map<uint8_t, int32_t> Default = {
    {1, 0},
    {21, KANAN21}, {22, KANAN22}, {23, KANAN23}, {24, KANAN24}, {25, KANAN25}, {26, KANAN26},
    {31, KIRI31}, {32, KIRI32}, {33, KIRI33}, {34, KIRI34}, {35, KIRI35}, {36, KIRI36}
};



/* Kebutuhan Rekam Gerak */
string fileDataTxt = ""; // Contain the data to be written to the file`
string fileDataJson = ""; // Contain the data to be written to the file
int counter = 0;
int32_t selisihPresentDefault = 0;

bool errorBaca = false;

// Terminal state
termios originalTermios;




string FileManager::generateFilename() {
    string filenameTxt;
    string filenameJson;

    do {
        // Generate the filename with the counter
        filenameTxt = string(FILE_PATH_TXT) + FILE_BASENAME + to_string(counter);
        filenameJson = string(FILE_PATH_JSON) + FILE_BASENAME + to_string(counter);
        counter++;
    } while (fs::exists(filenameTxt + FILE_EXTENSION_TXT));  // Check if the file already exists

    return FILE_BASENAME + to_string(counter-1);
}

void FileManager::createFile(string filename, string dataTxt, string dataJson) {
    // Error check
    if (errorBaca) {
        cerr << "Error detected. File not created.\n";
        errorBaca = false;
        fileDataTxt = ""; // Reset the file data
        fileDataJson = ""; // Reset the file data
        counter--;
        return;
    }

    ofstream fileTxt(string(FILE_PATH_TXT) + filename + FILE_EXTENSION_TXT);

    ofstream fileJson(string(FILE_PATH_JSON) + filename + FILE_EXTENSION_JSON);

    if (fileTxt.is_open()) {
        fileTxt << dataTxt;
        fileTxt.close();
        cout << "File created and written successfully.\n";

        fileDataTxt = ""; // Reset the file data
    } else {
        cerr << "Unable to create the TXT file.\n";
    }

    // Check if the file was created successfully
    if (fileJson.is_open()) {
        fileJson << "{\n";
        fileJson << "  \"data\": [\n";
        fileJson << dataJson;
        fileJson << "  ]\n";
        fileJson << "}\n";
        
        // Close the file after writing
        fileJson.close();
        cout << "File created and written successfully.\n";

        fileDataJson = ""; // Reset the file data
    } else {
        cerr << "Unable to create the JSON file.\n";
    }

}

void FileManager::writeFileData(uint8_t id, int32_t selisihPresentDefault) {
    // buat TXT file-nya
    fileDataTxt += "A";
    fileDataTxt += to_string(id);
    fileDataTxt += ";";
    fileDataTxt += to_string(selisihPresentDefault);
    fileDataTxt += "B\n";

    // buat JSON file-nya
    fileDataJson += "\t{\n";
    fileDataJson += "\t\t\"id\": " + to_string(id) + ",\n";
    fileDataJson += "\t\t\"deltaPosition\": " + to_string(selisihPresentDefault) + "\n";
    fileDataJson += "\t}";
    if (id != 36) {
        fileDataJson += ",\n";
    }
    else {
        fileDataJson += "\n";
    }
}

map<uint8_t, int32_t> FileManager::parseFileTxt(int counterGerak) {
    map<uint8_t, int32_t> parsedData;
    string filePath = string(FILE_PATH_TXT) + FILE_BASENAME + to_string(counterGerak) + FILE_EXTENSION_TXT;
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

map<uint8_t, int32_t> FileManager::parseFileJson(int counterGerak) {
    map<uint8_t, int32_t> parsedData;
    string filePath = string(FILE_PATH_JSON) + FILE_BASENAME + to_string(counterGerak) + FILE_EXTENSION_JSON;
    ifstream fileJson(filePath);

    if (fileJson.is_open()) {
        nlohmann::json jsonData;
        fileJson >> jsonData;
        fileJson.close();

        // for (auto const& [id, value] : jsonData.items()) {
        //     parsedData[stoi(id)] = value;
        // }
        for (const auto &entry : jsonData["data"]) {
            // cout << "ID: " << entry["id"] << ", Value: " << entry["deltaPosition"] << endl;
            uint8_t id = entry["id"];
            int32_t deltaPositionInByte = entry["deltaPosition"];
                
            if (id == 21 || id == 31) {
                parsedData[id] = deltaPositionInByte + ConvertUtils::degreeToValueMX28(Default[id]);   // In Byte
            }
            else {
                parsedData[id] = deltaPositionInByte + ConvertUtils::degreeToValueXL320(Default[id]);     // In Byte
            }
        }

    } else {
        cerr << "Unable to open file: " << filePath << endl;
    }

    return parsedData;
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
        cout << "Create file GERAK: " + to_string(counter - 1) + "\n";
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