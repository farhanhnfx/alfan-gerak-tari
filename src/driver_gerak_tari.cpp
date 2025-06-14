#include <alfan_gerak_tari/core/gerak_tari.h>
#include <alfan_gerak_tari/utils/terminal_helper.h>
#include <alfan_gerak_tari/core/servo_manager.h>

string config_name;
string fullpath_config;

void cobaGerakTangan() {
    printf("'q' - Quit\n'd' - Default\n'Space' - Play Gerak Tari\n");
    printf("'i' - Ping MX28 dan XL320\n");
    printf("'o' - Torque On\n");
    printf("'p' - Torque Off\n");

    auto gerakTariHandler = make_shared<GerakTariHandler>();

    if (TerminalHelper::key_pressed == 'd') {
        // Set Posisi Tangan Default
        cout << "Default Pose" << endl;
        // gerakTariHandler->setCustomDefaultPose();
        ServoManager::toDefaultPose(2500);
    }
    else if (TerminalHelper::key_pressed == ' ') {
        cout << "Playing Tari" << endl;
        std::vector<GerakTari> gerak_tari_var_kiri;
        // Preload config gerak tari harapannya akan dijalankan ketika program dimulai
        gerakTariHandler->preload_config(fullpath_config.c_str());
        gerakTariHandler->play();

        cout << "Tari Selesai\n\n";
    }
    else if (TerminalHelper::key_pressed == 'i') {
        gerakTariHandler->pingServos();
    }
    else if (TerminalHelper::key_pressed == 'o') {
        gerakTariHandler->setTorqueOn();
    }
    else if (TerminalHelper::key_pressed == 'p') {
        gerakTariHandler->setTorqueOff();
    }
}


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    bool on_deployment = false;
    if (argc > 1) {
        config_name = argv[1];
    }
    else {
        config_name = "";
    }
    on_deployment = (config_name == "deploy");
    fullpath_config = BASE_PATH + string("config_tari/") + config_name + ".yaml";

    ServoManager::init();
    ServoManager::pingMX28();
    ServoManager::pingXL320();
    ServoManager::setTorqueOn();

    ServoManager::toDefaultPose(2500);
    
    if (on_deployment) {
        auto gerakTariHandler = make_shared<GerakTariHandler>();
        gerakTariHandler->kudaKuda();

        // // keep spinning until the callback sets the flag:
        // while (rclcpp::ok() && !gerakTariHandler->isVariasiBtnHasPressed()) {
        //     rclcpp::spin_some(gerakTariHandler);
        //     std::this_thread::sleep_for(50ms);
        // }

        // gerakTariHandler->play();

        while (true) {
            rclcpp::spin_some(gerakTariHandler);
            if (rclcpp::ok() && gerakTariHandler->isVariasiBtnHasPressed()) {
                if (gerakTariHandler-> variasi_gerak_pressed == "VAR_SIJI"){  
                    // Ambil & load konfigurasi-nya
                    string config_path = BASE_PATH + "VAR_KIRI.yaml";
                    gerakTariHandler->preload_config(config_path.c_str());
                    gerakTariHandler->play();

                }
                else if (gerakTariHandler-> variasi_gerak_pressed == "VAR_LORO"){  
                    // Ambil & load konfigurasi-nya
                    string config_path = BASE_PATH + "VAR_KANAN.yaml";
                    gerakTariHandler->preload_config(config_path.c_str());
                    gerakTariHandler->play();

                }
                else if (gerakTariHandler-> variasi_gerak_pressed == "VAR_DEFAULT"){  
                    ServoManager::toDefaultPose(2500);
                    gerakTariHandler->kudaKuda();
                }
                else if (gerakTariHandler-> variasi_gerak_pressed == "VAR_SUGENG"){  
                    // Ambil & load konfigurasi-nya
                    string config_path = BASE_PATH + "VAR_SUGENG.yaml";
                    gerakTariHandler->preload_config(config_path.c_str());
                    gerakTariHandler->play();
                }
                 
                gerakTariHandler->variasi_gerak_pressed = ""; // Reset setelah play
                RCLCPP_INFO(rclcpp::get_logger("GerakTariHandler"), "-- SELESAI --");
            }
            std::this_thread::sleep_for(50ms);
        }
    }
    else {
        TerminalHelper::buildTerminalLoop(cobaGerakTangan);
    }

    rclcpp::shutdown();

    return 0;
}
