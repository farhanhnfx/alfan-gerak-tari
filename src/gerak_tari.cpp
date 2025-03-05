#include <program_rekam_gerak/gerak_tari.h>


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

    Communication comms;

    unsigned long interval = 0;

    auto start_tari_times = std::chrono::steady_clock::now();
    
    for (int i = 0; i < gerak_tari_size; i++) {
        // TO DO
        // if panic dll
        if (!panic_mode) {
            comms.request_music();
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
                    printf("Time out.\n");
                    break;
                }
            }

            if (count_transmit == 0) {
                comms.transmit_data(music_state, gerak_tari_current_index);
                count_transmit++;
            }

            printf("Waiting for counter match...(count time: %i)\n", count_time);
            comms.receives_data();
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate delay
        }

        // // check mute sementara
        // if (list_of_gerak_tari.at(i).name == "contohGerakTari_4") {
        //     setCurrentMusicState(0);
        // }
        // else if (list_of_gerak_tari.at(i).name == "contohGerakTari_5") {
        //     setCurrentMusicState(1);
        // }

        gerak_tari_current_index = i;
        execute_move(list_of_gerak_tari.at(i));

    }

    auto end_tari_times = std::chrono::steady_clock::now();
    long elapsed_tari_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_tari_times - start_tari_times).count();
    printf("Total time: %ld ms\n", elapsed_tari_time);
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

            printf("\t\tlast mute index: %i\n", last_mute_index);
            
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
                printf("\t\tshift_next_Walk_counter: %i\n", shift_next_walk_counter);
                for (int i = 0; i < shift_next_walk_counter; i++) {
                    int shifted_idx = range_of_shifted_gerak + (i+1);
                    printf("\t\t\tshifted index: %i\n", shifted_idx);
                    
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
        printf("EXECUTING %s with speeds %0.2f for %0.2f seconds (%i moves)\n", gerak_tari.name.c_str(), gerak_tari.speeds, (gerak_tari.times*gerak_tari.motion_frames.size())/1000.00, gerak_tari.motion_frames.size());
    }
    else {
        printf("KONDISI MUSIC MUTE - %s tidak dijalankan\n", gerak_tari.name.c_str());
    }

    // printf("\t\tmute_on_walk_counter: %i, shift_next_walk_counter: %i\n", mute_on_walk_counter, shift_next_walk_counter);

    // Eksekusi gerakan di sini aja ding
    for (int i = 0; i < gerak_tari.motion_frames.size(); i++) {
        MotionFrame motion_frame = gerak_tari.motion_frames.at(i);

        tanganController.bacaGerak(motion_frame.tangan, gerak_tari.speeds);

        if (music_state == 1) {
            // printf("------------------EXECUTING: %s with speeds %f\n\n", gerak_tari.name.c_str(), gerak_tari.speeds);
            tanganController.transmit();
        }
        else if (music_state == 0) {
            // printf("MUSIC STATE == FALSE | MOVEMENT NOT EXECUTED\n");
        }
            
        // usleep(gerak_tari.times * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(gerak_tari.times));

        printf("\nNext Frame:\n");
    }

}