#ifndef ALFAN_COMMUNICATION_H
#define ALFAN_COMMUNICATION_H

#include <unistd.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_srvs/srv/trigger.hpp>

class Communication: public rclcpp::Node {
    private:
    /*
     * Variabel-variabel dibawah digunakan untuk berkomunikasi dengan robot lain
     * melalui node ROS2 pada program yang berbeda (alfan_communication)
    */
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub_internal_index;  // Publisher untuk tracker index gerak tari saat ini (robot sendiri)
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_other_index;  // Client untuk request index gerak tari robot satunya
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_music_state;  // Client untuk request status musik
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_panic_state;  // Client untuk request status panic button
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_req_shutdown;     // Client untuk request shutdown ketika program selesai 

    public:
    Communication();
    ~Communication();
    /*
     * @brief Fungsi untuk memulai komunikasi, request ke music box, serta rescue jika diperlukan
    */
    void startingCondition();
    
    /*
     * @brief Fungsi untuk melakukan request status musik apakah sedang dimainkan atau tidak
     * @note Jika robot dalam keadaan PANIC maka music_state dianggap 1
    */
    void requestMusic();

    /*
     * @brief Fungsi ini akan melakukan publish index gerakan saat ini ke topic {nama_robot}/internal/current_index
    */
    void publishInternalIndex();

    /*
     * @brief Fungsi ini akan melakukan request untuk mendapatkan index gerakan saat ini milik robot satunya
    */
    void updateOtherIndex();

    /*
     * @brief Fungsi ini akan melakukan request untuk mendapatkan status musik
    */
    void updateMusicState();

    /*
     * @brief Fungsi ini akan melakukan request untuk mendapatkan status apakah PANIC button sedang dipencet atau tidak
    */
    void updatePanicState();

    /*
     * @brief Fungsi ini akan melakukan request untuk shutdown node program komunikasi ketika program telah selesai dijalankan
    */
    void requestShutdown();
};

#endif