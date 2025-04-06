#ifndef ALFAN_SERVO_MANAGER_H
#define ALDAN_SERVO_MANAGER_H

#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

/*
 * @brief Class untuk mengelola servo Dynamixel
*/
class ServoManager {
    /*
     * @brief Enum untuk mengelompokkan servo berdasarkan bagian tubuh
     *        yang dikendalikan
     * 
     * @note Kelompok ini digunakan untuk mempermudah pengelompokan servo
     *       berdasarkan bagian tubuh yang dikendalikan
    */
    public: enum class Group {
        ALL,
        TANGAN,
        KEPALA,
        KAKI
    };

    private:
    static const uint32_t baud_rate = 1000000;                  // Baudrate servo
    static constexpr const char* usb_port = "/dev/ttyUSB0";     // Port USB yang digunakan
    /*
     * @brief Fungsi untuk mendapatkan ID servo berdasarkan grupnya
     * @param group Grup servo antara ALL, TANGAN, KAKI, atau KEPALA
     * @return vector<uint8_t> related_ids ID servo terkait
    */
    static std::vector<uint8_t> getRelatedServoIdsByGroup(Group group);
    
    public:
    static DynamixelWorkbench dxl_wb;                           // Instance dari DynamixelWorkbench
    static bool is_init;                                        // Status inisialisasi
    static int32_t goal_positions[45];                          // Goal Position untuk servo
    static int32_t moving_speeds[45];                           // Moving Speed untuk servo
    static int32_t latest_position_in_degree[45];               // Posisi terakhir servo dalam derajat (Membantu perhitungan Moving Speed)
    static uint32_t degree_difference[45];                      // Selisih derajat antara posisi terakhir dan goal position (Membantu perhitungan Moving Speed)
    static uint8_t mx28_ids[14];                                // ID servo MX-28 (Kaki dan Tangan)
    static uint8_t xl320_ids[13];                               // ID servo XL-320 (Kepala dan Tangan)


    /*
     * @brief Inisialisasi servo Dynamixel
     * 
     * @return bool true jika inisialisasi berhasil, false jika gagal
    */
    static bool init();

    /*
     * @brief Melakukan ping ke salah satu servo
    */
    static void ping(uint8_t id);

    /*
     * @brief Melakukan ping ke beberapa servo berdasarkan grupnya (Semua, Tangan, Kaki, atau Kepala)
     * @param group Grup servo antara ALL, TANGAN, KAKI, atau KEPALA
    */
    static void ping(Group group);

    /*
     * @brief Melakukan ping ke semua servo MX-28
    */
    static void pingMX28();

    /*
     * @brief Melakukan ping ke semua servo XL-320
    */
    static void pingXL320();

    /*
     * @brief Mengaktifkan torsi semua servo
    */
    static void setTorqueOn();

    /*
     * @brief Mengaktifkan torsi servo tertentu
     * 
     * @param id ID servo yang akan diaktifkan torsinya
    */
    static void setTorqueOn(uint8_t id);

    /*
     * @brief Mengaktifkan torsi untuk beberapa servo berdasarkan grupnya (Semua, Tangan, Kaki, atau Kepala)
     * @param group Grup servo antara ALL, TANGAN, KAKI, atau KEPALA
    */
    static void setTorqueOn(Group group);

    /*
     * @brief Menonaktifkan torsi semua servo
    */
    static void setTorqueOff();

    /*
     * @brief Menonaktifkan torsi servo tertentu
     * 
     * @param id ID servo yang akan dinonaktifkan torsinya
    */
    static void setTorqueOff(uint8_t id);

    /*
     * @brief Menonaktifkan torsi untuk beberapa servo berdasarkan grupnya (Semua, Tangan, Kaki, atau Kepala)
     * @param group Grup servo antara ALL, TANGAN, KAKI, atau KEPALA
    */
    static void setTorqueOff(Group group);

    /*
     * @brief Mendapatkan posisi terakhir servo dan menyimpannya
     *        ke dalam variabel latest_position_in_degree
    */
    static void setCurrentPositionAsLatestPosition();

    /*
     * @brief Melakukan setup SyncWrite Handler untuk semua servo
    */
    static void setupSyncWriteHandler();

    /*
     * @brief Mengirim perintah untuk menggerakkan semua servo
    */
    static void sendMovementCommands();

    /*
     * @brief Mengirim perintah untuk menggerakan servo berdasarkan grupnya (Semua, Tangan, Kaki, atau Kepala)
     * @param group Grup servo antara ALL, TANGAN, KAKI, atau KEPALA
    */
    static void sendMovementCommandsByGroup(Group group);

    /*
     * @brief Menghitung kecepatan servo (dalam value) berdasarkan jarak Goal Position dengan posisi terakhir
     * @param id ID servo
     * @param goalPosition Posisi goal servo
     * @param speed Kecepatan servo (misal 5 maka diharapkan servo dapat mencapai goal position dalam 500 milidetik)
    */
    static void calculateSpeed(int id, int goalPosition, float speed);
   
    /*
     * @brief Mengupdate posisi terakhir servo dalam derajat
     * @param id ID servo
     * @param goalPosition Posisi goal servo
    */
    static void updateLatestPositionInDegree(int id, int goalPosition);

    /*
     * @brief Mengirim perintah untuk menggerakkan semua servo ke posisi default
     * @param millisec Waktu dalam milidetik untuk mencapai posisi default
     * @note Posisi default servo diambil dari array Default
    */
    static void toDefaultPose(int millisec);
};

#endif