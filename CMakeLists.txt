cmake_minimum_required(VERSION 3.10.2)
project(EvolutionaryRobotics_PredatorPreyModel VERSION 0.10)


# it suuposed to copy file if there is a new wersion of it
# probably, i still need to change it cause it's not working correctly 

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_subdirectory("src") # adding libraries
add_subdirectory("bin") # linking to executable
add_subdirectory("submodules") # linking to submodules
