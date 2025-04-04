#include <program_rekam_gerak/communication.h>

#include <sstream>


bool panic_mode = true;     // Status mode panic (1) atau tidak (0)
bool rescue_mode = false;    // Status mode rescue (1) atau tidak (0)
int music_state = 0;     // Status musik apakah play (1) atau mute (0)
int gerak_tari_current_index = 0; // Tracker indeks gerak tari ke-berapa
int data_gerak_tari_others_index = 0;    // Tracker data dari robot satunya
int count_time = 0; // variabel untuk membantu sinkronisasi gerakan

Communication::Communication() {
    rescue_mode = false;
    music_state = 0;   
    count_time = 0; // variabel untuk membantu sinkronisasi gerakan
    printf("Starting Communication\n");
    starting_condition();
}

bool Communication::can_communicate_with_others() {
    /*
        TO DO:
        Implementasi kode untuk mengetahui apakah
        dapat berkomunikasi dengan robot satunya
    */
    return false; // value dummy sementara
}

int Communication::read_others_data() {
    /*  
        TO DO:
        Implementasi kode untuk mendapatkan/membaca
        data dari robot satunya
    */
   return 0; // value dummy sementara
}

void Communication::starting_condition() {
    while (music_state == 0) {
        request_music();
        usleep(500);
    }

    printf("\tMusic oke\n");

    // if (can_communicate_with_others()) {
    //     transmit_data(music_state, gerak_tari_current_index);
    //     receives_data();
    // }

    request_comm_service();

    // Sinkronisasi nilai dari indeks gerak tari sekarang dan data yang didapat dari robot lain
    // untuk keadaan rescue
    while (gerak_tari_current_index != data_gerak_tari_others_index && data_gerak_tari_others_index != 0) {
        if (can_communicate_with_others()) {
            receives_data();
            // Proses sinkronisasi indeks/counter gerakan
            if (gerak_tari_current_index < data_gerak_tari_others_index) {
                // artinya robot ini ketinggalan geraknya
                gerak_tari_current_index = data_gerak_tari_others_index;
                rescue_mode = true;
                // sinkronisasi harusnya berhasil
            }
        }
    }

    if (rescue_mode) {
        gerak_tari_current_index++;
    }
}

void Communication::request_music() {
    if (panic_mode) {
        // Masuk panic mode, langsung assign 1 ke music_state
        music_state = 1;
        return;
    }

    /* 
        TO DO:
        Implementasi kode untuk komunikasi dengan Music Box
    */

}

void Communication::transmit_data(bool music_condition, int count) {
    // Komunikasi ke robot satunya
    // Enkripsi sederhana
    int packet_data = (music_condition * 128) + count;
    /*
        TO DO:
        Implementasi kode untuk mengirim data ke robot satunya
    */

}

void Communication::receives_data() {
    if (can_communicate_with_others()) {
        int data_from_others = read_others_data();
        // Decrypt data
        int is_music_on = data_from_others / 128;
        
        data_gerak_tari_others_index = data_from_others - (128 * is_music_on);
        count_time = 0;
    }
}

void Communication::request_comm_service() {
    rclcpp::init(0, nullptr);

    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("communication_client");
    rclcpp::Client<communication_interfaces::srv::CommunicationData>::SharedPtr client = node->create_client<communication_interfaces::srv::CommunicationData>("communication_data");

    auto request = std::make_shared<communication_interfaces::srv::CommunicationData::Request>();
    request->music_state = music_state;
    request->current_gerak_tari_index = gerak_tari_current_index;

    while (!client->wait_for_service(std::chrono::seconds(1))) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for the service. Exiting.");
            return;
        }
        RCLCPP_INFO(node->get_logger(), "Service not available, waiting again...");
    }

    auto result = client->async_send_request(request);

    if (rclcpp::spin_until_future_complete(node, result) == rclcpp::FutureReturnCode::SUCCESS) {
        RCLCPP_INFO(node->get_logger(), "Response: %d", result.get()->received_other_index);
        std::cout << "Response: " << result.get()->received_other_index << std::endl;

        data_gerak_tari_others_index = result.get()->received_other_index;
    } else {
        RCLCPP_ERROR(node->get_logger(), "Failed to call service");
    }
}