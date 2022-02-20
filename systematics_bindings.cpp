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

    // The py::nodelete here might cause memory leaks if someone tries to construct
    // a taxon without putting it in a systematics manager, but it seems necessary to
    // avoid a segfault on destruction of the systematics manager. There's also
    // not really any reason to ever make a taxon that you don't put in a systematics
    // manager
    py::class_<taxon_t, std::unique_ptr<taxon_t, py::nodelete> >(m, "Taxon")
        .def(py::init<size_t, taxon_info_t>())
        .def(py::init<size_t, taxon_info_t, taxon_t*>())
        .def("get_parent", &taxon_t::GetParent);

    py::class_<sys_t>(m, "Systematics")
        .def(py::init<std::function<taxon_info_t(org_t &)>, bool, bool, bool, bool>(), py::arg("calc_taxon"), py::arg("store_active") = true, py::arg("store_ancestors") = true, py::arg("store_all") = false, py::arg("store_pos") = true)
        // .def("test", [](const sys_t & s, int a, std::optional<int *> b){return a + *b;}, "A function which adds two numbers", py::arg("a"), py::arg("b") = py::none())

        // Setting systematics manager state
        .def("set_calc_info_fun", static_cast<void (sys_t::*) (std::function<taxon_info_t(org_t &)>)>(&sys_t::SetCalcInfoFun))        
        .def("set_store_active", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreActive))        
        .def("set_store_ancestors", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreAncestors))
        .def("set_store_outside", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreOutside))
        .def("set_store_archive", static_cast<void (sys_t::*) (bool)>(&sys_t::SetArchive))
        .def("set_store_position", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStorePosition))
        .def("set_track_synchronous", static_cast<void (sys_t::*) (bool)>(&sys_t::SetTrackSynchronous))
        .def("set_update", static_cast<void (sys_t::*) (size_t)>(&sys_t::SetUpdate))

        // Getting systematics manager state
        .def("get_store_active", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreActive))        
        .def("get_store_ancestors", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreAncestors))
        .def("get_store_outside", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreOutside))
        .def("get_store_archive", static_cast<bool (sys_t::*) () const>(&sys_t::GetArchive))
        .def("get_store_position", static_cast<bool (sys_t::*) () const>(&sys_t::GetStorePosition))
        .def("get_track_synchronous", static_cast<bool (sys_t::*) () const>(&sys_t::GetTrackSynchronous))
        .def("get_update", static_cast<size_t (sys_t::*) () const>(&sys_t::GetUpdate))
        .def("get_total_orgs", static_cast<size_t (sys_t::*) () const>(&sys_t::GetTotalOrgs))
        .def("get_num_roots", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumRoots))
        .def("get_next_id", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNextID))
        .def("get_ave_depth", static_cast<double (sys_t::*) () const>(&sys_t::GetAveDepth))

        // Birth notification
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org", [](sys_t & self, org_t & org){return self.AddOrg(org, nullptr);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        .def("add_org", [](sys_t & self, org_t & org, taxon_t * parent){return self.AddOrg(org, parent);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)

        // Death notification
        .def("remove_org_by_position", static_cast<bool (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrg))
        .def("remove_org", [](sys_t & self, taxon_t * tax){return self.RemoveOrg(tax);})
        .def("remove_org_by_position_after_repro", static_cast<void (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrgAfterRepro))
        .def("remove_org_after_repro", [](sys_t & self, taxon_t * tax){self.RemoveOrgAfterRepro(tax);})
        .def("set_next_parent_by_position", static_cast<void (sys_t::*) (emp::WorldPosition)>(&sys_t::SetNextParent))
        .def("set_next_parent", [](sys_t & self, taxon_t * tax){self.SetNextParent(tax);})



        ;
}



