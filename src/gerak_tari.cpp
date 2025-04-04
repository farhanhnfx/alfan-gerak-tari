#include <program_rekam_gerak/gerak_tari.h>


GerakTariHandler::GerakTariHandler() : Node(THIS_ROBOT_NAME + "_gerak_tari_handler") {
    subs_variasi_gerak = this->create_subscription<std_msgs::msg::String>(
        THIS_ROBOT_NAME + "/variasi_gerak", 10,
        [this](std_msgs::msg::String::SharedPtr msg) {
            variasi_gerak_pressed = msg->data;
            RCLCPP_INFO(this->get_logger(), "Received: %s", msg->data.c_str());

            /*
                variasi_gerak_pressed = VAR_KIRI atau VAR_KANAN
            */
           // Ambil & load konfigurasi-nya
            string config_path = BASE_PATH + variasi_gerak_pressed + ".yaml";
            this->preload_config(config_path.c_str());

            // Hapus subscription-nya setelah set variasi
            subs_variasi_gerak.reset();
        });
    
}

GerakTariHandler::~GerakTariHandler() {

}

bool GerakTariHandler::isVariasiBtnHasPressed() {
    return variasi_gerak_pressed == "VAR_KIRI"
                        ||
           variasi_gerak_pressed == "VAR_KANAN";
}

void GerakTariHandler::pingServos() {
    tanganController.pingMX28();
    tanganController.pingXL320();
}

void GerakTariHandler::setTorqueOn() {
    tanganController.setTorqueOn();
}

void GerakTariHandler::setTorqueOff() {
    tanganController.setTorqueOff();
}

void GerakTariHandler::setDefaultPose() {
    /*
        TO DO:
        Menentukan format konfigurasi .yaml
        Untuk kemudian nantinya dibaca di fungsi ini
        Sehingga bisa melakukan kustomisasi pose Default

        Fungsi ini akan dijalankan ketika robot dihidupkan

        Format konfigurasi yang dibayangkan:
        - tangan: (int) < baca rekaman tangan (mungkin di subfolder "DefaultPose" atau sejenisnya)
        - kepala: (x, y, z) < sudut yang diperlukan untuk kepala ID 41, 42, 43
        - kaki:
          - kanan: (x, y, z)
          - kiri: (x, y, z)

        Yang diperlukan:
        - baca posisi tangan yang sudah direkam untuk dijadikan pose default
        - set kepala dan kaki sesuai yang ditentukan
        - sleep selama 2-3 detik untuk mencapai posisi Default
    */

    RCLCPP_INFO(this->get_logger(), "POse default robot");

    tanganController.toDefaultPose();
}

void GerakTariHandler::preload_config(const char* config_path) {
    cout << config_path << endl;
    if (fs::exists(config_path)) {
        cout << "File exists" << endl;
    }
    else {
        cout << "File doesn't exist, using main config" << endl;
        config_path = PATH_TO_CONFIG;
    }
    std::string yaml_content = read_file(config_path);
    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_content));

    // clearing
    if (list_of_gerak_tari.size() > 0) {
        list_of_gerak_tari.clear();
    }
        
    ryml::NodeRef root = tree["gerak_tari_sequences"];
    cout << root.num_children() << endl;
    string subfolder;
    root["subfolder"] >> subfolder;
    FileManager::setNewFullPathTxt(FILE_PATH_TXT + subfolder);

    if (root["sequences"].num_children() > 0) {
        for (int i = 0; i < root["sequences"].num_children(); i++) {
            GerakTari gt;
            auto tree_gerak_tari = root["sequences"][i];

            tree_gerak_tari["name"] >> gt.name;
            tree_gerak_tari["is_walking"] >> gt.is_walking;
            tree_gerak_tari["speeds"] >> gt.speeds;
            tree_gerak_tari["times"] >> gt.times;

            auto tree_motion_frames = tree_gerak_tari["motion_frames"];
            if (tree_motion_frames.num_children() > 0) {
                for (int j = 0; j < tree_motion_frames.num_children(); j++) {
                    MotionFrame mf;
                    tree_motion_frames[j]["tangan"] >> mf.tangan;
                    if (tree_motion_frames[j].has_child("kepala")) {
                        /* TO DO: Menyimpan action kepala ke dalam Motion Frame 
                            tree_motifon_frames[j]["kepala"] >> mf.kepala */
                        tree_motion_frames[j]["kepala"]["kepala41"] >> mf.kepala["kepala41"];

                        // // cout << tree_motion_frames[j]["kepala"]["kepala41"].val() << endl;
                        // cout << mf.kepala["kepala41"] << endl;
                        // cout << tree_motion_frames[j]["kepala"]["kepala42"] << endl;
                        // cout << tree_motion_frames[j]["kepala"]["kepala43"] << endl;
                    }

                    gt.motion_frames.push_back(mf);
                }
            }

            list_of_gerak_tari.push_back(gt);
        }
    }

    for (int i = 0; i < list_of_gerak_tari.size(); i++) {
        cout << list_of_gerak_tari.at(i).name << endl;
    }
    cout << file_path_txt << endl;

    gerak_tari_size = list_of_gerak_tari.size();
}

