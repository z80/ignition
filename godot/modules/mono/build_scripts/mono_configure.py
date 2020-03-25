import os
import os.path
import sys
import subprocess

from SCons.Script import Dir, Environment

if os.name == 'nt':
    from . import mono_reg_utils as monoreg


android_arch_dirs = {
    'armv7': 'armeabi-v7a',
    'arm64v8': 'arm64-v8a',
    'x86': 'x86',
    'x86_64': 'x86_64'
}


def get_android_out_dir(env):
    return os.path.join(Dir('#platform/android/java/lib/libs').abspath,
                        'release' if env['target'] == 'release' else 'debug',
                        android_arch_dirs[env['android_arch']])


def find_file_in_dir(directory, files, prefix='', extension=''):
    if not extension.startswith('.'):
        extension = '.' + extension
    for curfile in files:
        if os.path.isfile(os.path.join(directory, prefix + curfile + extension)):
            return curfile
    return ''


def copy_file(src_dir, dst_dir, name):
    from shutil import copy

    src_path = os.path.join(Dir(src_dir).abspath, name)
    dst_dir = Dir(dst_dir).abspath

    if not os.path.isdir(dst_dir):
        os.makedirs(dst_dir)

    copy(src_path, dst_dir)


def is_desktop(platform):
    return platform in ['windows', 'osx', 'x11', 'server', 'uwp', 'haiku']


def is_unix_like(platform):
    return platform in ['osx', 'x11', 'server', 'android', 'haiku']


def module_supports_tools_on(platform):
    return platform not in ['android', 'javascript']


def find_wasm_src_dir(mono_root):
    hint_dirs = [
        os.path.join(mono_root, 'src'),
        os.path.join(mono_root, '../src'),
    ]
    for hint_dir in hint_dirs:
        if os.path.isfile(os.path.join(hint_dir, 'driver.c')):
            return hint_dir
    return ''


