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

    while (test_run) {
        key = TerminalHelper::getKeyPress();
        if (key != -1) {
            cout << "Key pressed: " << char(key) << endl;
            if (key == 'q') {
            TerminalHelper::resetTerminal();
            test_run = false;
            break;
        }
        // simulasi mute
        else if (key == 'm') {
            gerakTariHandler.setCurrentMusicState(0);
        }
        else if (key == 'r') {
            gerakTariHandler.setCurrentMusicState(1);
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
