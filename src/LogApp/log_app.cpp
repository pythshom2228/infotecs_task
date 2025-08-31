#include <LogApp/log_app.hpp>
#include <Logger/file_sink.hpp>
#include <iostream>

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

    LogApp app(logfile.data(), def_level);
    app.run_console();

    return EXIT_SUCCESS;
}


LogApp::LogApp(const std::string& logfile, Level default_level)
    : logger_(logfile,default_level)
{
    worker_thread_ = std::thread(&LogApp::worker_loop, this);
}

LogApp::~LogApp() {
    queue_.stop();
    if (worker_thread_.joinable())
        worker_thread_.join();
}

void LogApp::run_console() {
    std::string message;
    std::string level_str;

    while (true) {
        std::cout << "Введите сообщение (или 'quit' для выхода): ";
        if (!std::getline(std::cin, message)) break;
        if (message == "quit") break;

        std::cout << "Введите уровень (info|warn|error, Enter = info): ";
        if (!std::getline(std::cin, level_str)) break;
        std::cout << '\n';
        Level lvl = Level::Info;
        if (!level_str.empty()) {
            lvl = parse_level(level_str);
        }

        queue_.push(Task{message, lvl});
    }
    queue_.stop();
}

void LogApp::worker_loop() {
    while (true) {
        std::optional<Task> task_opt = queue_.pop();
        if(!task_opt) break;
        logger_.log(task_opt->message, task_opt->level);
    }
}

Level LogApp::parse_level(const std::string& token) {
    if (token == "error" || token == "2") return Level::Error;
    else if (token == "warn"  || token == "1") return Level::Warning;
    else if (token == "info"  || token == "0") return Level::Info;
    return Level::Info; // если не распознали
}