def configure(env, env_mono):
    bits = env['bits']
    is_android = env['platform'] == 'android'
    is_javascript = env['platform'] == 'javascript'

    tools_enabled = env['tools']
    mono_static = env['mono_static']
    copy_mono_root = env['copy_mono_root']

    mono_prefix = env['mono_prefix']

    mono_lib_names = ['mono-2.0-sgen', 'monosgen-2.0']

    is_travis = os.environ.get('TRAVIS') == 'true'

    if is_travis:
        # Travis CI may have a Mono version lower than 5.12
        env_mono.Append(CPPDEFINES=['NO_PENDING_EXCEPTIONS'])

    if is_android and not env['android_arch'] in android_arch_dirs:
        raise RuntimeError('This module does not support the specified \'android_arch\': ' + env['android_arch'])

    if tools_enabled and not module_supports_tools_on(env['platform']):
        # TODO:
        # Android: We have to add the data directory to the apk, concretely the Api and Tools folders.
        raise RuntimeError('This module does not currently support building for this platform with tools enabled')

    if is_android and mono_static:
        # Android: When static linking and doing something that requires libmono-native, we get a dlopen error as libmono-native seems to depend on libmonosgen-2.0
        raise RuntimeError('Statically linking Mono is not currently supported on this platform')

    if is_javascript:
        mono_static = True

    if not mono_prefix and (os.getenv('MONO32_PREFIX') or os.getenv('MONO64_PREFIX')):
        print("WARNING: The environment variables 'MONO32_PREFIX' and 'MONO64_PREFIX' are deprecated; use the 'mono_prefix' SCons parameter instead")

    if env['platform'] == 'windows':
        mono_root = mono_prefix

        if not mono_root and os.name == 'nt':
            mono_root = monoreg.find_mono_root_dir(bits)

        if not mono_root:
            raise RuntimeError("Mono installation directory not found; specify one manually with the 'mono_prefix' SCons parameter")

        print('Found Mono root directory: ' + mono_root)

        mono_lib_path = os.path.join(mono_root, 'lib')

        env.Append(LIBPATH=mono_lib_path)
        env_mono.Prepend(CPPPATH=os.path.join(mono_root, 'include', 'mono-2.0'))

        lib_suffix = Environment()['LIBSUFFIX']

        if mono_static:
            if env.msvc:
                mono_static_lib_name = 'libmono-static-sgen'
            else:
                mono_static_lib_name = 'libmonosgen-2.0'

            if not os.path.isfile(os.path.join(mono_lib_path, mono_static_lib_name + lib_suffix)):
                raise RuntimeError('Could not find static mono library in: ' + mono_lib_path)

            if env.msvc:
                env.Append(LINKFLAGS=mono_static_lib_name + lib_suffix)

                env.Append(LINKFLAGS='Mincore' + lib_suffix)
                env.Append(LINKFLAGS='msvcrt' + lib_suffix)
                env.Append(LINKFLAGS='LIBCMT' + lib_suffix)
                env.Append(LINKFLAGS='Psapi' + lib_suffix)
            else:
                env.Append(LINKFLAGS=os.path.join(mono_lib_path, mono_static_lib_name + lib_suffix))

                env.Append(LIBS=['psapi'])
                env.Append(LIBS=['version'])
        else:
            mono_lib_name = find_file_in_dir(mono_lib_path, mono_lib_names, extension=lib_suffix)

            if not mono_lib_name:
                raise RuntimeError('Could not find mono library in: ' + mono_lib_path)

            if env.msvc:
                env.Append(LINKFLAGS=mono_lib_name + lib_suffix)
            else:
                env.Append(LIBS=[mono_lib_name])

            mono_bin_path = os.path.join(mono_root, 'bin')

            mono_dll_name = find_file_in_dir(mono_bin_path, mono_lib_names, extension='.dll')

            if not mono_dll_name:
                raise RuntimeError('Could not find mono shared library in: ' + mono_bin_path)

            copy_file(mono_bin_path, '#bin', mono_dll_name + '.dll')
    else:
        is_apple = env['platform'] in ['osx', 'iphone']

        sharedlib_ext = '.dylib' if is_apple else '.so'

        mono_root = mono_prefix
        mono_lib_path = ''
        mono_so_name = ''

        if not mono_root and (is_android or is_javascript):
            raise RuntimeError("Mono installation directory not found; specify one manually with the 'mono_prefix' SCons parameter")

        if not mono_root and is_apple:
            # Try with some known directories under OSX
            hint_dirs = ['/Library/Frameworks/Mono.framework/Versions/Current', '/usr/local/var/homebrew/linked/mono']
            for hint_dir in hint_dirs:
                if os.path.isdir(hint_dir):
                    mono_root = hint_dir
                    break

        # We can't use pkg-config to link mono statically,
        # but we can still use it to find the mono root directory
        if not mono_root and mono_static:
            mono_root = pkgconfig_try_find_mono_root(mono_lib_names, sharedlib_ext)
            if not mono_root:
                raise RuntimeError("Building with mono_static=yes, but failed to find the mono prefix with pkg-config; " + \
                    "specify one manually with the 'mono_prefix' SCons parameter")

        if mono_root:
            print('Found Mono root directory: ' + mono_root)

            mono_lib_path = os.path.join(mono_root, 'lib')

            env.Append(LIBPATH=[mono_lib_path])
            env_mono.Prepend(CPPPATH=os.path.join(mono_root, 'include', 'mono-2.0'))

            mono_lib = find_file_in_dir(mono_lib_path, mono_lib_names, prefix='lib', extension='.a')

            if not mono_lib:
                raise RuntimeError('Could not find mono library in: ' + mono_lib_path)

            env_mono.Append(CPPDEFINES=['_REENTRANT'])

            if mono_static:
                env.Append(LINKFLAGS=['-rdynamic'])

                mono_lib_file = os.path.join(mono_lib_path, 'lib' + mono_lib + '.a')

                if is_apple:
                    env.Append(LINKFLAGS=['-Wl,-force_load,' + mono_lib_file])
                else:
                    assert is_desktop(env['platform']) or is_android or is_javascript
                    env.Append(LINKFLAGS=['-Wl,-whole-archive', mono_lib_file, '-Wl,-no-whole-archive'])

                if is_javascript:
                    env.Append(LIBS=['mono-icall-table', 'mono-native', 'mono-ilgen', 'mono-ee-interp'])

                    wasm_src_dir = os.path.join(mono_root, 'src')
                    if not os.path.isdir(wasm_src_dir):
                        raise RuntimeError('Could not find mono wasm src directory')

                    # Ideally this should be defined only for 'driver.c', but I can't fight scons for another 2 hours
                    env_mono.Append(CPPDEFINES=['CORE_BINDINGS'])

                    env_mono.add_source_files(env.modules_sources, [
                        os.path.join(wasm_src_dir, 'driver.c'),
                        os.path.join(wasm_src_dir, 'zlib-helper.c'),
                        os.path.join(wasm_src_dir, 'corebindings.c')
                    ])

                    env.Append(LINKFLAGS=[
                        '--js-library', os.path.join(wasm_src_dir, 'library_mono.js'),
                        '--js-library', os.path.join(wasm_src_dir, 'binding_support.js'),
                        '--js-library', os.path.join(wasm_src_dir, 'dotnet_support.js')
                    ])
            else:
                env.Append(LIBS=[mono_lib])

            if is_apple:
                env.Append(LIBS=['iconv', 'pthread'])
            elif is_android:
                pass # Nothing
            elif is_javascript:
                env.Append(LIBS=['m', 'rt', 'dl', 'pthread'])
            else:
                env.Append(LIBS=['m', 'rt', 'dl', 'pthread'])

            if not mono_static:
                mono_so_name = find_file_in_dir(mono_lib_path, mono_lib_names, prefix='lib', extension=sharedlib_ext)

                if not mono_so_name:
                    raise RuntimeError('Could not find mono shared library in: ' + mono_lib_path)

                copy_file(mono_lib_path, '#bin', 'lib' + mono_so_name + sharedlib_ext)
        else:
            assert not mono_static

            # TODO: Add option to force using pkg-config
            print('Mono root directory not found. Using pkg-config instead')

            env.ParseConfig('pkg-config monosgen-2 --libs')
            env_mono.ParseConfig('pkg-config monosgen-2 --cflags')

            tmpenv = Environment()
            tmpenv.AppendENVPath('PKG_CONFIG_PATH', os.getenv('PKG_CONFIG_PATH'))
            tmpenv.ParseConfig('pkg-config monosgen-2 --libs-only-L')

            for hint_dir in tmpenv['LIBPATH']:
                name_found = find_file_in_dir(hint_dir, mono_lib_names, prefix='lib', extension=sharedlib_ext)
                if name_found:
                    mono_lib_path = hint_dir
                    mono_so_name = name_found
                    break

            if not mono_so_name:
                raise RuntimeError('Could not find mono shared library in: ' + str(tmpenv['LIBPATH']))

        if not mono_static:
            libs_output_dir = get_android_out_dir(env) if is_android else '#bin'
            copy_file(mono_lib_path, libs_output_dir, 'lib' + mono_so_name + sharedlib_ext)

    if not tools_enabled:
        if is_desktop(env['platform']):
            if not mono_root:
                mono_root = subprocess.check_output(['pkg-config', 'mono-2', '--variable=prefix']).decode('utf8').strip()

            make_template_dir(env, mono_root)
        elif is_android:
            # Compress Android Mono Config
            from . import make_android_mono_config
            module_dir = os.getcwd()
            config_file_path = os.path.join(module_dir, 'build_scripts', 'mono_android_config.xml')
            make_android_mono_config.generate_compressed_config(config_file_path, 'mono_gd/')

            # Copy the required shared libraries
            copy_mono_shared_libs(env, mono_root, None)
        elif is_javascript:
            pass # No data directory for this platform

    if copy_mono_root:
        if not mono_root:
            mono_root = subprocess.check_output(['pkg-config', 'mono-2', '--variable=prefix']).decode('utf8').strip()

        if tools_enabled:
           copy_mono_root_files(env, mono_root)
        else:
            print("Ignoring option: 'copy_mono_root'; only available for builds with 'tools' enabled.")