void GerakTariHandler::play() {
    /*
        TO DO:
        Sebelum benar-benar play tarian, cek komunikasi terlebih dahulu
    */
   
    // rclcpp::init(0, nullptr);
    auto comms = std::make_shared<Communication>();
    comms->starting_condition();

    unsigned long interval = 0;

    auto start_tari_times = std::chrono::steady_clock::now();

    int tari_index = 0;
    if (rescue_mode) {
        tari_index = gerak_tari_current_index;
    }
    
    for (tari_index; tari_index< gerak_tari_size; tari_index++) {
        // TO DO
        // if panic dll
        comms->update_panic_state();
        comms->publish_internal_index();
        comms->update_other_index();
        if (!panic_mode) {
            comms->request_music();
        }
        else if (panic_mode) {
            // Panic ON
        }
        // kondisi sinkronisasi indeks/counter gerak tari ke berapa
        // unsigned long previous_millis = millis(); // Waktu saat ini
        auto previous_millis = std::chrono::steady_clock::now();

        while (gerak_tari_current_index > data_gerak_tari_others_index) {
            static int count_transmit = 0;
            // unsigned long current_millis = millis(); // Waktu saat ini
            auto current_millis = std::chrono::steady_clock::now();

            // Interval 250 ketika robot tiba-tiba mati, dan
            // 50 untuk kondisi setelah menari sendiri
            if (count_time <= 1) {
                interval = 250;
            }
            else {
                interval = 50;
            }

            // Ketika robot tidak dalam keadaan rscure, maka terdapat
            // durasi maksimal untuk melakukan sinkronisasi
            // sesuai interval
            if (!rescue_mode) {
                long elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_millis - previous_millis).count();
                if (elapsed_time >= interval) {
                    count_time++;
                    RCLCPP_INFO(comms->get_logger(), "Time out.");
                    break;
                }
            }

            if (count_transmit == 0) {
                count_transmit++;
            }

            RCLCPP_INFO(comms->get_logger(), "Waiting for counter match...(count time: %i)", count_time);
            comms->update_other_index();
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate delay
        }

        execute_move(list_of_gerak_tari.at(tari_index));
        gerak_tari_current_index++;
        rescue_mode = false;
        music_state = 1;
    }

    auto end_tari_times = std::chrono::steady_clock::now();
    long elapsed_tari_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_tari_times - start_tari_times).count();
    RCLCPP_INFO(comms->get_logger(), "Total time: %ld ms", elapsed_tari_time);

    // Selesai - Shutdown ROS2
    RCLCPP_INFO(comms->get_logger(), "Gerak tari selesai - SHUTDOWN");
    comms->request_shutdown();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // rclcpp::shutdown();
}

