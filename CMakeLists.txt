cmake_minimum_required(VERSION 3.8)
project(alfan_gerak_tari)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(dynamixel_workbench_toolbox REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

add_library(lib_globals src/globals.cpp)
add_library(lib_io_filemgr src/io/file_manager.cpp)
add_library(lib_io_yamlutils src/io/yaml_utils.cpp)
add_library(lib_utils_terminal src/utils/terminal_helper.cpp)
add_library(lib_core_servomgr src/core/servo_manager.cpp)
add_library(lib_core_comms src/core/communication.cpp)
add_library(lib_core_tangan src/core/tangan.cpp)
add_library(lib_core_kepala src/core/kepala.cpp)
add_library(lib_core_geraktari src/core/gerak_tari.cpp src/core/servo_manager.cpp src/core/communication.cpp src/core/tangan.cpp src/core/kepala.cpp)
add_library(lib_recording src/record/rekam_gerak.cpp)

target_include_directories(lib_globals PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_io_filemgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_io_yamlutils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_utils_terminal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_core_servomgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_core_comms PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_core_tangan PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_core_kepala PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_core_geraktari PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(lib_recording PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Define executable
add_executable(rekam_gerak src/driver_rekam_gerak.cpp)
add_executable(gerak_tari src/driver_gerak_tari.cpp)

target_include_directories(rekam_gerak PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(gerak_tari PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(rekam_gerak PUBLIC c_std_99 cxx_std_17)
target_compile_features(gerak_tari PUBLIC c_std_99 cxx_std_17)

# Ensure gerak_tari_lib links to required dependencies
ament_target_dependencies(lib_core_servomgr rclcpp dynamixel_workbench_toolbox)
ament_target_dependencies(lib_core_comms rclcpp std_msgs std_srvs)
# the one that was missing:
ament_target_dependencies(
  lib_core_tangan
  rclcpp
  dynamixel_workbench_toolbox
)
ament_target_dependencies(
  lib_core_kepala
  rclcpp
  dynamixel_workbench_toolbox
)
ament_target_dependencies(lib_core_geraktari rclcpp std_msgs std_srvs dynamixel_workbench_toolbox)
# recording library (also missing):
ament_target_dependencies(
  lib_recording
  rclcpp
  dynamixel_workbench_toolbox
)


# Library Rapid YAML
add_subdirectory(external/rapidyaml ryml)

# Link RapidYAML to gerak_tari_lib
target_link_libraries(lib_io_yamlutils ryml)

# either link the ryml target into your library:
target_link_libraries(lib_core_geraktari
    ryml
)

# Link dependencies
ament_target_dependencies(rekam_gerak rclcpp dynamixel_workbench_toolbox)
ament_target_dependencies(gerak_tari rclcpp dynamixel_workbench_toolbox)

target_link_libraries(rekam_gerak ryml lib_globals lib_recording lib_io_yamlutils lib_io_filemgr lib_utils_terminal)
target_link_libraries(gerak_tari ryml lib_globals lib_core_servomgr lib_core_comms lib_core_tangan lib_core_kepala lib_core_geraktari lib_io_yamlutils lib_io_filemgr lib_utils_terminal)


# Install target and include directory
install(TARGETS rekam_gerak gerak_tari
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
