cmake_minimum_required(VERSION 3.8)
project(program_tangan_alfan)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(dynamixel_workbench_toolbox REQUIRED)

# Add the gerak_tari library
add_library(alfan_lib_main src/lib_alfan.cpp src/tangan_controller.cpp)
add_library(comms_lib src/communication.cpp)
add_library(gerak_tari_lib src/gerak_tari.cpp)

target_include_directories(alfan_lib_main PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(comms_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(gerak_tari_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Link this library to your executables
# target_link_libraries(tangan_driver gerak_tari_lib)

# Define executable
add_executable(rekam_gerak src/rekam_gerak.cpp)
add_executable(tangan_driver src/tangan_driver.cpp)

target_include_directories(rekam_gerak PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(tangan_driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(rekam_gerak PUBLIC c_std_99 cxx_std_17)
target_compile_features(tangan_driver PUBLIC c_std_99 cxx_std_17)

# Ensure gerak_tari_lib links to required dependencies
ament_target_dependencies(alfan_lib_main rclcpp dynamixel_workbench_toolbox)
ament_target_dependencies(comms_lib)
ament_target_dependencies(gerak_tari_lib rclcpp dynamixel_workbench_toolbox)

# Link RapidYAML to gerak_tari_lib
target_link_libraries(gerak_tari_lib ryml comms_lib)

# Link dependencies
ament_target_dependencies(rekam_gerak rclcpp dynamixel_workbench_toolbox)
ament_target_dependencies(tangan_driver rclcpp dynamixel_workbench_toolbox)

# Library Rapid YAML
add_subdirectory(external/rapidyaml ryml)
target_link_libraries(rekam_gerak alfan_lib_main)
target_link_libraries(tangan_driver ryml alfan_lib_main comms_lib gerak_tari_lib)


# Install target and include directory
install(TARGETS rekam_gerak tangan_driver
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
