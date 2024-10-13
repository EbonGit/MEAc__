#ifndef MEAC___PYTHONAPI_H
#define MEAC___PYTHONAPI_H

#include "Data.h"
#include "pybind11/embed.h"

namespace py = pybind11;

class PythonAPI: public virtual Data{
private :
    py::object instance;
public :
    PythonAPI();
    void add_i();
    int print_selected_window();
};


#endif //MEAC___PYTHONAPI_H