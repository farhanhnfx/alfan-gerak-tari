#ifndef TANGAN_CONTROLLER_H
#define TANGAN_CONTROLLER_H

#include <program_rekam_gerak/lib_alfan.h>
// #include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

// /* ENUM FOR CONTROL TABLE ADDRESS
//     SERVO MX-28 DAN XL-320 
//     DETAILNYA BISA DICEK DI DOKUMENTASI RESMI
//     MX-28 DAN XL-320 
//     */
// enum ControlTableAddress {
//     R_PROFILE_VELOCITY_MX28 = 112,  // LENGTH 4
//     R_GOAL_POSITION_MX28 = 116,     // LENGTH 4
    
//     R_GOAL_POSITION_XL320 = 30,     // LENGTH 2
//     R_MOVING_SPEED_XL320 = 32       // LENGTH 2
// };

/* Conversion Factor untuk Menghitung Moving Speed XL-320
   dan Profile Velocity MX-28
   Penjelasan perhitungan dapat dilihat di:
   https://docs.google.com/document/d/14YH6WwX3MLx6o_tqQHlnr0oFchm8Jjc4bFIMNI2iFy8 */
const float CONVERSION_FACTOR_MX28 = 0.1374;
const float CONVERSION_FACTOR_XL320 = 0.0666;


class TanganController {
    public:
    TanganController();
    /* Sama seperti baca(int gerak, float speed) di program lama (SEMOGA) */
    void bacaGerak(int gerakCounter, float speed);
    void transmit();
    void toDefaultPose();

    private:
    DynamixelWorkbench dxl_wb;
    const uint32_t baud_rate = 1000000;
    const char* usb_port = "/dev/ttyUSB0";

    /* Buat nampung perhitungan Goal Position &
        Moving Speed masing-masing servo */
    int32_t goal_positions[45];
    int32_t moving_speeds[45];
    // Untuk membantu perhitungan moving_speed
    int32_t latestPositionInDegree[45];
    uint32_t degreeDifference[45];

    /* ID Servo Tangan (MX-28 & XL-320) */
    uint8_t mx28_ids[2] = {21, 31};
    uint8_t xl320_ids[10] = {22, 23, 24, 25, 26, 32, 33, 34, 35, 36};

    // uint8_t mx28_profile_velocity_handler;
    // uint8_t mx28_goal_position_handler;
    // uint8_t xl320_goal_position_handler;
    // uint8_t xl320_moving_speed_handler;

    uint8_t handler_index_count;

    void init();
    void setupSyncWriteHandler();
    /* processMotionFrame = baca() di program lama */
    void processMotionFrame(int gerakCounter, float speed);
    /* Menghitung perhitungan Moving Speed (XL-320) dan Profile Velocity (MX-28) */
    void calculateSpeeds(int id, int goalPosition, float speed);
    void updateLatestPositionInDegree(int id, int goalPosition);
    void sendMovementCommands();
};

#endif