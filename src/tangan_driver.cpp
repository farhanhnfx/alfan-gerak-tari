#include <program_rekam_gerak/gerak_tari.h>

// GerakTariHandler gerakTariHandler;
// auto gerakTariHandler = make_shared<GerakTariHandler>();
string config_name;
string fullpath_config;

// void cobaGerakTangan() {
//     printf("'q' - Quit\n'd' - Default\n'Space' - Play Gerak Tari\n");
//     printf("'i' - Ping MX28 dan XL320\n");
//     printf("'o' - Torque On\n");
//     printf("'p' - Torque Off\n");

//     if (TerminalHelper::key_pressed == 'd') {
//         // Set Posisi Tangan Default
//         cout << "Default Pose" << endl;
//         gerakTariHandler->setDefaultPose();
//     }
//     else if (TerminalHelper::key_pressed == ' ') {
//         cout << "Playing Tari" << endl;
//         std::vector<GerakTari> gerak_tari_var_kiri;
//         // Preload config gerak tari harapannya akan dijalankan ketika program dimulai
//         gerakTariHandler->preload_config(fullpath_config.c_str());
//         gerakTariHandler->play();

//         cout << "Tari Selesai\n\n";
//     }
//     else if (TerminalHelper::key_pressed == 'i') {
//         gerakTariHandler->pingServos();
//     }
//     else if (TerminalHelper::key_pressed == 'o') {
//         gerakTariHandler->setTorqueOn();
//     }
//     else if (TerminalHelper::key_pressed == 'p') {
//         gerakTariHandler->setTorqueOff();
//     }
// }


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto gerakTariHandler = make_shared<GerakTariHandler>();

    bool on_deployment = false;
    if (argc > 1) {
        config_name = argv[1];
        on_deployment = (argc > 2 && std::string(argv[2]) == "deploy") ? true : false;
    }
    else {
        config_name = "";
    }
    fullpath_config = BASE_PATH + string("config_tari/") + config_name + ".yaml";

    // TerminalHelper::buildTerminalLoop(cobaGerakTangan);
    
    if (on_deployment) {
        gerakTariHandler->setDefaultPose();

        rclcpp::spin_some(gerakTariHandler);

        while (!gerakTariHandler->isVariasiBtnHasPressed()) {
            // Tunggu sampai button dipencet
        }

        gerakTariHandler->play();
    }
    else {
        // TerminalHelper::buildTerminalLoop(cobaGerakTangan);
    }

    rclcpp::shutdown();

    return 0;
}
