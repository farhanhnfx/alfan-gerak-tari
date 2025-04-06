#include <alfan_gerak_tari/core/communication.h>
#include <alfan_gerak_tari/globals.h>

Communication::Communication(): Node(THIS_ROBOT_NAME + "_communication_manager_cpp"){
    pub_internal_index = this->create_publisher<std_msgs::msg::Int32>(THIS_ROBOT_NAME + "/internal/current_index", 10);
    cli_get_other_index = this->create_client<std_srvs::srv::Trigger>(OTHER_ROBOT_NAME + "/get_current_index");
    cli_get_music_state = this->create_client<std_srvs::srv::Trigger>(THIS_ROBOT_NAME + "/get_music_state");
    cli_get_panic_state = this->create_client<std_srvs::srv::Trigger>(THIS_ROBOT_NAME + "/get_panic_state");
    cli_req_shutdown = this->create_client<std_srvs::srv::Trigger>(THIS_ROBOT_NAME + "/request_shutdown");

    panic_mode = true;
    rescue_mode = false;
    music_state = 0;   
    count_time = 0; // variabel untuk membantu sinkronisasi gerakan
    RCLCPP_INFO(this->get_logger(), "Starting Communication");
}

Communication::~Communication() {}

void Communication::startingCondition() {
    while (music_state == 0) {
        requestMusic();
        usleep(500);
    }

    RCLCPP_INFO(this->get_logger(), "Music oke");

    publishInternalIndex();
    updateOtherIndex();

    // Sinkronisasi nilai dari indeks gerak tari sekarang dan data yang didapat dari robot lain
    // untuk keadaan rescue
    while (gerak_tari_current_index != data_gerak_tari_others_index && data_gerak_tari_others_index != 0) {
        RCLCPP_INFO(this->get_logger(), "Data yang diterima berbeda");
        // Proses sinkronisasi indeks/counter gerakan
        if (gerak_tari_current_index < data_gerak_tari_others_index) {
            RCLCPP_INFO(this->get_logger(), "RESCUING.......");
            gerak_tari_current_index = data_gerak_tari_others_index;
            rescue_mode = true;
            // sinkronisasi harusnya berhasil
        }
    }

    if (rescue_mode) {
        gerak_tari_current_index++;
    }

    publishInternalIndex();
}

void Communication::requestMusic() {
    updatePanicState();

    if (panic_mode) {
        // Masuk panic mode, langsung assign 1 ke music_state
        music_state = 1;
        return;
    }
    
    updateMusicState();
}

void Communication::publishInternalIndex() {
    std_msgs::msg::Int32 msg;
    msg.data = gerak_tari_current_index;
    pub_internal_index->publish(msg);
}

void Communication::updateOtherIndex() {
    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto result = cli_get_other_index->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result, std::chrono::milliseconds(100)) == rclcpp::FutureReturnCode::SUCCESS) {
        data_gerak_tari_others_index = std::stoi(result.get()->message);
        RCLCPP_INFO(this->get_logger(), "Data Gerak Tari Other Index: %d", data_gerak_tari_others_index);
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get other index (TIMEOUT)");
    }
    count_time = 0; // Reset count_time after updating other index
}

void Communication::updateMusicState() {
    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto result = cli_get_music_state->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result, std::chrono::milliseconds(50)) == rclcpp::FutureReturnCode::SUCCESS) {
        music_state = std::stoi(result.get()->message);
        // RCLCPP_INFO(this->get_logger(), "Music State: %d", music_state);
    } else {
        // RCLCPP_ERROR(this->get_logger(), "Failed to get music state (TIMEOUT)");
    }
}

void Communication::updatePanicState() {
    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto result = cli_get_panic_state->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result, std::chrono::milliseconds(50)) == rclcpp::FutureReturnCode::SUCCESS) {
        panic_mode = std::stoi(result.get()->message);
        // RCLCPP_INFO(this->get_logger(), "Panic Mode: %d", panic_mode);
    } else {
        // RCLCPP_ERROR(this->get_logger(), "Failed to get panic state (TIMEOUT)");
    }
}

void Communication::requestShutdown() {
    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto result = cli_req_shutdown->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result, std::chrono::milliseconds(200)) == rclcpp::FutureReturnCode::SUCCESS) {
        RCLCPP_INFO(this->get_logger(), "Shutdown request sent");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to send shutdown request (TIMEOUT)");
    }
}

