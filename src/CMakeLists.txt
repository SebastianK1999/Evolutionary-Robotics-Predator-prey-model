if (NOT TARGET ERPPM_LIB)
# Create library
    file(GLOB_RECURSE ERPPM_FILES_CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    file(GLOB_RECURSE ERPPM_FILES_HPP "${CMAKE_CURRENT_LIST_DIR}/../include/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/../include/*.inl")

    add_library(
        ERPPM_LIB
        STATIC
        ${ERPPM_FILES_HPP}
        ${ERPPM_FILES_CPP}
    )
    target_include_directories(ERPPM_LIB PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include")
    target_link_libraries(ERPPM_LIB PUBLIC ${CORE_LIBS} glm oglu JimmyNeuron libepoxy glfw GLEW whereami)
endif()