def make_template_dir(env, mono_root):
    from shutil import rmtree

    platform = env['platform']
    target = env['target']

    template_dir_name = ''

    assert is_desktop(platform)

    template_dir_name = 'data.mono.%s.%s.%s' % (platform, env['bits'], target)

    output_dir = Dir('#bin').abspath
    template_dir = os.path.join(output_dir, template_dir_name)

    template_mono_root_dir = os.path.join(template_dir, 'Mono')

    if os.path.isdir(template_mono_root_dir):
        rmtree(template_mono_root_dir) # Clean first

    # Copy etc/mono/

    template_mono_config_dir = os.path.join(template_mono_root_dir, 'etc', 'mono')
    copy_mono_etc_dir(mono_root, template_mono_config_dir, platform)

    # Copy the required shared libraries

    copy_mono_shared_libs(env, mono_root, template_mono_root_dir)


def copy_mono_root_files(env, mono_root):
    from glob import glob
    from shutil import copy
    from shutil import rmtree

    if not mono_root:
        raise RuntimeError('Mono installation directory not found')

    output_dir = Dir('#bin').abspath
    editor_mono_root_dir = os.path.join(output_dir, 'GodotSharp', 'Mono')

    if os.path.isdir(editor_mono_root_dir):
        rmtree(editor_mono_root_dir) # Clean first

    # Copy etc/mono/

    editor_mono_config_dir = os.path.join(editor_mono_root_dir, 'etc', 'mono')
    copy_mono_etc_dir(mono_root, editor_mono_config_dir, env['platform'])

    # Copy the required shared libraries

    copy_mono_shared_libs(env, mono_root, editor_mono_root_dir)

    # Copy framework assemblies

    mono_framework_dir = os.path.join(mono_root, 'lib', 'mono', '4.5')
    mono_framework_facades_dir = os.path.join(mono_framework_dir, 'Facades')

    editor_mono_framework_dir = os.path.join(editor_mono_root_dir, 'lib', 'mono', '4.5')
    editor_mono_framework_facades_dir = os.path.join(editor_mono_framework_dir, 'Facades')

    if not os.path.isdir(editor_mono_framework_dir):
        os.makedirs(editor_mono_framework_dir)
    if not os.path.isdir(editor_mono_framework_facades_dir):
        os.makedirs(editor_mono_framework_facades_dir)

    for assembly in glob(os.path.join(mono_framework_dir, '*.dll')):
        copy(assembly, editor_mono_framework_dir)
    for assembly in glob(os.path.join(mono_framework_facades_dir, '*.dll')):
        copy(assembly, editor_mono_framework_facades_dir)


