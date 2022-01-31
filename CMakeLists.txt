cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})


# имя проекта
project(bulk_server VERSION ${PROJECT_VESRION})



# настройки для boost-а:
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 



# тут не надо - asio он header only
#find_package(Boost REQUIRED COMPONENTS asio)

include_directories(${Boost_INCLUDE_DIR})



# имя основной утилиты будет таким же как и имя проекта?
add_executable(bulk_server
src/bulk_server.cpp
src/bulk_machine.cpp
src/global_place.cpp
src/server.cpp
src/sync_console.cpp
src/tcp_connect.cpp
)


set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)



target_include_directories(bulk_server PUBLIC
${PROJECT_SOURCE_DIR}/./
)



# пока не буду делать библиотеку, но по идее потом может и надо буует
#add_library(helloworld lib.cpp)
#add_executable(test_version test_version.cpp)

target_include_directories(bulk_server
    PRIVATE "${CMAKE_BINARY_DIR}"
)

#  Пока не надо - тестов не добавляю
#set_target_properties(test_version PROPERTIES
#    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
#    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
#)


# Пока всё без доп библиотек
#target_link_libraries(helloworld_cli PRIVATE
#    helloworld
#)
#target_link_libraries(test_version
#    ${Boost_LIBRARIES}
#    helloworld
#)


target_link_libraries(bulk_server ${Boost_LIBRARIES})


if (MSVC)
    target_compile_options(bulk_server PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk_server PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

