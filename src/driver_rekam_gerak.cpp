#include <alfan_gerak_tari/record/rekam_gerak.h>
#include <alfan_gerak_tari/io/yaml_utils.h>
#include <alfan_gerak_tari/io/file_manager.h>
#include <alfan_gerak_tari/utils/terminal_helper.h>
#include <alfan_gerak_tari/globals.h>
#include <iostream>
#include <string>

using namespace std;

RekamGerakHelper rekamGerakHelper;

void terminalRekamGerak() {
    printf("'q' - Quit\n");
    printf("'r' - Rekam Gerak\n");
    printf("'b' - Break (buat sequence gerak baru)\n");
    printf("'c' - Cancel (hapus rekaman gerak/motion frame terakhir)\n");

    if (TerminalHelper::key_pressed == 'r') {
          cout << "Rekam gerak..." << endl;
          string filename = FileManager::generateFilename();

          // Tangan Kanan
          rekamGerakHelper.readMX28(21);
          rekamGerakHelper.readXL320(22);
          rekamGerakHelper.readXL320(23);
          rekamGerakHelper.readXL320(24);
          rekamGerakHelper.readXL320(25);
          rekamGerakHelper.readXL320(26);

          // Tangan Kiri
          rekamGerakHelper.readMX28(31);
          rekamGerakHelper.readXL320(32);
          rekamGerakHelper.readXL320(33);
          rekamGerakHelper.readXL320(34);
          rekamGerakHelper.readXL320(35);
          rekamGerakHelper.readXL320(36);

          FileManager::createFile(filename, fileDataTxt);
          YamlUtils::addNewRecordedMotionFrame(counter_rekam_gerak - 1);

          printf("\n'q' - Quit\n'd' - Default\n");
          printf("'r' - Rekam Gerak\n");
          printf("'b' - Break (buat sequence gerak baru)\n");
          printf("'c' - Cancel (hapus rekaman gerak/motion frame terakhir)\n");
    }
    else if (TerminalHelper::key_pressed == 'b') {
        // Break
        cout << "Membuat sequence gerakan baru..." << endl;
        YamlUtils::breakCurrentSequence();
    }
    else if (TerminalHelper::key_pressed == 'c') {
        // Cancel
        cout << "Membatalkan rekaman terakhir..." << endl;
        YamlUtils::deleteLastRecordedMotionFrame();
    }
}

int main() {
    rekamGerakHelper.init();

    cout << "Masukkan nama Gerak Tari: ";
    string subfolder;
    getline(cin, subfolder);
    printf("Subfolder Gerak Tari: %s\n", subfolder.c_str());

    FileManager::setSubfolder(subfolder);

    YamlUtils::createFile(subfolder);

    TerminalHelper::buildTerminalLoop(terminalRekamGerak);

    return 0;
}
