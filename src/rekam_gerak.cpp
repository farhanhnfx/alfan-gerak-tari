#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <program_rekam_gerak/lib_alfan.h>
#include <algorithm>


RekamGerakHelper rekamGerakHelper;
string file_name;

void terminalRekamGerak() {
    printf("'q' - Quit\n");
    printf("'r' - Rekam Gerak\n");

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

          FileManager::createFile(filename, fileDataTxt, fileDataJson);

          printf("\n'q' - Quit\n'd' - Default\n");
          printf("'r' - Rekam Gerak\n");
    }
    else if (TerminalHelper::key_pressed == 'c') {
        cout << "Membatalkan rekaman terakhir..." << endl;
        FileManager::deleteLatestRecord();
    }
}

void createYAMLFile(string name) {
    file_name = name;
    replace(file_name.begin(), file_name.end(), ' ', '_');
    fs::create_directory(BASE_PATH + string("config_tari/"));
    ofstream fileYAML(BASE_PATH + string("config_tari/") + file_name+ ".yaml");

    fileYAML << "gerak_tari_sequences:\n";
    fileYAML << "\tsubfolder: " << name << "\n";
    fileYAML << "\tsequences:\n";

    fileYAML.close();
}



int main() {
    rekamGerakHelper.init();

    cout << "Masukkan nama Gerak Tari: ";
    string subfolder;
    getline(cin, subfolder);
    printf("Subfolder Gerak Tari: %s\n", subfolder.c_str());

    FileManager::setSubfolder(subfolder);

    createYAMLFile(subfolder);

    TerminalHelper::buildTerminalLoop(terminalRekamGerak);

    return 0;
}
