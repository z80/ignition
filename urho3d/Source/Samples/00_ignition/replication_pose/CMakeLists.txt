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
set( TARGET_NAME replication_pose )

include_directories( "../relative_pose/src"
                     "../relative_pose/src/math"
                     "../relative_pose/src/item"
                     "../relative_pose/src/sphere_tools"
                     "../relative_pose/src/objects"
                     "../relative_pose/src/objects/sphere"
                     "../relative_pose/src/physics"
                     "../relative_pose/src/ui"
                     "../relative_pose/src/kepler"
                     "../relative_pose/src/3dparty"
                     "../relative_pose/src/3dparty/Config"
                     "../relative_pose/src/3dparty/Messages"
                     "../relative_pose/src/3dparty/Audio"
                     "../relative_pose/src/3dparty/Input"
                     "../relative_pose/src/3dparty/Console"
                     "../relative_pose/src/3dparty/Ui"
                     "../relative_pose/src/3dparty/Ui/Pause"
                     "../relative_pose/src/3dparty/Ui/PopupMessage"
                     "../relative_pose/src/3dparty/Ui/QuitConfirmation"
                     "../relative_pose/src/3dparty/Ui/Settings"
                     "./3dparty/eigen-3.3.7" )

file( GLOB_RECURSE src "../relative_pose/src/*.cpp" )

#message( WARNING "src: ${src}" )

# Define source files
define_source_files( EXTRA_CPP_FILES ${src}
                     EXTRA_H_FILES   ${COMMON_SAMPLE_H_FILES}
                   )


# Setup target with resource copying
setup_main_executable()

# Setup test cases
#setup_test()



