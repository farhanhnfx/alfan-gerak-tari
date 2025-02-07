cmake_minimum_required(VERSION 3.8)
project(program_tangan_alfan)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(dynamixel_workbench_toolbox REQUIRED)

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

# Link dependencies
ament_target_dependencies(rekam_gerak rclcpp dynamixel_workbench_toolbox)
ament_target_dependencies(tangan_driver rclcpp dynamixel_workbench_toolbox)


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
