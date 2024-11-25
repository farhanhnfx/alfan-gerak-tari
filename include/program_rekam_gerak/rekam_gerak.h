#ifndef REKAM_GERAK_H
#define REKAM_GERAK_H

#include <rclcpp/rclcpp.hpp>

#include <cstdio>
#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace rclcpp;
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

#define BYTE_TO_DEGREE_MX28 360.0/4095.0
#define BYTE_TO_DEGREE_XL320 300.0/1023.0

unordered_map<uint8_t, int32_t> Default = {
    {1, 0},
    {21, KANAN21}, {22, KANAN22}, {23, KANAN23}, {24, KANAN24}, {25, KANAN25}, {26, KANAN26},
    {31, KIRI31}, {32, KIRI32}, {33, KIRI33}, {34, KIRI34}, {35, KIRI35}, {36, KIRI36}
};


string fileDataTxt = ""; // Contain the data to be written to the file`
string fileDataJson = ""; // Contain the data to be written to the file
int counter = 0;
int32_t selisihPresentDefault = 0;

bool errorBaca = false;


// Terminal state
termios originalTermios;


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
        // RCLCPP_ERROR(this->get_logger(), "Error detected. File not created.");
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
        // RCLCPP_INFO(this->get_logger(), "File TXT created and written successfully.");

        fileDataTxt = ""; // Reset the file data
    } else {
        cerr << "Unable to create the TXT file.\n";
        // RCLCPP_ERROR(this->get_logger(), "Unable to create the TXT file.");
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
        // RCLCPP_INFO(this->get_logger(), "File  JSON created and written successfully.");

        fileDataJson = ""; // Reset the file data
    } else {
        cerr << "Unable to create the JSON file.\n";
        // RCLCPP_ERROR(this->get_logger(), "Unable to create the JSON file.");
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

};


class RekamGerakHelper {
  public:
  static int degreeToByteMX28(float degree) {
      float degreeToByte = 360.0 / 4095.0;
      return degree / degreeToByte;
  }

  static int degreeToByteXL320(float degree) {
      float degreeToByte = 300.0 / 1023.0;
      return degree / degreeToByte;
  }

  static int byteToDegreeMX28(int byte) {
      float degreeToByte = 360.0 / 4095.0;
      return abs(floor(byte * degreeToByte));
  }

  static int byteToDegreeXL320(int byte) {
      float degreeToByte = 300.0 / 1023.0;
      return abs(floor(byte * degreeToByte));
  }

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
        //   RCLCPP_INFO(this->get_logger(), "Create file GERAK: %d", counter);
      }
      
      cout << "ID: " << id << "\t";
    //   RCLCPP_INFO(this->get_logger(), "ID: %d", id);
      cout << "ΔSudut: " << selisihPresentDefault << " (DEC)\t";
    //   RCLCPP_INFO(this->get_logger(), "ΔSudut: %d (DEC)", selisihPresentDefault);
      if (id == 21 || id == 31) {
          cout << byteToDegreeMX28(selisihPresentDefault) << " (DEG) ";
        //   RCLCPP_INFO(this->get_logger(), "%d (DEG)", byteToDegreeMX28(selisihPresentDefault));
      } else {
          cout << byteToDegreeXL320(selisihPresentDefault) << " (DEG) ";
        //   RCLCPP_INFO(this->get_logger(), "%d (DEG)", byteToDegreeXL320(selisihPresentDefault));
      }
      cout << changeDirection << "\t\t";
      cout << "Present Position: " << presentPosition << " (DEC)" << endl;
    //   RCLCPP_INFO(this->get_logger(), "Present Position: %d (DEC)", presentPosition);
  }
};

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