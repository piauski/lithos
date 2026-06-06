#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "vendor/nob.h"

#define EXECUTABLE_NAME "Lithos"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define VENDOR_FOLDER "vendor/"

#define RAYLIB_DIR VENDOR_FOLDER"raylib-6.0_linux_amd64/"

#define CLANGD_FORMAT "compile_flags.txt"

void cmd_append_include(Cmd *cmd, String_Builder *sb)
{
    cmd_append(cmd, "-I"VENDOR_FOLDER);
    sb_append_cstr(sb, "-I"VENDOR_FOLDER);
    sb_append_cstr(sb, "\n");

    cmd_append(cmd, "-I"RAYLIB_DIR"include");
    sb_append_cstr(sb, "-I"RAYLIB_DIR"include");
    sb_append_cstr(sb, "\n");
}

void cmd_append_lib(Cmd *cmd)
{
    cmd_append(cmd, "-L"RAYLIB_DIR"lib");
    cmd_append(cmd, "-l:libraylib.a");
    cmd_append(cmd, "-lm");
    cmd_append(cmd, "-lX11");
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    String_Builder clangd_include_sb = {0};
    sb_append_cstr(&clangd_include_sb,
"CompileFlags:\n"
"  Add:\n"
);
    mkdir_if_not_exists(BUILD_FOLDER);
    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "-Wall", "-Wextra");
    cmd_append(&cmd, "-o", BUILD_FOLDER EXECUTABLE_NAME);
    cmd_append(&cmd, SRC_FOLDER"one.c");
    cmd_append_include(&cmd, &clangd_include_sb);
    cmd_append_lib(&cmd);

    delete_file(CLANGD_FORMAT);
    write_entire_file(CLANGD_FORMAT, clangd_include_sb.items, clangd_include_sb.count);
    nob_log(INFO, "generated %s", CLANGD_FORMAT);
    if (!nob_cmd_run(&cmd)) return 1;

    if (argc > 1) {
        const char *arg = shift(argv, argc);
        if (strcmp(arg, "-run") != 0) {
            cmd_append(&cmd, BUILD_FOLDER EXECUTABLE_NAME);
            if (!nob_cmd_run(&cmd)) return 1;
        }
    }

    return 0;


}
