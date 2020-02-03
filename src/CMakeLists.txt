add_executable(
        rs2-config-clear
        WIN32
        main.cpp
        MainWidget.cpp
        MainWidget.hpp
        Utils.cpp
        Utils.hpp)

target_link_libraries(rs2-config-clear Qt5::Widgets)
