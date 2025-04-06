#ifndef ALFAN_REKAM_GERAK_H
#define ALFAN_REKAM_GERAK_H

#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

/* 
 * Class Helper untuk mempermudah rekam gerak
*/
class RekamGerakHelper {
    private:
    DynamixelWorkbench dxl_wb;              // < Instance of DynamixelWorkbench
    const uint32_t baud_rate = 1000000;     // < Baudrate for the servo communication
    const char* usb_port = "/dev/ttyUSB0";  // < USB port for the servo communication
    const char *debuglog;                   // < Debug log for the servo communication

    public:
    /*
     * Fungsi untuk menginisialisasi koneksi ke servo
    */
    void init();

    /*
     * Fungsi untuk membaca data dari servo MX-28
     * @param id ID servo yang akan dibaca
    */
    void readMX28(int id);

    /*
     * Fungsi untuk membaca data dari servo XL-320
     * @param id ID servo yang akan dibaca
    */
    void readXL320(int id);

    /*
     * @brief Fungsi untuk debugging rekam gerak
     * @param id ID servo yang sedang direkam
     * @param presentPosition Posisi saat ini dari servo
     * @note Fungsi ini digunakan untuk menampilkan informasi debug
    */
    static void debug(uint8_t id, int32_t presentPosition);
};

#endif