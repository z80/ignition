"""Functions used to generate source files during build time

All such functions are invoked in a subprocess on Windows to prevent build flakiness.

"""
from platform_methods import subprocess_main
from compat import byte_to_str
from collections import OrderedDict


def make_splash(target, source, env):
    src = source[0]
    dst = target[0]

    with open(src, "rb") as f:
        buf = f.read()

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#ifndef BOOT_SPLASH_H\n")
        g.write("#define BOOT_SPLASH_H\n")
        g.write('static const Color boot_splash_bg_color = Color(0.14, 0.14, 0.14);\n')
        g.write("static const unsigned char boot_splash_png[] = {\n")
        for i in range(len(buf)):
            g.write(byte_to_str(buf[i]) + ",\n")
        g.write("};\n")
        g.write("#endif")


def make_splash_editor(target, source, env):
    src = source[0]
    dst = target[0]

    with open(src, "rb") as f:
        buf = f.read()

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#ifndef BOOT_SPLASH_EDITOR_H\n")
        g.write("#define BOOT_SPLASH_EDITOR_H\n")
        g.write('static const Color boot_splash_editor_bg_color = Color(0.14, 0.14, 0.14);\n')
        g.write("static const unsigned char boot_splash_editor_png[] = {\n")
        for i in range(len(buf)):
            g.write(byte_to_str(buf[i]) + ",\n")
        g.write("};\n")
        g.write("#endif")


def make_app_icon(target, source, env):
    src = source[0]
    dst = target[0]

    with open(src, "rb") as f:
        buf = f.read()

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#ifndef APP_ICON_H\n")
        g.write("#define APP_ICON_H\n")
        g.write("static const unsigned char app_icon_png[] = {\n")
        for i in range(len(buf)):
            g.write(byte_to_str(buf[i]) + ",\n")
        g.write("};\n")
        g.write("#endif")


def make_default_controller_mappings(target, source, env):
    dst = target[0]
    g = open(dst, "w")

    g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
    g.write("#include \"core/typedefs.h\"\n")
    g.write("#include \"main/default_controller_mappings.h\"\n")

    # ensure mappings have a consistent order
    platform_mappings = OrderedDict()
    for src_path in source:
        with open(src_path, "r") as f:
            # read mapping file and skip header
            mapping_file_lines = f.readlines()[2:]

        current_platform = None
        for line in mapping_file_lines:
            if not line:
                continue
            line = line.strip()
            if len(line) == 0:
                continue
            if line[0] == "#":
                current_platform = line[1:].strip()
                if current_platform not in platform_mappings:
                    platform_mappings[current_platform] = {}
            elif current_platform:
                line_parts = line.split(",")
                guid = line_parts[0]
                if guid in platform_mappings[current_platform]:
                    g.write("// WARNING - DATABASE {} OVERWROTE PRIOR MAPPING: {} {}\n".format(src_path, current_platform, platform_mappings[current_platform][guid]))
                valid_mapping = True
                for input_map in line_parts[2:]:
                    if "+" in input_map or "-" in input_map or "~" in input_map:
                        g.write("// WARNING - DISCARDED UNSUPPORTED MAPPING TYPE FROM DATABASE {}: {} {}\n".format(src_path, current_platform, line))
                        valid_mapping = False
                        break
                if valid_mapping:
                    platform_mappings[current_platform][guid] = line

    platform_variables = {
        "Linux": "#if X11_ENABLED",
        "Windows": "#ifdef WINDOWS_ENABLED",
        "Mac OS X": "#ifdef OSX_ENABLED",
        "Android": "#if defined(__ANDROID__)",
        "iOS": "#ifdef IPHONE_ENABLED",
        "Javascript": "#ifdef JAVASCRIPT_ENABLED",
        "UWP": "#ifdef UWP_ENABLED",
    }

    g.write("const char* DefaultControllerMappings::mappings[] = {\n")
    for platform, mappings in platform_mappings.items():
        variable = platform_variables[platform]
        g.write("{}\n".format(variable))
        for mapping in mappings.values():
            g.write("\t\"{}\",\n".format(mapping))
        g.write("#endif\n")

    g.write("\tNULL\n};\n")
    g.close()


if __name__ == '__main__':
    subprocess_main(globals())
