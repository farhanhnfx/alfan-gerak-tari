#include <alfan_gerak_tari/record/rekam_gerak.h>
#include <alfan_gerak_tari/utils/convert_utils.h>
#include <alfan_gerak_tari/io/file_manager.h>
#include <alfan_gerak_tari/globals.h>
#include <cstdlib>
#include <iostream>

using namespace std;

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
    int defaultInByte = alfan::ConvertUtils::degreeToValueMX28(Default[id]);
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

    debug(id, presentPosition);
}
void RekamGerakHelper::readXL320(int id) {
    int defaultIntByte = alfan::ConvertUtils::degreeToValueXL320(Default[id]);
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

    RekamGerakHelper::debug(id, presentPosition);
}

void RekamGerakHelper::debug(uint8_t id, int32_t presentPosition) {
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
        cout << alfan::ConvertUtils::valueToDegreeMX28(selisihPresentDefault) << " (DEG) ";
    } else {
        cout << alfan::ConvertUtils::valueToDegreeXL320(selisihPresentDefault) << " (DEG) ";
    }
    cout << changeDirection << "\t\t";
    cout << "Present Position: " << presentPosition << " (DEC)" << endl;
}
