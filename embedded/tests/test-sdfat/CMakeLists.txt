project(test-sdfat)

add_executable(${PROJECT_NAME} 
    QuickStart.cpp
    main.cpp
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    SdFat
    core
    )

