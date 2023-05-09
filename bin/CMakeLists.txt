
file (GLOB_RECURSE EXAMPLES CONFIGURE_DEPENDS ./*.cpp)
foreach (example ${EXAMPLES})
  get_filename_component(example_name ${example} NAME_WLE)
  add_executable(${example_name} ${example})
  target_link_libraries(${example_name} ${CORE_LIBS} oglu JimmyNeuron glm glfw GLEW whereami ERPPM_LIB)
  target_compile_features(${example_name} PRIVATE cxx_std_20)
endforeach()