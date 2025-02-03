#ifndef LIB_ALFAN_H
#define LIB_ALFAN_H

// #include <rclcpp/rclcpp.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <cmath>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <program_rekam_gerak/nlohmann_json/json.hpp>

using namespace std;
// using namespace rclcpp;
namespace fs = std::filesystem;

//_FILE SETTINGS_//
#define FILE_PATH_TXT "/home/farhanhanif/alfan_ws/src/program_rekam_gerak/src/Result/txt/"
#define FILE_PATH_JSON "/home/farhanhanif/alfan_ws/src/program_rekam_gerak/src/Result/json/"
#define FILE_BASENAME "GRK"
#define FILE_EXTENSION_TXT ".txt"
#define FILE_EXTENSION_JSON ".json"

//_RIGHT HAND_//
//___DEGREE___//
#define KANAN21 135  //+ NAIK
#define KANAN22 105  //+keluar
#define KANAN23 180  //+keluar
#define KANAN24 65   //+angkat
#define KANAN25 105  //+keluar
#define KANAN26 150  //-turun

//_LEFT HAND_//
//___DEGREE___//
#define KIRI31 225  //- NAIK
#define KIRI32 195  //-keluar
#define KIRI33 105  // +dalam
#define KIRI34 250  //-naik
#define KIRI35 195  // +dalam
#define KIRI36 150  // -naek

#define KANAN 0
#define KIRI 1

// Maksimal sudut dibagi dengan maksimal value dari servo
const float CONST_MX28 = 360.0 / 4095.0;    // 0.088
const float CONST_XL320 = 300.0 / 1023.0;   // 0.293

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

/* Digunakan untuk konversi dari
   derajat ke value ataupun
   sebaliknya */
class ConvertUtils {
    public:
    static int degreeToValueMX28(float degree) {
        return degree / CONST_MX28;
    }

    static int degreeToValueXL320(float degree) {
        return degree / CONST_XL320;
    }

    static int valueToDegreeMX28(int value) {
        return abs(floor(value * CONST_MX28));
    }

    static int valueToDegreeXL320(int value) {
        return abs(floor(value * CONST_XL320));
    }
};

/* Digunakan untuk write/read file Program Rekam Gerak */
class FileManager {
    public:
    static string generateFilename() {
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

    static void createFile(string filename, string dataTxt, string dataJson) {
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

    static void writeFileData(uint8_t id, int32_t selisihPresentDefault) {
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


    static unordered_map<uint8_t, int32_t> parseFileTxt(int counterGerak) {
        unordered_map<uint8_t, int32_t> parsedData;
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


    static unordered_map<uint8_t, int32_t> parseFileJson(int counterGerak) {
        unordered_map<uint8_t, int32_t> parsedData;
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
};

/* Helper untuk mempermudah debug rekam gerak */
class RekamGerakHelper {
    public:
    static void debugRekam(uint8_t id, int32_t presentPosition) {
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
      
      cout << "ID: " << id << "\t";
      cout << "ΔSudut: " << selisihPresentDefault << " (DEC)\t";
      if (id == 21 || id == 31) {
          cout << ConvertUtils::valueToDegreeMX28(selisihPresentDefault) << " (DEG) ";
      } else {
          cout << ConvertUtils::valueToDegreeXL320(selisihPresentDefault) << " (DEG) ";
      }
      cout << changeDirection << "\t\t";
      cout << "Present Position: " << presentPosition << " (DEC)" << endl;
  }
};


/* Digunakan untuk program Rekam Gerak yang
   memerlukan bantuan terminal */
class TerminalHelper {
    public:
    // Function to make terminal non-blocking
    static void setNonBlockingInput() {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~ICANON;  // Disable canonical mode
        term.c_lflag &= ~ECHO;   // Disable echo
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }

    static void saveOriginalTerminal() {
        tcgetattr(STDIN_FILENO, &originalTermios);
    }

    static void resetTerminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
    }

    // Function to check for a key press
    static int getKeyPress() {
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
};


#endif