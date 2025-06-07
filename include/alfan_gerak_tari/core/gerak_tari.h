#ifndef ALFAN_GERAK_TARI_H
#define ALFAN_GERAK_TARI_H

#include <alfan_gerak_tari/core/tangan.h>
#include <alfan_gerak_tari/core/kepala.h>
#include <alfan_gerak_tari/core/kaki.h>
#include <alfan_gerak_tari/globals.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std;

/*
 * Struct ini merupakan Custom Structure
 * yang mendefinisikan setiap frame pergerakan
 * berisikan tangan, kepala, dan kaki (menyusul)
 * @note tangan merupakan GRK<counter>.txt yang akan dibaca
 * @note kepala berisikan KepalaStruct (posisi ID 41, 42, 43, serta speed-nya)
 * @note kaki menyusul?
*/
struct MotionFrame {
    int tangan_frame;
    // kepala, kaki, dll menyusul
    KepalaStruct kepala_frame;
    KakiStruct kaki_frame;
};

/*
 * Struct ini merupakan Custom Structure
 * yang mendefinisikan sebuah gerak tari (diselaraskan dengan format .yaml gerak tari)
 * Sebuah GerakTari merupakan kumpulan dari satu atau lebih MotionFrame
 * @note name merupakan nama gerakan
 * @note is_walking menandakan apakah gerak ini termasuk gerak jalan atau tidak
 * @note speeds mengacu pada kecepatan servo yang diperlukan (misal 5 maka diharapkan servo dapat mencapai Goal Position dalam 500 milidetik) di setiap frame-nya
 * @note times mengacu pada berapa lama program akan berhenti sejenak menunggu servo sampai tujuannya (dalam milidetik) - (default-nya akan mengambil dari speeds*100)
 * @note motion_frames yaitu kumpulan satu atau lebih MotionFrame yang dibutuhkan untuk membentuk sebuah gerakan
*/
struct GerakTari {
    string name;                            // nama gerakan
    bool is_walking;                        // menandakan apakah gerak ini termasuk gerak jalan atau tidak
    float speeds;                           // kecepatan servo yang diperlukan (misal 5 maka diharapkan servo dapat mencapai Goal Position dalam 500 milidetik) di setiap frame-nya
    int times;                              // berapa lama program akan berhenti sejenak menunggu servo sampai tujuannya (dalam milidetik) - (default-nya akan mengambil dari speeds*100)
    std::vector<MotionFrame> motion_frames; // kumpulan satu atau lebih MotionFrame yang dibutuhkan untuk membentuk sebuah gerakan
};

/*
 * Class ini digunakan untuk meng-handle hal yang diperlukan dalam menari
*/
class GerakTariHandler: public rclcpp::Node {
    private:
    // Subscriber untuk mendapatkan informasi tombol variasi yang dipencet dari program komunikasi (alfan_communication)
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subs_variasi_gerak;

    // Menyimpan list atau sequence dari gerak tari (sehingga menjadi satu tarian utuh)
    std::vector<GerakTari> list_of_gerak_tari;
    // Digunakan untuk menyimpan total gerak tari yang ada
    int gerak_tari_size = 0;
    // Menghitung berapa banyak mute ketika walk [int mute di program lama]
    int mute_on_walk_counter = 0;
    // List berisi walk yang akan dijalankan karena sebelumnya di-mute [array data_replace di program lama]
    std::vector<GerakTari> muted_walks;
    // List berisi gerak tari (jalan) selanjutnya yang dipindah urutannya agar tidak hilang karena kondisi setelah mute [array data_shift di program lama]
    std::vector<GerakTari> shifted_next_walk_moves;
    // Menghitung berapa gerak tari (jalan) selanjutnya yang perlu dipindah urutannya [int shift di program lama]
    int shift_next_walk_counter = 0;
    // Menyimpan index terakhir gerak tari yang di-mute [int last_mute di program lama]
    int last_mute_index = 0;
    
    Tangan tangan;  // Instance dari Tangan
    Kepala kepala;  // Instance dari Kepala
    Kaki kaki;      // Instance dari kaki
    
    /*
     * @brief Fungsi ini digunakan untuk menjalankan sebuah Gerak Tari (jika kondisi music hidup)
     * @param gerak_tari GerakTari yang akan dijalankan
    */
    void execute_move(GerakTari gerak_tari);

    public:
    GerakTariHandler();
    ~GerakTariHandler();
    // Menyimpan tombol variasi mana yang dipencet (VAR_KIRI atau VAR_KANAN)
    string variasi_gerak_pressed;
    /*
     * @brief Fungsi ini digunakan untuk melakukan load konfigurasi sebuah tari yang akan ditampilkan
     * @param config_path Path menuju file konfigurasi tariannya
     * @note Ketika dalam keadaan deploy, maka konfigurasi yang digunakan yaitu antara VAR_KIRI.yaml atau VAR_KANAN.yaml
     * @note Path ketika menguji gerakan (misal setelah rekam) bergantung pada arguments yang dimasukkan ketika menjalankan program driver_gerak_tari (secara standalone)
     *       misalnya ros2 run alfan_gerak_tari gerak_tari fortune_cookie maka akan me-load konfigurasi fortune_cookie.yaml dari folder config_tari/fortune_cookie.yaml
    */
    void preload_config(const char* config_path = (BASE_PATH + "gerak_tari.yaml").c_str());

    /*
     * @brief Fungsi ini akan menjalankan semua gerak tari yang telah di-load sebelumnya
     * @note Di sini juga ada algoritma untuk melakukan sinkronisasi antar-robot serta mengubah urutan gerak jika diperlukan (misal ketika music mute)
    */
    void play();

    /*
     * @brief Fungsi ini akan me-load konfigurasi pose default atau kuda-kuda robot berdasarkan custom_default_pose.yaml
     * @param millisec Berapa lama waktu yang dibutuhkan untuk menuju posisi default, dalam milidetik
    */
    void setCustomDefaultPose(int millisec = 3000);

    /*
     * @brief Melakukan ping ke semua servo
    */
    void pingServos();

    /*
     * @brief Mengaktifkan torsi semua servo
    */
    void setTorqueOn();

    /*
     * @brief Menonaktifkan torsi semua servo
    */
    void setTorqueOff();

    /*
     * @brief Mengecek apakah tombol variasi sudah dipencet?
     * @return bool pressed
    */
    bool isVariasiBtnHasPressed();
};



#endif