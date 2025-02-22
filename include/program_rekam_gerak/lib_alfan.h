#ifndef LIB_ALFAN_H
#define LIB_ALFAN_H

// #include <rclcpp/rclcpp.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <cmath>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <program_rekam_gerak/nlohmann_json/json.hpp>

using namespace std;
// using namespace rclcpp;
namespace fs = std::filesystem;

//_FILE SETTINGS_//
#define BASE_PATH "/home/farhanhanif/alfan_ws/src/program_rekam_gerak/src/"
#define FILE_PATH_TXT BASE_PATH "Result/txt/"
#define FILE_PATH_JSON BASE_PATH "Result/json/"
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
#define KIRI32 105  //-keluar
#define KIRI33 105  // +dalam
#define KIRI34 250  //-naik
#define KIRI35 195  // +dalam
#define KIRI36 150  // -naek

#define KANAN 0
#define KIRI 1

// Maksimal sudut dibagi dengan maksimal value dari servo
const float CONST_MX28 = 360.0 / 4095.0;    // 0.088
const float CONST_XL320 = 300.0 / 1023.0;   // 0.293

// GLOBAL VARIABLES
extern unordered_map<uint8_t, int32_t> Default;
extern string fileDataTxt;
extern string fileDataJson;
extern int32_t selisihPresentDefault;
extern bool errorBaca;

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
    static string generateFilename();

    static void createFile(string filename, string dataTxt, string dataJson);
    static void writeFileData(uint8_t id, int32_t selisihPresentDefault);

    static map<uint8_t, int32_t> parseFileTxt(int counterGerak);

    static map<uint8_t, int32_t> parseFileJson(int counterGerak);
};

/* Helper untuk mempermudah debug rekam gerak */
class RekamGerakHelper {
    public:
    static void debugRekam(uint8_t id, int32_t presentPosition);
};


/* Digunakan untuk program Rekam Gerak yang
   memerlukan bantuan terminal */
class TerminalHelper {
    public:
    // Function to make terminal non-blocking
    static void setNonBlockingInput();

    static void saveOriginalTerminal();

    static void resetTerminal();

    // Function to check for a key press
    static int getKeyPress();
};


#endif