#include "PythonAPI.h"

PythonAPI::PythonAPI() {
    py::initialize_interpreter();

    try {
        py::module_ myscript = py::module_::import("main");

        myscript.attr("hello")();

        py::object API = myscript.attr("API");

        instance = API();

        api_thread = std::thread(&PythonAPI::start, this);
        api_thread.detach();

    } catch (py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }

}

PythonAPI::~PythonAPI() {
    py::finalize_interpreter();
    api_thread.join();
}

void PythonAPI::add_i() {
    try {
        instance.attr("add_i")();
    } catch (py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
}

int PythonAPI::print_selected_window() {
    try {
        int sum = instance.attr("print_selected_window")(selectedWindow).cast<int>();
        return sum;
    } catch (py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
    return -1;
}

void PythonAPI::start() {
    int iterations = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        try {
            if (python){
                for (Zone z : zones) {
                    float mean_zone = 0;
                    for (int i = 0; i < z.indexes.size(); i++) {
                        ThresholdingResult stack_zones = signals[z.indexes[i]].get_threshold();
                        mean_zone += stack_zones.spikes;
                    }
                    mean_zone /= z.indexes.size();
                    py::dict py_zone = py::dict();
                    py_zone["name"] = z.name;
                    py_zone["mean"] = mean_zone;

                    py::bytes temp = instance.attr("set_object")(py_zone).cast<py::bytes>();
                    std::string temp_str = temp;
                    msg.buffer[0] = temp_str[0];
                    msg.buffer[1] = temp_str[1];
                    msg.buffer[2] = temp_str[2];
                    std::cout << temp_str << std::endl;
                    msg.isOpen = false;

                }
                std::cout << "iteration: " << iterations++ << std::endl;
            }
        } catch (py::error_already_set& e) {
            std::cerr << "Python error: " << e.what() << std::endl;
        }
    }
}

