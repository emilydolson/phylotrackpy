#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "Empirical/include/emp/Evolve/Systematics.hpp"

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    py::class_<emp::Systematics<std::string, std::string> >(m, "Systematics")
        .def(py::init<std::function<std::string(std::string &)> >());
}