std::string GerakTariHandler::read_file(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "[ERROR] Gagal membuka " << filename << std::endl;
        exit(1);
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

void GerakTariHandler::execute_move(GerakTari gerak_tari) {
    // Kondisi musik hidup
    if (music_state == 1) {
        // Kondisi ketika terdapat mute yang dilakukan sebelumnya
        if (mute_on_walk_counter > 0) {
            /*
            Sebelum eksekusi replace jalan yang ada di kondisi mute, maka kita akan
            melakukan deteksi terlebih dahulu apakah terdapat walk yang akan di shifting
            agar jalannya tidak berkurang, tetapi di geser
            */
           
           // [int count_shift di program lama]
            int range_of_shifted_gerak = last_mute_index + mute_on_walk_counter;

            RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "last mute index: %i", last_mute_index);
            
            // Melakukan deteksi apakah ada walk setelah kondisi mute selesai
            for (int i = last_mute_index + 1; i <= range_of_shifted_gerak; i++ ) {
                GerakTari next_gerak_tari = list_of_gerak_tari.at(i);
                while (next_gerak_tari.is_walking) {
                    // Simpan ke vector shifted_moves
                    shifted_next_walk_moves.push_back(next_gerak_tari);
                    // Menghitung berapa banyak gerakan selanjutnya yang perlu dipindah
                    shift_next_walk_counter++;
                    next_gerak_tari = list_of_gerak_tari.at(++i);
                }
            }

            // Melakukan assign walk di muted_walks ke gerak tari yang dituju
            for (int i = 0; i < mute_on_walk_counter; i++) {
                int replaced_idx = last_mute_index + (i+1);

                list_of_gerak_tari.at(replaced_idx) = muted_walks.at(i);
                if (gerak_tari_current_index == replaced_idx) {
                    gerak_tari = list_of_gerak_tari.at(replaced_idx);
                }
            }

            // Melakukan assign walk di shifted_walk_moves ke gerak tari yang dituju
            // jika ada walk yang di-shifting atau perlu digeser
            if (shift_next_walk_counter > 0) {
                RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "shift_next_Walk_counter: %i", shift_next_walk_counter);
                for (int i = 0; i < shift_next_walk_counter; i++) {
                    int shifted_idx = range_of_shifted_gerak + (i+1);
                    RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "shifted index: %i", shifted_idx);
                    
                    list_of_gerak_tari.at(shifted_idx) = shifted_next_walk_moves.at(i);
                }
            }

            // Kembali ke kondisi awal
            shift_next_walk_counter = 0;
            mute_on_walk_counter = 0;

        }

        if (rescue_mode == 1) { // Kondisi rescue
            music_state = true;
        }

        // Eksekusi gerakan

    }
    else if (music_state == 0) {
        // Tetap eksekusi gerakan
        
        // Menghitung berapa banyak walk yang terkena mute
        if (gerak_tari.is_walking) {
            muted_walks.push_back(gerak_tari);

            // menghitung muted_walks
            mute_on_walk_counter++;
        }

        // Mendeteksi counter yang menjadi last mute
        last_mute_index = gerak_tari_current_index;
    }

    if (music_state == 1) {
        RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "EXECUTING %s with speeds %0.2f for %0.2f seconds (%i moves)", gerak_tari.name.c_str(), gerak_tari.speeds, (gerak_tari.times*gerak_tari.motion_frames.size())/1000.00, gerak_tari.motion_frames.size());
    }
    else {
        RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "KONDISI MUSIC MUTE - %s tidak dijalankan", gerak_tari.name.c_str());
    }

    // Eksekusi gerakan di sini aja ding
    for (int i = 0; i < gerak_tari.motion_frames.size(); i++) {
        MotionFrame motion_frame = gerak_tari.motion_frames.at(i);

        tanganController.bacaGerak(motion_frame.tangan, gerak_tari.speeds);

        if (music_state == 1) {
            tanganController.transmit();
        }
        else if (music_state == 0) {
            // RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "MUSIC STATE == FALSE | MOVEMENT NOT EXECUTED");
        }
            
        // usleep(gerak_tari.times * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(gerak_tari.times));

        // RCLCPP_INFO(rclcpp::get_logger("gerak_tari"), "Next Frame:");
    }

}