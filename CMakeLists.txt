# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
# Minimalna wersja CMake
cmake_minimum_required(VERSION 3.5)

# Dołączenie pliku CMake z SDK (ten plik ustawia środowisko ESP-IDF)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Ustawienie zmiennej środowiskowej z ścieżką do SDK i własnych komponentów (wewnątrz kontenera)
set(EXTRA_COMPONENT_DIRS /opt/esp/ESP8266_RTOS_SDK/components
    "${PROJECT_DIR}/../components")

# Nazwa projektu
project(nESP_WiFi_Tutorials)