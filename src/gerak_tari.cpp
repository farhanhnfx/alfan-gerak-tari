#include <program_rekam_gerak/gerak_tari.h>

void GerakTariHandler::preload_config() {
    std::string yaml_content = read_file(PATH_TO_CONFIG);
    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_content));
        
    ryml::NodeRef root = tree["gerak_tari_sequences"];
    cout << root.num_children() << endl;
    if (root.num_children() > 0) {
        for (int i = 0; i < root.num_children(); i++) {
            GerakTari gt;
            auto tree_gerak_tari = root[i];

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

                        // cout << tree_motion_frames[j]["kepala"]["kepala41"].val() << endl;
                        cout << mf.kepala["kepala41"] << endl;
                        cout << tree_motion_frames[j]["kepala"]["kepala42"] << endl;
                        cout << tree_motion_frames[j]["kepala"]["kepala43"] << endl;
                    }

                    gt.motion_frames.push_back(mf);
                }
            }

            list_of_gerak_tari.push_back(gt);
        }
    }

    // for (int i = 0; i < gerak_tari_seq.size(); i++) {
    //     cout << gerak_tari_seq.at(i).name << endl;
    //     cout << gerak_tari_seq.at(i).is_walking << endl;
    //     cout << gerak_tari_seq.at(i).speeds << endl;
    //     cout << gerak_tari_seq.at(i).times << endl;
                
    //     cout << "\t Each Motion Frame:" << endl;
    //     for (int j = 0; j < gerak_tari_seq.at(i).motion_frames.size(); j++) {
    //         cout << " \t\t Tangan:" + to_string(gerak_tari_seq.at(i).motion_frames.at(j).tangan) << endl;
    //     }
    //     cout << "-----------------------" << endl;
    // }
}

void GerakTariHandler::play() {
    for (int i = 0; i < list_of_gerak_tari.size(); i++) {
        execute_move(list_of_gerak_tari.at(i));
    }
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
    for (int i = 0; i < gerak_tari.motion_frames.size(); i++) {
        MotionFrame motion_frame = gerak_tari.motion_frames.at(i);

        tanganController.bacaGerak(motion_frame.tangan, gerak_tari.speeds);

        if (music_state == 1) {
            printf("------------------EXECUTING: %s with speeds %f\n\n", gerak_tari.name.c_str(), gerak_tari.speeds);
            tanganController.transmit();
        }
        else if (music_state == 0) {
            printf("MUSIC STATE == FALSE | MOVEMENT NOT EXECUTED\n");
        }
            
        usleep(gerak_tari.times * 1000);
    }
}