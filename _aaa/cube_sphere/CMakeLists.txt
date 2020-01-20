# Define target name
set( TARGET_NAME 00_sphere_utils )

include_directories( "./src"
                     "./src/math"
                     "./src/sphere_tools"
                     )

file( GLOB_RECURSE src src/*.cpp )

# Define source files
define_source_files( EXTRA_CPP_FILES ${src}
                     EXTRA_H_FILES   ${COMMON_SAMPLE_H_FILES}
                   )


# Setup target with resource copying
setup_main_executable()

# Setup test cases
#setup_test()



