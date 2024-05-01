#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>

#define REL_PATH_MAX 16

void make_directory(const char* name) {
#ifdef __linux__
    mkdir(name, 777);
#else
    mkdir(name);
#endif
}

void panic_msg(const char* msg, bool usage) {
    fprintf(stderr, "%s\n", msg);
    if (usage) {
        printf("%s\n", "Usage:\n>setup_year.c [year]");
    }
    exit(0);
}

const char * template = "#include <stdio.h>\n\n"
                        "int main() {\n"
                        "    FILE* input = fopen(\"data/day_%02d.txt\", \"r\");\n\n\n"
                        "    fclose(input);\n"
                        "}";

int main(int argc, char** argv) {
    if (argc != 2) {
        panic_msg("Incorrect number of arguments", true);
    }
    char* year_str = argv[1];
    int year = atoi(year_str);
    if (year < 2015) {
        panic_msg("Invalid year provided", true);
    }
    struct stat sb;
    if (stat(year_str, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        panic_msg("Folder already exists", false);
    }

    make_directory(year_str);
    char path[REL_PATH_MAX];
    snprintf(path, REL_PATH_MAX, "%s/%s", year_str, "data");
    make_directory(path);
    snprintf(path, REL_PATH_MAX, "%s/%s", year_str, "bin");
    make_directory(path);

    for (int i = 1; i < 26; ++i) {
        snprintf(path, REL_PATH_MAX, "%s/day_%02d.c", year_str, i);
        // printf("%s\n", path);
        FILE* c_file = fopen(path, "w");
        fprintf(c_file, template, i);
        // fprintf(c_file, "#include <stdio.h>\n\nint main() {\n    FILE* input = fopen(\"data/day_%02d.txt\", \"r\");\n\n\n    fclose(input);\n}", i);
        fclose(c_file);
    }

}