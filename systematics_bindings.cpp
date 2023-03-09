#include <tuple>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include "Empirical/include/emp/Evolve/Systematics.hpp"

namespace py = pybind11;


PYBIND11_DECLARE_HOLDER_TYPE(T, emp::Ptr<T>, false);

// Only needed if the type's `.get()` goes by another name
namespace pybind11 { namespace detail {
    template <typename T>
    struct holder_helper<emp::Ptr<T>> { // <-- specialization
        static const T *get(const emp::Ptr<T> &p) { return p.Raw(); }
    };
}}

// namespace emp {
//     namespace datastruct {
//         struct python {
//             // using phen_t = py::object;
//             using has_phen_t = std::false_type;
//             using has_mutations_t = std::false_type;
//             using has_fitness_t = std::false_type;

//             py::object data;

//         };
//     }
// }

using taxon_info_t = std::string;
using org_t = py::object;
using sys_t = emp::Systematics<org_t, taxon_info_t>;
using taxon_t = emp::Taxon<taxon_info_t>;
using taxon_ptr = emp::Ptr<taxon_t>;


PYBIND11_MODULE(systematics, m) {
    // py::class_<emp::datastruct::python>(m, "DataStruct")
    //     .def("set_data", [](emp::datastruct::python & self, py::object & d){self.data = d;})
    //     .def("get_data", [](emp::datastruct::python & self, py::object & d){return self.data;}, py::return_value_policy::reference_internal)
    //     .def_readwrite("data", &emp::datastruct::python::data)
    //     ;

    py::class_<emp::WorldPosition>(m, "WorldPosition")
        .def(py::init<size_t, size_t>())
        // .def(py::init<std::tuple<size_t, size_t>>())
        .def("get_index", &emp::WorldPosition::GetIndex, R"mydelimiter(
            Returns the index (position within the population) represented by this WorldPosition as an int 
            )mydelimiter")
        .def("get_pop_ID", &emp::WorldPosition::GetPopID, R"mydelimiter(
            Returns the ID (as an int) of the population that this WorldPosition is referring to.

            Wondering why there might be multiple populations? It's because WorldPosition objects 
            are designed to gracefully handle configurations in which there are
            multiple separate populations (e.g. due to islands or separated generations).
            If you're just using one population, the pop ID will always be 0 and that's fine.
            )mydelimiter")
        .def("is_active", &emp::WorldPosition::IsActive, R"mydelimiter(
            Returns a boolean indicating whether this position potentially represents an "active" (i.e. "alive")
            organism in the context of a generational/synchronous configuration (i.e. one in which a new generation
            is created on each time step and there is no overlap of organisms between generations; this 
            configuration is commonly used in evolutionary computation and simple evolutionary models). In this case, 
            the next generation (i.e. the one that is currently in the process of being created) is considered 
            "inactive". Conventionally, the population that selection is currently happening on has ID 0 and the population
            holding the new generation has ID 1. Thus, this method simply returns true if the population ID is 0 and false 
            otherwise.

            If you are using population IDs for a separate purpose (e.g. islands), this method will not yield accurate
            results.
            )mydelimiter")
        .def("is_valid", &emp::WorldPosition::IsValid, R"mydelimiter(
            -1 can be used as a sentinel value to indicate that a position is not to be used.
            This method returns a boolean indicating whether this position is "valid" 
            (i.e. the index is not equal to -1).
            )mydelimiter");

    // py::implicitly_convertible<std::tuple<int, int>, emp::WorldPosition>();

    // The py::nodelete here might cause memory leaks if someone tries to construct
    // a taxon without putting it in a systematics manager, but it seems necessary to
    // avoid a segfault on destruction of the systematics manager. There's also
    // not really any reason to ever make a taxon that you don't put in a systematics
    // manager
    py::class_<taxon_t, std::unique_ptr<taxon_t, py::nodelete> >(m, "Taxon")
        .def(py::init<size_t, taxon_info_t>())
        .def(py::init<size_t, taxon_info_t, taxon_t*>())
        .def("get_parent", &taxon_t::GetParent)
        .def("get_id", &taxon_t::GetID)        
        .def("get_total_offspring", &taxon_t::GetTotalOffspring)
        .def("get_total_orgs", &taxon_t::GetTotOrgs)        
        .def("get_num_offspring", &taxon_t::GetNumOff)
        .def("get_num_orgs", &taxon_t::GetNumOrgs)
        .def("get_offspring", &taxon_t::GetOffspring)
        .def("get_destruction_time", &taxon_t::GetDestructionTime)
        .def("get_origination_time", &taxon_t::GetOriginationTime)
        // .def("set_data", &taxon_t::SetData)
        // .def("get_data", [](taxon_t & self){return self.GetData();})
        ;

    py::class_<sys_t>(m, "Systematics")
        .def(py::init<std::function<taxon_info_t(org_t &)>, bool, bool, bool, bool>(), py::arg("calc_taxon"), py::arg("store_active") = true, py::arg("store_ancestors") = true, py::arg("store_all") = false, py::arg("store_pos") = true)

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
        .def("get_num_active", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumActive))
        .def("get_num_ancestors", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumAncestors))
        .def("get_num_outside", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumOutside))
        .def("get_tree_size", static_cast<size_t (sys_t::*) () const>(&sys_t::GetTreeSize))
        .def("get_num_taxa", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumTaxa))
        .def("get_max_depth", static_cast<int (sys_t::*) () >(&sys_t::GetMaxDepth))
        .def("get_num_roots", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumRoots))
        .def("get_next_id", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNextID))
        .def("get_ave_depth", static_cast<double (sys_t::*) () const>(&sys_t::GetAveDepth))
        .def("get_active_taxa_reference", static_cast<std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > * (sys_t::*) ()>(&sys_t::GetActivePtr), py::return_value_policy::reference_internal)
        .def("get_active_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetActive), py::return_value_policy::reference_internal)
        .def("get_ancestor_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetAncestors), py::return_value_policy::reference_internal)
        .def("get_outside_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetOutside), py::return_value_policy::reference_internal)
        .def("get_next_parent", static_cast<emp::Ptr<taxon_t> (sys_t::*) ()>(&sys_t::GetNextParent), py::return_value_policy::reference_internal)
        .def("get_most_recent", static_cast<emp::Ptr<taxon_t> (sys_t::*) ()>(&sys_t::GetMostRecent), py::return_value_policy::reference_internal)
        .def("parent", [](sys_t & self, taxon_t * tax){return self.Parent(tax);}, py::return_value_policy::reference_internal)
        .def("is_taxon_at", static_cast<bool (sys_t::*) (emp::WorldPosition)>(&sys_t::IsTaxonAt))
        .def("get_taxon_at", static_cast<emp::Ptr<taxon_t> (sys_t::*) (emp::WorldPosition)>(&sys_t::GetTaxonAt))
        .def("get_mrca", static_cast<emp::Ptr<taxon_t> (sys_t::*) () const>(&sys_t::GetMRCA), py::return_value_policy::reference_internal)
        .def("get_shared_ancestor", [](sys_t & self, taxon_t * t1, taxon_t * t2){return self.GetSharedAncestor(t1, t2);}, py::return_value_policy::reference_internal)
        
        // Birth notification
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org", [](sys_t & self, org_t & org){return self.AddOrg(org, nullptr);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        .def("add_org", [](sys_t & self, org_t & org, taxon_t * parent){return self.AddOrg(org, parent);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)

        // Death notification
        .def("remove_org_by_position", static_cast<bool (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrg), R"mydelimiter(
            Notify the systematics manager that an organism has died. Use this method if you're having the systematics
            manager keep track of each organism's position and want to use the position to tell the systematics manager which
            organism died. Otherwise use remove_org.

            Parameters
            ----------
            WorldPosition position: The location of the organism that died. 
            )mydelimiter")
        .def("remove_org", [](sys_t & self, taxon_t * tax){return self.RemoveOrg(tax);}, R"mydelimiter(
            Notify the systematics manager that an organism has died. Use this method if you are keeping track of
            taxon objects yourself (rather than having the systematics manager handle it by tracking position).

            Parameters
            ----------
            Taxon tax: The taxon of the organism that died. 
            )mydelimiter")
        .def("remove_org_by_position_after_repro", static_cast<void (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrgAfterRepro), R"mydelimiter(
            Notify the systematics manager that an organism has died but that it shouldn't record the death until the next reproduction event. 
            You might want to do this if there's a chance that the organism simultaneously died and reproduced (e.g. if the organism's
            offspring might have replaced it in the population). 
            
            Calling remove_org_by_position in that scenario could result in a segmentation fault if it was the last organism of its taxon. It could
            either be incorrectly marked extinct (if the offspring is part of the same taxon) and/or incorrectly pruned (if the offspring is part of a new
            taxon).

            Use this method if you're having the systematics manager keep track of each organism's position and want to use the position to 
            tell the systematics manager which organism died. Otherwise use remove_org_after_repro.

            Parameters
            ----------
            WorldPosition position: The location of the organism that died. 
            )mydelimiter")
        .def("remove_org_after_repro", [](sys_t & self, taxon_t * tax){self.RemoveOrgAfterRepro(tax);}, R"mydelimiter(
            Notify the systematics manager that an organism has died but that it shouldn't record the death until the next reproduction event. 
            You might want to do this if there's a chance that the organism simultaneously died and reproduced (e.g. if the organism's
            offspring might have replaced it in the population). 
            
            Calling remove_org_by_position in that scenario could result in a segmentation fault if it was the last organism of its taxon. It could
            either be incorrectly marked extinct (if the offspring is part of the same taxon) and/or incorrectly pruned (if the offspring is part of a new
            taxon).

            Use this method if you are keeping track of taxon objects yourself (rather than having the systematics manager handle it by tracking position).

            Parameters
            ----------
            Taxon tax: The taxon of the organism that died. 
            )mydelimiter")
        .def("set_next_parent", [](sys_t & self, taxon_t * tax){self.SetNextParent(tax);}, R"mydelimiter(
            Sometimes, due to the flow of your program, you may not have access to the taxon object for the parent and
            the offspring at the same time. In these cases, you can use set_next_parent to tell the systematics manager
            what the taxon of the parent of the next offspring should be. The next time you call one of the add_org 
            methods without a specified parent, the systematics manager will used the specified taxon as the parent for that organism.

            Parameters
            ----------
            Taxon tax: The taxon to set as the next parent. 
            )mydelimiter")
        .def("set_next_parent_by_position", static_cast<void (sys_t::*) (emp::WorldPosition)>(&sys_t::SetNextParent), R"mydelimiter(
            Works just like set_next_parent except for systematics managers that are tracking based on position.

            Parameters
            ----------
            WorldPosition pos: The position in the world of the organism that should be the next parent.
            )mydelimiter")

        // Move notification
        .def("swap_positions", static_cast<void (sys_t::*) (emp::WorldPosition, emp::WorldPosition)>(&sys_t::SwapPositions), R"mydelimiter(
            If you are tracking taxa by positions it is important that you notify the systematics manager any time a taxon changes
            position for any reason. This function allows you to do so. Assumes that all changes in position take the form of the
            contents of one location being replaced with the contents of a different location. If only one organism is moving, the location
            it is moving to is presumably empty and it is presumably leaving its former position empty. Thus, you will swap an organism with
            an empty space.

            Parameters
            ----------
            WorldPosition pos1: The position of one of the organisms being swapped. 
            WorldPosition pos2: The position of one of the organisms being swapped. 
            )mydelimiter")

        // Signals
        .def("on_new", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t, org_t & org)> & fun){self.OnNew(fun);})
        .def("on_extinct", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t)> & fun){self.OnExtinct(fun);})
        .def("on_prune", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t)> & fun){self.OnPrune(fun);})
        
        // Phylostatistics
        .def("calc_diversity", static_cast<double (sys_t::*) () const>(&sys_t::CalcDiversity))
        .def("mrca_depth", static_cast<int (sys_t::*) () const>(&sys_t::GetMRCADepth))
        .def("colless_like_index", static_cast<double (sys_t::*) () const>(&sys_t::CollessLikeIndex))
        .def("sackin_index", static_cast<int (sys_t::*) () const>(&sys_t::SackinIndex))
        .def("get_branches_to_root", [](sys_t & self, taxon_t * tax){return self.GetBranchesToRoot(tax);})
        .def("get_distance_to_root", [](sys_t & self, taxon_t * tax){return self.GetDistanceToRoot(tax);})
        .def("get_variance_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetVariancePairwiseDistance))
        .def("get_mean_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetMeanPairwiseDistance))
        .def("get_sum_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetSumPairwiseDistance))
        .def("get_phylogenetic_diversity", static_cast<int (sys_t::*) () const>(&sys_t::GetPhylogeneticDiversity))
        .def("get_average_origin_time", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetAverageOriginTime), py::arg("normalize") = false)
        .def("get_out_degree_distribution", static_cast<std::unordered_map<int, int> (sys_t::*) () const>(&sys_t::GetOutDegreeDistribution))
        .def("get_mean_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetMeanEvolutionaryDistinctiveness))
        .def("get_sum_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetSumEvolutionaryDistinctiveness))
        .def("get_variance_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetVarianceEvolutionaryDistinctiveness))

        // Input
        .def("load_from_file", static_cast<void (sys_t::*) (const std::string &, const std::string &, bool, bool)>(&sys_t::LoadFromFile), py::arg("file_path"), py::arg("info_col") = "info", py::arg("assume_leaves_extant") = true, py::arg("adjust_total_offspring") = true)

        // Output
        .def("snapshot", static_cast<void (sys_t::*) (const std::string &) const>(&sys_t::Snapshot))
        .def("add_snapshot_fun", static_cast<void (sys_t::*) (const std::function<std::string(const taxon_t &)> &, const std::string &, const std::string &) >(&sys_t::AddSnapshotFun))
        .def("print_status", [](sys_t & self){self.PrintStatus();})
        .def("print_lineage", [](sys_t & self, taxon_t * tax){self.PrintLineage(tax);})
        
        // Time tracking
        .def("update", static_cast<void (sys_t::*) ()>(&sys_t::Update))

        // Efficiency functions        
        .def("remove_before", static_cast<void (sys_t::*) (int)>(&sys_t::RemoveBefore))
        ;
}



