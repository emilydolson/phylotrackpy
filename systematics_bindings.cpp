#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include "Empirical/include/emp/Evolve/Systematics.hpp"

namespace py = pybind11;

using taxon_info_t = py::object;
using org_t = py::object;
using sys_t = emp::Systematics<org_t, taxon_info_t>;
using taxon_t = emp::Taxon<taxon_info_t>;
using taxon_ptr = emp::Ptr<taxon_t>;



PYBIND11_DECLARE_HOLDER_TYPE(T, emp::Ptr<T>, false);

// Only needed if the type's `.get()` goes by another name
namespace pybind11 { namespace detail {
    template <typename T>
    struct holder_helper<emp::Ptr<T>> { // <-- specialization
        static const T *get(const emp::Ptr<T> &p) { return p.Raw(); }
    };
}}

PYBIND11_MODULE(systematics, m) {
    py::class_<emp::WorldPosition>(m, "WorldPosition")
        .def(py::init<size_t, size_t>())
        .def("getIndex", &emp::WorldPosition::GetIndex)
        .def("getPopID", &emp::WorldPosition::GetPopID)
        .def("isActive", &emp::WorldPosition::IsActive)
        .def("isValid", &emp::WorldPosition::IsValid);

    py::class_<taxon_t, std::unique_ptr<taxon_t, py::nodelete> >(m, "Taxon")
        .def(py::init<size_t, taxon_info_t>())
        .def(py::init<size_t, taxon_info_t, taxon_t*>())
        .def("getParent", &taxon_t::GetParent);

    py::class_<sys_t>(m, "Systematics")
        .def(py::init<std::function<taxon_info_t(org_t &)>, bool, bool, bool, bool>(), py::arg("calc_taxon"), py::arg("store_active") = true, py::arg("store_ancestors") = true, py::arg("store_all") = false, py::arg("store_pos") = true)
        // .def("test", [](const sys_t & s, int a, std::optional<int *> b){return a + *b;}, "A function which adds two numbers", py::arg("a"), py::arg("b") = py::none())
        .def("addOrgByPosition", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("addOrgByPosition", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        // .def("addOrg", static_cast<taxon_ptr (sys_t::*) (org_t &, emp::WorldPosition, taxon_t *)>(&sys_t::AddOrg), "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        // .def("addOrg", static_cast<taxon_ptr (sys_t::*) (py::handle &, taxon_ptr)>(&sys_t::AddOrg), "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        .def("addOrg", [](sys_t & self, org_t & org){return self.AddOrg(org, nullptr);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        .def("addOrg", [](sys_t & self, org_t & org, taxon_t * parent){return self.AddOrg(org, parent);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        ;
}



