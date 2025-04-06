#ifndef ALFAN_YAML_UTILS_H
#define ALFAN_YAML_UTILS_H

#include <ryml.hpp>
#include <ryml_std.hpp>

// Struktur untuk menyimpan informasi komentar
struct Comment {
    int line_num;
    int primary_comment_pos;
    int additional_comment_pos;
};

/*
 * Class untuk hal yang berkaitan dengan file YAML
*/
class YamlUtils {
    public:
    /*
    * @brief Fungsi untuk membuat file YAML baru
    * @param name Nama file yang akan dibuat
    * @return string nama file yang telah dibuat
    * @note Jika file sudah ada, tidak akan membuat file baru
    * @note Nama file akan diganti spasi dengan underscore
    * @note File akan disimpan di direktori "config_tari"
    */
    static std::string createFile(std::string name);
    
    /*
     * @brief Fungsi untuk membaca file YAML
     * @param path_to_file Path ke file YAML yang akan dibaca
     * @return string isi dari file YAML
     * @note Semua komentar akan diabaikan
    */
    static std::string readFile(const std::string &path_to_file);

    /*
     * @brief Fungsi untuk membaca file YAML dan mempertahankan komentar
     * @param path_to_file Path ke file YAML yang akan dibaca
     * @return string isi dari file YAML
     * @note Semua komentar akan dipertahankan
    */
    static std::string readFilePreserveComments(const std::string &path_to_file);

    /*
     * @brief Fungsi untuk membuat atau menambah 'Sequence' gerakan baru pada file YAML
     * @param sequences_of_gerak_tari NodeRef yang merepresentasikan list 'sequences' gerakan
     * @note Fungsi ini digunakan ketika Rekam Gerak
     * @note 'Sequence' merupakan urutan gerakan yang telah berhasil direkam
    */
    static void createNewSequence(ryml::NodeRef &sequences_of_gerak_tari);

    /*
     * @brief Fungsi untuk menambah 'Motion Frame' baru pada file YAML
     * @param new_motion_frame_recorded dari 'Motion Frame' yang baru saja direkam
     * @note Fungsi ini digunakan ketika Rekam Gerak
     * @note 'Motion Frame' merupakan gerakan yang telah berhasil direkam (disimpan menjadi file GRK<counter>.txt)
    */
    static void addNewRecordedMotionFrame(int new_motion_frame_recorded);

    /*
     * @brief Fungsi untuk memecah 'Sequence' gerakan saat ini
     * @note Fungsi ini digunakan ketika Rekam Gerak
     * @note 'Sequence' merupakan urutan gerakan yang telah berhasil direkam
    */
    static void breakCurrentSequence();

    /*
     * @brief Fungsi untuk menghapus 'Motion Frame' terakhir yang direkam
     * @note Fungsi ini digunakan ketika Rekam Gerak
     * @note 'Motion Frame' merupakan gerakan yang telah berhasil direkam (disimpan menjadi file GRK<counter>.txt)
     * @note Jika 'Motion Frame' terakhir dihapus, maka 'Sequence' terakhir juga akan dihapus
    */
    static void deleteLastRecordedMotionFrame();

    /*
     * @brief Fungsi untuk memperbarui file YAML
     * @param path Path ke file YAML yang akan diperbarui
     * @param yaml_tree Tree YAML yang akan ditulis ke file
     * @note Fungsi ini digunakan untuk menulis kembali isi file YAML ketika Rekam Gerak
     * @note Fungsi ini akan menambahkan baris kosong sebelum setiap 'Sequence' baru
     * @note Fungsi ini juga akan menambahkan komentar yang sebelumnya telah disimpan
    */
    static void updateFile(std::string path, ryml::Tree yaml_tree);
};

#endif