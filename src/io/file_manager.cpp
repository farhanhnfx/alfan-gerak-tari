#include <alfan_gerak_tari/globals.h>
#include <alfan_gerak_tari/utils/convert_utils.h>
#include <alfan_gerak_tari/io/file_manager.h>

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;


string FileManager::generateFilename() {
    string filenameTxt;

    do {
        // Generate the filename with the counter
        filenameTxt = file_path_txt + FILE_BASENAME + to_string(counter_rekam_gerak);
        counter_rekam_gerak++;
    } while (fs::exists(filenameTxt + TXT));  // Check if the file already exists

    return FILE_BASENAME + to_string(counter_rekam_gerak-1);
}

void FileManager::createFile(string filename, string dataTxt) {
    // Error check
    if (errorBaca) {
        std::cerr << "Error detected. File not created.\n";
        errorBaca = false;
        fileDataTxt = ""; // Reset the file data
        counter_rekam_gerak--;
        return;
    }

    // ofstream fileTxt(string(FILE_PATH_TXT) + filename + TXT);
    ofstream fileTxt(file_path_txt + filename + TXT);

    if (fileTxt.is_open()) {
        fileTxt << dataTxt;
        fileTxt.close();
        cout << "File created and written successfully.\n";

        fileDataTxt = ""; // Reset the file data
    } else {
        std::cerr << "Unable to create the TXT file.\n";
    }
}

void FileManager::setSubfolder(string subfolder) {
    file_path_txt = string(FILE_PATH) + "/" + subfolder + "/";
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

map<uint8_t, int32_t> FileManager::parseFile(int counterGerak) {
    map<uint8_t, int32_t> parsedData;
    string filePath = file_path_txt + FILE_BASENAME + to_string(counterGerak) + TXT;
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
                        value = deltaPositionInByte + alfan::ConvertUtils::degreeToValueMX28(Default[id]);
                    }
                    else {
                        value = deltaPositionInByte + alfan::ConvertUtils::degreeToValueXL320(Default[id]);
                    }
                    parsedData[id] = value;                                         // Store in map

                    // goalPosition[id] = value;
                }
            }
        }
        fileTxt.close();
    } else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
    }

    return parsedData;
}

bool FileManager::deleteRecord(int counter) {
    if (counter != counter_rekam_gerak) {
        std::cerr << "Tidak Valid (Perbedaan counter)";
        return false;
    }

    if (counter_rekam_gerak > 0) {
        counter_rekam_gerak--;
        string filePathTxt = file_path_txt + FILE_BASENAME + to_string(counter) + TXT;
        if (fs::exists(filePathTxt)) {
            fs::remove(filePathTxt);
            cout << "File deleted successfully.\n";
        }
        else {
            std::cerr << "File not found.\n";
        }
    }
    else {
        std::cerr << "No record to delete.\n";
    }
    return true;
}


