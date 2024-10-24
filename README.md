# nESP_WiFi_Tutorials

Repository for getting knowledge about WiFi connection in ESP8266 and ESP32 (programing in ESP-IDF).

# How to setup all project (will write soon)

# My project structure
Information about Project Building:
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html

NOTE: After changes in CMakeLists.txt use "idf.py reconfigure"!

        my_project/
        ├── CMakeLists.txt              # Główny plik CMake
        ├── main/
        │   ├── CMakeLists.txt          # Plik CMake dla katalogu main
        │   ├── main.h                  # Nagłowek dla pliku źródłowego
        │   └── main.c                  # Główny plik źródłowy
        ├── components/
        │   ├── init_fun
        │	    ├── CMakeLists.txt  # Plik CMake dla komponentu init_fun
        │	    ├── init_fun.h      # Własne bibloteki
        │   	└── init_fun.c          # Nagłówek własnej bibloteki
        │   └── ...                     # Inne komponenty
        ├── .devcontainer/
        │   ├── devcontainer.json
        │   └── Dockerfile
        ├── .vscode/
        │   ├── c_cpp_properties.json
        │   └── settings.json
        └── README.md
