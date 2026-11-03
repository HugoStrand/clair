# Tell c2py it is run within clair.
# c2py is a subproject but in this specific case, we want to build its tests 
set(IS_CLAIR_BUILD ON) 

FetchContent_Declare(
  c2py
  GIT_REPOSITORY  https://github.com/flatironinstitute/c2py
  GIT_TAG         unstable
  EXCLUDE_FROM_ALL # exclude from all so that c2py is NOT installed.
)
FetchContent_MakeAvailable(c2py)

file(COPY ${PROJECT_SOURCE_DIR}/.clang-format DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# We add the tests back to the ALL to have them run
add_custom_target(c2py_make_all_tests ALL DEPENDS ${c2py_all_full_tests} ${c2py_all_low_level_tests})

# Keep clair-specific fixtures and generated bindings in the build tree.
foreach(fixture factory_constructor factory_constructor_invalid)
  configure_file(${fixture}.cpp ${fixture}.cpp COPYONLY)
  configure_file(${fixture}.toml ${fixture}.toml COPYONLY)
endforeach()

Python_add_library(factory_constructor MODULE ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.cpp)
target_link_libraries(factory_constructor PRIVATE c2py c2py_warnings)
# Optional NRVO must not hide an accidental copy/move of the immovable product.
target_compile_options(factory_constructor PRIVATE -fno-elide-constructors)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.wrap.cxx
         ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.wrap.hxx
  COMMAND $<TARGET_FILE:clair-c2py> -p ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.cpp
  DEPENDS clair-c2py ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.cpp
          ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.toml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(generate_factory_constructor DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor.wrap.cxx)
add_dependencies(factory_constructor generate_factory_constructor)
add_test(NAME factory_constructor_py
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/factory_constructor_test.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
set_property(TEST factory_constructor_py APPEND PROPERTY ENVIRONMENT
  PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH} ${SANITIZER_RT_PRELOAD}
)

# This target supplies the exact compiler flags for diagnostic tests, without
# compiling them or modifying the fetched c2py tests.
add_library(factory_constructor_invalid_compile_db OBJECT EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor_invalid.cpp)
target_link_libraries(factory_constructor_invalid_compile_db PRIVATE c2py)
add_test(NAME factory_constructor_invalid
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/factory_constructor_invalid_test.py
    $<TARGET_FILE:clair-c2py> ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/factory_constructor_invalid.cpp
)
set_tests_properties(factory_constructor_invalid PROPERTIES TIMEOUT 180)
