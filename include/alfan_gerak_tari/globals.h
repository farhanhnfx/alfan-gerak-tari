#ifndef GLOBALS_H
#define GLOBALS_H

#include <unordered_map>
#include <cstdint>
#include <string>

using namespace std;

/*_
 * FILE SETTINGS
*/
#define FILE_BASENAME "GRK"
#define TXT ".txt"

/*
 * TANGAN KANAN
 * ID 21-26
 * (DALAM DERAJAT)
*/
#define KANAN21 135  //+ NAIK
#define KANAN22 105  //+keluar
#define KANAN23 180  //+keluar
#define KANAN24 65   //+angkat
#define KANAN25 90  //+keluar
#define KANAN26 150  //-turun

/*
 * TANGAN KIRI
 * ID 31-36
 * (DALAM DERAJAT)
*/
#define KIRI31 225  //- NAIK
#define KIRI32 105  //-keluar
#define KIRI33 105  // +dalam
#define KIRI34 250  //-naik
#define KIRI35 210  // +dalam
#define KIRI36 150  // -naek

/*
 * KEPALA
 * ID 41, 42, 43
 * (DALAM DERAJAT)
*/
#define KEPALA41 150
#define KEPALA42 180
#define KEPALA43 150

/*
 * PERUT
 * ID 17
 * (DALAM DERAJAT)
*/
#define PERUT17 180

/*
 * KAKI KANAN
 * ID 1-6
 * (DALAM DERAJAT)
*/
#define KANAN1 180
#define KANAN2 180
#define KANAN3 180
#define KANAN4 180
#define KANAN5 180
#define KANAN6 180

/*
 * KAKI KIRI
 * ID 11-16
 * (DALAM DERAJAT)
*/
#define KIRI11 180
#define KIRI12 180
#define KIRI13 180
#define KIRI14 180
#define KIRI15 180
#define KIRI16 180

#define KANAN 0
#define KIRI 1

/* 
 * ENUM FOR CONTROL TABLE ADDRESS
 *     SERVO MX-28 DAN XL-320 
 * DETAILNYA BISA DICEK DI DOKUMENTASI RESMI
*/
enum ControlTableAddress {
    R_PROFILE_VELOCITY_MX28 = 112,  // LENGTH 4
    R_GOAL_POSITION_MX28 = 116,     // LENGTH 4
    
    R_GOAL_POSITION_XL320 = 30,     // LENGTH 2
    R_MOVING_SPEED_XL320 = 32       // LENGTH 2
};


/* 
 * Conversion Factor untuk Menghitung Moving Speed XL-320
 * dan Profile Velocity MX-28
 * Penjelasan perhitungan dapat dilihat di:
 * https://docs.google.com/document/d/14YH6WwX3MLx6o_tqQHlnr0oFchm8Jjc4bFIMNI2iFy8
*/
   const float CONVERSION_FACTOR_MX28 = 0.1374;
   const float CONVERSION_FACTOR_XL320 = 0.0666;

/*
 * GLOBAL VARIABLES
*/
extern string BASE_PATH;    // < Path program utama
extern string FILE_PATH;    // < Path untuk file TXT rekaman
extern unordered_map<uint8_t, int32_t> Default;    // < Default value servo (Dalam derajat)

/* 
 * Konfigurasi Nama Robot 
 * Untuk keperluan komunikasi antar-robot
 * Robot 1: JUNA
 * Robot 2: CHACA
*/
extern string THIS_ROBOT_NAME;     // < Nama Robot
extern string OTHER_ROBOT_NAME;    // < Nama Robot Lain

/*
 * Kebutuhan Rekam Gerak
 * Untuk menyimpan data yang dibaca dari servo
 * dan disimpan ke dalam file TXT
*/
extern int counter_rekam_gerak;
extern string fileDataTxt;
extern int32_t selisihPresentDefault;
extern bool errorBaca;
extern string file_path_txt;

/*
 * Variabel komunikasi dan sinkronisasi antar-robot
 * dan juga music box
*/
extern bool panic_mode;                 // < Status mode panic (1) atau tidak (0)
extern bool rescue_mode;                // < Status mode rescue (1) atau tidak (0)
extern int music_state;                 // < Status musik apakah play (1) atau mute (0)
extern int gerak_tari_current_index;    // < Tracker indeks gerak tari ke-berapa
extern int data_gerak_tari_others_index;// < Tracker data dari robot satunya
extern int count_time;                  // < variabel untuk membantu sinkronisasi gerakan



#endif