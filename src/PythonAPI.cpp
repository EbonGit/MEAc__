#include "PythonAPI.h"

PythonAPI::PythonAPI() {
    py::initialize_interpreter();

    try {
        py::module_ myscript = py::module_::import("main");

        myscript.attr("hello")();

        py::object API = myscript.attr("API");

        instance = API();


    } catch (py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }

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
}
