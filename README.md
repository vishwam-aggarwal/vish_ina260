I2C Current Sensor Library for the Raspberry Pi Pico. Loose port of the Adafruit INA260 library for Arduino.

This uses lower level repos, one must run:

git submodule update --recursive

to load the nested repos

The repo folder should live in the project folder and must be included in the linked target libraries in the CMakeLists.txt file as follows:

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME} 
        vish_ina260
        )
