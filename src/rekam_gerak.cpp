// #include <rclcpp/rclcpp.hpp>
#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <program_rekam_gerak/rekam_gerak.h>


class RekamGerak : public rclcpp::Node
{
public:
  RekamGerak()
  : Node("rekam_gerak")
  {
    // string filename = FileManager::generateFilename();
    // readServoMX28(21);

    // FileManager::createFile(filename, fileDataTxt, fileDataJson);
  }

  void spinNode() {
    TerminalHelper::saveOriginalTerminal();
    while (rclcpp::ok()) {
      int key = TerminalHelper::getKeyPress();
      if (key != -1) {
        RCLCPP_INFO(this->get_logger(), "Key pressed: %c", key);
        if (key == 'q') {
          TerminalHelper::resetTerminal();
          break;
        }
        else if (key == 'r') {
          RCLCPP_INFO(this->get_logger(), "Rekam gerak...");
          string filename = FileManager::generateFilename();
          readServoMX28(21);
          readServoXL320(22);
          FileManager::createFile(filename, fileDataTxt, fileDataJson);
        }
        else if (key == 'e') {
          errorBaca = true;
          RCLCPP_INFO(this->get_logger(), "Rekam gerak...");
          string filename = FileManager::generateFilename();
          readServoMX28(21);
          readServoXL320(22);
          FileManager::createFile(filename, fileDataTxt, fileDataJson);
        }
      }
    }
  }

private:
  DynamixelWorkbench dxl_wb_;
  std::string port_name_;
  int baud_rate_;

  void scan_for_devices() {
    uint8_t dxl_ids[254]; // Array to store found IDs
    uint8_t found_devices = 0;

    RCLCPP_INFO(this->get_logger(), "Scanning for Dynamixel devices on %s...", port_name_.c_str());
    if (dxl_wb_.scan(dxl_ids, &found_devices, 0, 253, nullptr)) {
      if (found_devices > 0) {
        RCLCPP_INFO(this->get_logger(), "Found %d Dynamixel device(s):", found_devices);
        for (uint8_t i = 0; i < found_devices; ++i) {
          RCLCPP_INFO(this->get_logger(), "  - ID: %d", static_cast<int>(dxl_ids[i]));
        }
      } else {
        RCLCPP_WARN(this->get_logger(), "No Dynamixel devices found.");
      }
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to scan for devices.");
    }
  }

  void readServoMX28(int id) {
      int defaultInByte = RekamGerakHelper::degreeToByteMX28(Default[id]);
      // int presentPosition =  rand()%4095; // TO DO: Read the present position from the servo
      int32_t presentPosition = defaultInByte;

      FileManager::writeFileData(id, selisihPresentDefault);

      RekamGerakHelper::debugRekam(id, presentPosition);
  }

  

  void readServoXL320(int id) {
      int defaultIntByte = RekamGerakHelper::degreeToByteXL320(Default[id]);
      int presentPosition = rand()%1023; // TO DO: Read the present position from the servo

      selisihPresentDefault = presentPosition - defaultIntByte;

      FileManager::writeFileData(id, selisihPresentDefault);

      RekamGerakHelper::debugRekam(id, presentPosition);
  }
  
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<RekamGerak>();
  // rclcpp::spin(node);
  node->spinNode();
  rclcpp::shutdown();
  return 0;
}