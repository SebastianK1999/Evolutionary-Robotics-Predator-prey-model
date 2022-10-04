#glm
#add_subdirectory ("glm")

# OpenGL-Utils
add_subdirectory ("oglUtils")

# JimmyNeuron
if (NOT TARGET JimmyNeuron)
    add_subdirectory("JimmyNeuron")
endif()


# whereami
if (NOT TARGET whereami)
    file(GLOB_RECURSE WHEREAMI_FILES_HPP "${CMAKE_CURRENT_SOURCE_DIR}/whereami/src/*.h") 
    file(GLOB_RECURSE WHEREAMI_FILES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/whereami/src/*.c")
    add_library(
        whereami 
        STATIC 
        ${WHEREAMI_FILES_HPP}
        ${WHEREAMI_FILES_CPP}
    )
    set_target_properties(whereami PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(whereami PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/whereami/src/")
endif()