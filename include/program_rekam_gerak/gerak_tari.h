#ifndef GERAK_TARI_H
#define GERAK_TARI_H


#include <program_rekam_gerak/tangan_controller.h>
#include <unistd.h>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include <iomanip>
#include <sstream>

/* CUSTOM DATA STRUCTURE FOR GERAK TARI */
struct MotionFrame {
    int tangan;
    // kepala, kaki, dll menyusul
    unordered_map<string, int> kepala;
};


struct GerakTari {
    string name;
    bool is_walking;
    float speeds;
    int times;
    std::vector<MotionFrame> motion_frames;
};

#define PATH_TO_CONFIG BASE_PATH "gerak_tari.yaml"


class GerakTariHandler {
    public:
    // GerakTariHandler() {
    //     preload_config(list_of_gerak_tari);
    // }

    void preload_config();
    void play();
    
    private:
    std::vector<GerakTari> list_of_gerak_tari;
    TanganController tanganController;
    int music_state = 1;
    
    std::string read_file(const char *filename);
    void execute_move(GerakTari gerak_tari);
};


#endif

