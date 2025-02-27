#include <program_rekam_gerak/gerak_tari.h>

// int main() {
// std::vector<GerakTari> gerak_tari_var_kiri;
//     // Preload config gerak tari harapannya akan dijalankan ketika program dimulai

//     GerakTariHandler gerakTariHandler;
//     gerakTariHandler.preload_config();

//     gerakTariHandler.play();

//     return 0;
// }

bool test_run = true;


int main() {
    TerminalHelper::saveOriginalTerminal();

    GerakTariHandler gerakTariHandler;
    int key = 0;

    printf("'q' untuk Quit\n'd' untuk Default\n'Space untuk Play Gerak Tari\n");

    while (test_run) {
        key = TerminalHelper::getKeyPress();
        if (key != -1) {
            cout << "Key pressed: " << char(key) << endl;
            if (key == 'q') {
            TerminalHelper::resetTerminal();
            test_run = false;
            break;
        }
        else if (key == 'd') {
            // Set Posisi Tangan Default
            cout << "Default Pose" << endl;
            gerakTariHandler.tanganController.toDefaultPose();
        }
        else if (key == ' ') {
            cout << "Playing Tari" << endl;
            std::vector<GerakTari> gerak_tari_var_kiri;
            // Preload config gerak tari harapannya akan dijalankan ketika program dimulai

            gerakTariHandler.preload_config();

            gerakTariHandler.play();

            cout << "Tari Selesai\n\n";
        }

      }
    }

    return 0;
}
