#include <alfan_gerak_tari/globals.h>
#include <unordered_map>

unordered_map<uint8_t, int32_t> Default = {
    {1, 0},
    {21, KANAN21}, {22, KANAN22}, {23, KANAN23}, {24, KANAN24}, {25, KANAN25}, {26, KANAN26},
    {31, KIRI31}, {32, KIRI32}, {33, KIRI33}, {34, KIRI34}, {35, KIRI35}, {36, KIRI36}
};


string BASE_PATH = "/home/farhanhanif/alfan_ws/src/program_rekam_gerak/src/";   // Sesuaikan dengan path di komputer masing-masing
string FILE_PATH = BASE_PATH + "Result/txt/";                                   // Sesuaikan dengan path di komputer masing-masing [SD Card]

/* Kebutuhan Rekam Gerak */
string fileDataTxt = "";
int counter_rekam_gerak = 0;
int32_t selisihPresentDefault = 0;
bool errorBaca = false;
string file_path_txt = FILE_PATH;

/* 
 * Konfigurasi Nama Robot 
 * Untuk keperluan komunikasi antar-robot
 * Robot 1: JUNA
 * Robot 2: CHACA
*/
string THIS_ROBOT_NAME = "JUNA";
string OTHER_ROBOT_NAME = "CHACA";

/*
 * Variabel komunikasi dan sinkronisasi antar-robot
 * dan juga music box
*/
bool panic_mode = true;     // Status mode panic (1) atau tidak (0)
bool rescue_mode = false;    // Status mode rescue (1) atau tidak (0)
int music_state = 0;     // Status musik apakah play (1) atau mute (0)
int gerak_tari_current_index = 0; // Tracker indeks gerak tari ke-berapa
int data_gerak_tari_others_index = 0;    // Tracker data dari robot satunya
int count_time = 0; // variabel untuk membantu sinkronisasi gerakan
