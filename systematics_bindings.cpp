#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "Empirical/include/emp/Evolve/Systematics.hpp"

namespace py = pybind11;

using sys_t = emp::Systematics<py::object, py::object>;

PYBIND11_MODULE(systematics, m) {
    py::class_<emp::WorldPosition>(m, "WorldPosition")
        .def(py::init<size_t, size_t>())
        .def("getIndex", &emp::WorldPosition::GetIndex)
        .def("getPopID", &emp::WorldPosition::GetPopID)
        .def("isActive", &emp::WorldPosition::IsActive)
        .def("isValid", &emp::WorldPosition::IsValid);
    py::class_<sys_t>(m, "Systematics")
        .def(py::init<std::function<py::object(py::object &)> >())
        .def("addOrg", static_cast<void (sys_t::*) (py::object &, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager");
}