def copy_mono_etc_dir(mono_root, target_mono_config_dir, platform):
    from distutils.dir_util import copy_tree
    from glob import glob
    from shutil import copy

    if not os.path.isdir(target_mono_config_dir):
        os.makedirs(target_mono_config_dir)

    mono_etc_dir = os.path.join(mono_root, 'etc', 'mono')
    if not os.path.isdir(mono_etc_dir):
        mono_etc_dir = ''
        etc_hint_dirs = []
        if platform != 'windows':
            etc_hint_dirs += ['/etc/mono', '/usr/local/etc/mono']
        if 'MONO_CFG_DIR' in os.environ:
            etc_hint_dirs += [os.path.join(os.environ['MONO_CFG_DIR'], 'mono')]
        for etc_hint_dir in etc_hint_dirs:
            if os.path.isdir(etc_hint_dir):
                mono_etc_dir = etc_hint_dir
                break
        if not mono_etc_dir:
            raise RuntimeError('Mono installation etc directory not found')

    copy_tree(os.path.join(mono_etc_dir, '2.0'), os.path.join(target_mono_config_dir, '2.0'))
    copy_tree(os.path.join(mono_etc_dir, '4.0'), os.path.join(target_mono_config_dir, '4.0'))
    copy_tree(os.path.join(mono_etc_dir, '4.5'), os.path.join(target_mono_config_dir, '4.5'))
    if os.path.isdir(os.path.join(mono_etc_dir, 'mconfig')):
        copy_tree(os.path.join(mono_etc_dir, 'mconfig'), os.path.join(target_mono_config_dir, 'mconfig'))

    for file in glob(os.path.join(mono_etc_dir, '*')):
        if os.path.isfile(file):
            copy(file, target_mono_config_dir)


