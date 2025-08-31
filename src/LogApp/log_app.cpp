#include <LogApp/file_logger.hpp>
#include <iostream>

using namespace logging;

int main(int argc, char* argv[]) {
    auto usage_str = "Использование: ./log_app <logfile> <default_level: info|warn|error>\n";

    if (argc < 3) {
        std::cerr << usage_str;
        return EXIT_FAILURE;
    }

    std::string_view logfile = argv[1];
    std::string_view level_str = argv[2];

    Level def_level;
    if (level_str == "info") {
        def_level = Level::Info;
    }
    else if (level_str == "warn") {
        def_level = Level::Warning;
    }
    else if(level_str == "error") {
        def_level = Level::Error;
    }
    else {
        std::cerr << usage_str;
        return EXIT_FAILURE;
    }

    FileLogger app(logfile.data(), def_level);
    app.run_console();

    return EXIT_SUCCESS;
}
