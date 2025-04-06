#ifndef ALFAN_FILE_MANAGER_H
#define ALFAN_FILE_MANAGER_H

#include <string>
#include <map>

using namespace std;

/*
 * Class untuk hal yang berkaitan dengan file (write, read, delete)
*/
class FileManager {
    public:
    /*
     * @brief Fungsi untuk generate nama file (Rekam Gerak)
     * @return string nama file yang dihasilkan
     * @note Nama file dihasilkan dengan format "GRK<counter>"
    */
    static string generateFilename();

    /*
     * @brief Fungsi untuk membuat file GRK<counter>.txt setelah berhasil Rekam Gerak
     * @param filename Nama file yang akan dibuat (GRK<counter>)
     * @param dataTxt Data yang akan dimasukkan ke dalam file
    */
    static void createFile(string filename, string dataTxt);

    /*
     * @brief Fungsi untuk menuliskan data ke internal fileDataTxt
     * @param id ID servo
     * @param selisihPresentDefault selisih posisi saat direkam dengan posisi default-nya
     * @note Fungsi ini sebenarnya hanya dijalankan di internal FileManager saja (harusnya private)
    */
    static void writeFileData(uint8_t id, int32_t selisihPresentDefault);

    /*
     * @brief Fungsi ini digunakan untuk set subfolder ketika Rekam Gerak, atau membuat subfolder baru jika belum ada
     * @param subfolder Nama subfolder yang akan dibuat
    */
    static void setSubfolder(string subfolder);

    /*
     * @brief Fungsi ini digunakan untuk assign full path baru ke variabel file_path_txt
     * @param full_path Path baru
     * @note Digunakan ketika akan membaca file konfigurasi pada saat load Gerak Tari dan Custom Default
    */
    static void setNewFullPathTxt(string full_path);

    /*
     * @brief Fungsi ini digunakan untuk menghapus atau cancel hasil Rekam Gerak
     * @param counter GRK<counter> yang akan dihapus
     * @return boolean berhasil atau tidak?
    */
    static bool deleteRecord(int counter);

    /*
     * @brief Fungsi ini digunakan melakukan parse dari format A21;123B menjadi map 21: 123 dan seterusnya
     * @param counterGerak GRK<counter> yang akan diparsing
     * @return map dengan ID sebagai key-nya dan Delta Position sebagai value-nya
    */
    static map<uint8_t, int32_t> parseFile(int counterGerak);
};


#endif