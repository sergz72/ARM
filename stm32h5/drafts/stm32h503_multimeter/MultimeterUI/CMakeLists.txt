cmake_minimum_required(VERSION 3.30)
project(MultimeterUI C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_executable(MultimeterUI
        src/hal.c
        src/ui.cpp
        src/main.cpp
        src/multimeter.cpp
        src/MeasurementUnitAD7793.cpp
        src/MeasurementUnitADS1220.cpp
        src/MeasurementUnitInternal.cpp
        ../../my/Programs/ARM_CLION/common_lib/display/lcd_sh1107.c
        ../../my/Programs/ARM_CLION/common_lib/display/lcd.c
        ../../my/Programs/ARM_CLION/common_lib/display/font.c
        ../../my/Programs/ARM_CLION/common_lib/display/fonts/font10.c
        ../../my/Programs/ARM_CLION/common_lib/myprintf.c
        ../../my/Programs/ARM_CLION/common_lib/ultoa.c
        ../../my/Programs/ARM_CLION/common_lib/itoa.c
)

include_directories(
        /usr/include/gtk-4.0
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/cairo
        /usr/include/pango-1.0
        /usr/include/harfbuzz
        /usr/include/gdk-pixbuf-2.0
        /usr/include/graphene-1.0
        /usr/lib/x86_64-linux-gnu/graphene-1.0/include
        src
        ../../my/Programs/ARM_CLION/common_lib
        ../../my/Programs/ARM_CLION/common_lib/display
        ../../my/Programs/ARM_CLION/common_lib/adc
)

target_link_libraries(MultimeterUI
        /usr/lib/x86_64-linux-gnu/libgtk-4.so
        /usr/lib/x86_64-linux-gnu/libglib-2.0.so
        /usr/lib/x86_64-linux-gnu/libpango-1.0.so
        /usr/lib/x86_64-linux-gnu/libgobject-2.0.so
        /usr/lib/x86_64-linux-gnu/libcairo.so
        /usr/lib/x86_64-linux-gnu/libgdk_pixbuf-2.0.so
        /usr/lib/x86_64-linux-gnu/libharfbuzz.so
        /usr/lib/x86_64-linux-gnu/libgraphene-1.0.so
        /usr/lib/x86_64-linux-gnu/libgio-2.0.so
)