def copy_mono_shared_libs(env, mono_root, target_mono_root_dir):
    from shutil import copy

    def copy_if_exists(src, dst):
        if os.path.isfile(src):
            copy(src, dst)

    platform = env['platform']

    if platform == 'windows':
        src_mono_bin_dir = os.path.join(mono_root, 'bin')
        target_mono_bin_dir = os.path.join(target_mono_root_dir, 'bin')

        if not os.path.isdir(target_mono_bin_dir):
            os.makedirs(target_mono_bin_dir)

        mono_posix_helper_name = find_file_in_dir(src_mono_bin_dir, ['MonoPosixHelper', 'libMonoPosixHelper'], extension='.dll')
        copy(os.path.join(src_mono_bin_dir, mono_posix_helper_name + '.dll'), os.path.join(target_mono_bin_dir, 'MonoPosixHelper.dll'))

        # For newer versions
        btls_dll_path = os.path.join(src_mono_bin_dir, 'libmono-btls-shared.dll')
        if os.path.isfile(btls_dll_path):
            copy(btls_dll_path, target_mono_bin_dir)
    else:
        target_mono_lib_dir = get_android_out_dir(env) if platform == 'android' else os.path.join(target_mono_root_dir, 'lib')

        if not os.path.isdir(target_mono_lib_dir):
            os.makedirs(target_mono_lib_dir)

        lib_file_names = []
        if platform == 'osx':
            lib_file_names = [lib_name + '.dylib' for lib_name in [
                'libmono-btls-shared', 'libmono-native-compat', 'libMonoPosixHelper'
            ]]
        elif is_unix_like(platform):
            lib_file_names = [lib_name + '.so' for lib_name in [
                'libmono-btls-shared', 'libmono-ee-interp', 'libmono-native', 'libMonoPosixHelper',
                'libmono-profiler-aot', 'libmono-profiler-coverage', 'libmono-profiler-log', 'libMonoSupportW'
            ]]

        for lib_file_name in lib_file_names:
            copy_if_exists(os.path.join(mono_root, 'lib', lib_file_name), target_mono_lib_dir)

def pkgconfig_try_find_mono_root(mono_lib_names, sharedlib_ext):
    tmpenv = Environment()
    tmpenv.AppendENVPath('PKG_CONFIG_PATH', os.getenv('PKG_CONFIG_PATH'))
    tmpenv.ParseConfig('pkg-config monosgen-2 --libs-only-L')
    for hint_dir in tmpenv['LIBPATH']:
        name_found = find_file_in_dir(hint_dir, mono_lib_names, prefix='lib', extension=sharedlib_ext)
        if name_found and os.path.isdir(os.path.join(hint_dir, '..', 'include', 'mono-2.0')):
            return os.path.join(hint_dir, '..')
    return ''
