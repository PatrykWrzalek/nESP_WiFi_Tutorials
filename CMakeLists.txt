# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
# Minimalna wersja CMake
cmake_minimum_required(VERSION 3.5)

# Ustawienie zmiennej środowiskowej z ścieżką do SDK (wewnątrz kontenera)
set(EXTRA_COMPONENT_DIRS /opt/esp/ESP8266_RTOS_SDK/components)

# Dołączenie pliku CMake z SDK
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Nazwa projektu
project(nESP_WiFi_Tutorials)