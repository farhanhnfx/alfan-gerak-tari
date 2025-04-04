#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <unistd.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_srvs/srv/trigger.hpp>

extern bool panic_mode;     // Status mode panic (1) atau tidak (0)
extern bool rescue_mode;    // Status mode rescue (1) atau tidak (0)
extern int music_state;     // Status musik apakah play (1) atau mute (0)
extern int gerak_tari_current_index; // Tracker indeks gerak tari ke-berapa
extern int data_gerak_tari_others_index;    // Tracker data dari robot satunya
extern int count_time; // variabel untuk membantu sinkronisasi gerakan

extern std::string THIS_ROBOT_NAME;
extern std::string OTHER_ROBOT_NAME;

class Communication: public rclcpp::Node {
    private:
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub_internal_index;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_other_index;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_music_state;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_get_panic_state;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cli_req_shutdown;

    public:
    Communication();
    ~Communication();
    void starting_condition();
    void request_music();
    void publish_internal_index();
    void update_other_index();
    void update_music_state();
    void update_panic_state();
    void request_shutdown();
    

};


#endif