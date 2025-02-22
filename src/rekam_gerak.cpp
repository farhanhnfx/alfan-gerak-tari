#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <program_rekam_gerak/lib_alfan.h>


const char usb_port[] = "/dev/ttyUSB0";
const uint32_t baud_rate = 1000000;


bool isRecording = true;
DynamixelWorkbench dxl_wb;

const char *debuglog;
bool result = false;

void readServoMX28(int id) {
    int defaultInByte = ConvertUtils::valueToDegreeMX28(Default[id]);
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

    RekamGerakHelper::debugRekam(id, presentPosition);
}

void readServoXL320(int id) {
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

int main() {
    result = dxl_wb.init(usb_port, baud_rate, &debuglog);
    if (result == false) {
      printf("%s\n", debuglog);
      printf("Failed to init\n");
      // return 0;
    } else {
      printf("Succeed to init(%d)\n", baud_rate);
    }

    TerminalHelper::saveOriginalTerminal();

    while (isRecording) {
      int key = TerminalHelper::getKeyPress();
      if (key != -1) {
        cout << "Key pressed: " << char(key) << endl;
        if (key == 'q') {
          TerminalHelper::resetTerminal();
          isRecording = false;
          break;
        }
        else if (key == 'r') {
          cout << "Rekam gerak..." << endl;
          string filename = FileManager::generateFilename();

          // Tangan Kanan
          readServoMX28(21);
          readServoXL320(22);
          readServoXL320(23);
          readServoXL320(24);
          readServoXL320(25);
          readServoXL320(26);

          // Tangan Kiri
          readServoMX28(31);
          readServoXL320(32);
          readServoXL320(33);
          readServoXL320(34);
          readServoXL320(35);
          readServoXL320(36);
          FileManager::createFile(filename, fileDataTxt, fileDataJson);
        }

        // cuma simulasi kalo error pas baca servo
        // else if (key == 'e') {
        //   errorBaca = true;
        //   cout << "Rekam gerak..." << endl;
        //   string filename = FileManager::generateFilename();
        //   readServoMX28(21);
        //   readServoXL320(22);
        //   FileManager::createFile(filename, fileDataTxt, fileDataJson);
        // }
      }
    }

    return 0;
}
