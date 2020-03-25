#
# Copyright (c) 2008-2019 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Define target name
set( TARGET_NAME 00_relative_pose )

include_directories( "./src"
                     "./src/math"
                     "./src/item"
                     "./src/sphere_tools"
                     "./src/resistance"
                     "./src/noise"
                     "./src/objects"
                     "./src/objects/sphere"
                     "./src/kepler"
                     "./src/physics"
                     "./src/ui"
                     "./src/3dparty"
                     "./src/3dparty/Config"
                     "./src/3dparty/Messages"
                     "./src/3dparty/Audio"
                     "./src/3dparty/Input"
                     "./src/3dparty/Console"
                     "./src/3dparty/Ui"
                     "./src/3dparty/Ui/Pause"
                     "./src/3dparty/Ui/PopupMessage"
                     "./src/3dparty/Ui/QuitConfirmation"
                     "./src/3dparty/Ui/Settings"
                     "./3dparty/eigen-3.3.7" 
                     "./3dparty/pcg-cpp" 
                     "./3dparty/pi_random" 
                     )

file( GLOB_RECURSE src src/*.cpp )

# Define source files
define_source_files( EXTRA_CPP_FILES ${src}
                     EXTRA_H_FILES   ${COMMON_SAMPLE_H_FILES}
                   )


# Setup target with resource copying
setup_main_executable()

#target_compile_features( ${TARGET_NAME} PUBLIC cxx_range_for )

# Setup test cases
#setup_test()



