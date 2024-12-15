cmake_minimum_required(VERSION 3.30)
project(OvenControlUI C)

set(CMAKE_C_STANDARD 23)

add_executable(OvenControlUI
        src/hal.c
        src/ui.c
        src/main.c
        src/controller.c
        src/ws2812.c
        ../../my/Programs/ARM_CLION/common_lib/display/lcd_ssd1306.c
        ../../my/Programs/ARM_CLION/common_lib/display/lcd.c
        ../../my/Programs/ARM_CLION/common_lib/display/font.c
        ../../my/Programs/ARM_CLION/common_lib/display/fonts/font36.c
        ../../my/Programs/ARM_CLION/common_lib/display/fonts/font18.c
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
        ../../my/Programs/ARM_CLION/common_lib/shell
        ../../my/Programs/ARM_CLION/common_lib/display
)

target_link_libraries(OvenControlUI
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