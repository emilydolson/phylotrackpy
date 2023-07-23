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

            Wondering why there might be multiple populations? It's because WorldPosition objects are designed to gracefully handle configurations in which there are multiple separate populations (e.g. due to islands or separated generations).
            If you're just using one population, the pop ID will always be 0 and that's fine.
            )mydelimiter")
        .def("is_active", &emp::WorldPosition::IsActive, R"mydelimiter(
            Returns a boolean indicating whether this position potentially represents an "active" (i.e. "alive") organism in the context of a generational/synchronous configuration (i.e. one in which a new generation is created on each time step and there is no overlap of organisms between generations; this configuration is commonly used in evolutionary computation and simple evolutionary models). In this case, the next generation (i.e. the one that is currently in the process of being created) is considered "inactive". Conventionally, the population that selection is currently happening on has ID 0 and the population holding the new generation has ID 1. Thus, this method simply returns true if the population ID is 0 and false otherwise.

            If you are using population IDs for a separate purpose (e.g. islands), this method will not yield accurate results.
            )mydelimiter")
        .def("is_valid", &emp::WorldPosition::IsValid, R"mydelimiter(
            -1 can be used as a sentinel value to indicate that a position is not to be used.
            This method returns a boolean indicating whether this position is "valid" (i.e. the index is not equal to -1).
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
        .def("__copy__",  [](const taxon_t &self) -> const taxon_t & {
            return self;
        }, py::return_value_policy::reference_internal)
        .def("__deepcopy__", [](const taxon_t &self, py::dict) -> const taxon_t & {
            return self;
        }, py::return_value_policy::reference_internal)
        .def("get_parent", &taxon_t::GetParent)
        .def("get_info", &taxon_t::GetInfo)
        .def("get_id", &taxon_t::GetID, R"mydelimiter(
            Returns the ID (as an int) that uniquely identifies this taxon.
            IDs are assigned sequentially, sp higher IDs will correspond to more recent taxa.
        )mydelimiter")
        .def("get_total_offspring", &taxon_t::GetTotalOffspring, R"mydelimiter(
            Returns the number of direct and indirect descendants of this taxon.
        )mydelimiter")
        .def("get_total_orgs", &taxon_t::GetTotOrgs, R"mydelimiter(
            Returns the total number of organisms that have ever lived corresponding to this taxon.
        )mydelimiter")
        .def("get_num_offspring", &taxon_t::GetNumOff, R"mydelimiter(
            Returns the number of new taxa that were produced by organisms corresponding to this taxon.
        )mydelimiter")
        .def("get_num_orgs", &taxon_t::GetNumOrgs, R"mydelimiter(
            Returns the total number of *currently living* organisms corresponding to this taxon.
        )mydelimiter")
        .def("get_offspring", &taxon_t::GetOffspring, R"mydelimiter(
            Returns a set holding all immediate offspring taxa.
            This method can be used to traverse the phylogenetic tree.
        )mydelimiter")
        .def("get_destruction_time", &taxon_t::GetDestructionTime, R"mydelimiter(
            Return the time (as a float) when this taxon left the population.
            If the taxon is currently alive, this will be equal to float(+inf).
            This call will only succeed when the systematics manager has been set to track times via `set_update()`.
        )mydelimiter")
        .def("get_origination_time", &taxon_t::GetOriginationTime, R"mydelimiter(
            Return the time (as a float) when this taxon first appeared in the population.
            This call will only succeed when the systematics manager has been set to track times via `set_update()`.
        )mydelimiter")
        // .def("set_data", &taxon_t::SetData)
        // .def("get_data", [](taxon_t & self){return self.GetData();})
        ;

    py::class_<sys_t>(m, "Systematics")
        .def(py::init<std::function<taxon_info_t(org_t &)>, bool, bool, bool, bool>(), py::arg("calc_taxon"), py::arg("store_active") = true, py::arg("store_ancestors") = true, py::arg("store_all") = false, py::arg("store_pos") = true)

        // Setting systematics manager state
        .def("set_calc_info_fun", static_cast<void (sys_t::*) (std::function<taxon_info_t(org_t &)>)>(&sys_t::SetCalcInfoFun), R"mydelimiter(
            Set the function used to calculate the information associated with an organism.
            This information is used to categorize organisms within the systematics manager.
            Possible information includes genotype, phenotype, genome sequence, etc.
            For more information on flexible taxon definitions, please see https://phylotrackpy.readthedocs.io/en/latest/index.html#flexible-taxon-definitions.

            Parameters
            ----------
            Callable[org_t, taxon_info_t] func A function that takes in an organism and returns its associated information.
        )mydelimiter")
        .def("set_store_active", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreActive), R"mydelimiter(
            A setter method to configure whether to store all taxa that are active in the population.
            Warning: this is an advanced feature --- setting this to False breaks a lot of assumptions and will most likely disrupt normal functioning of the systematics manager. There is probably not a real reason to change this setting.
            This option defaults to True.

            Parameters
            ----------
            bool val Value representing whether to store all taxa that are active in the population.
        )mydelimiter")
        .def("set_store_ancestors", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreAncestors), R"mydelimiter(
            A setter method to configure whether to store all taxa that are the ancestors of living organisms in the population.
            Warning: this is an advanced feature --- setting this to False breaks a lot of assumptions and will most likely disrupt normal functioning of the systematics manager. As opposed to `set_store_active()`, there are probably some situations where this setting could be useful.
            This option defaults to True.

            Parameters
            ----------
            bool val Value representing whether to store all taxa that are the ancestors of living organisms in the population.
        )mydelimiter")
        .def("set_store_outside", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStoreOutside), R"mydelimiter(
            A setter method to configure whether to store all dead taxa whose descendats have also died.
            Warning: this is an advanced feature --- this setting will use a lot of memory in most cases.
            This option defaults to False.

            Parameters
            ----------
            bool val Value representing whether to store all dead taxa whose descendats have also died.
        )mydelimiter")
        .def("set_store_archive", static_cast<void (sys_t::*) (bool)>(&sys_t::SetArchive), R"mydelimiter(
            A setter method to configure whether to store taxa types that have become extinct.
            Warning: this is an advanced feature --- archive is handled internally. Overriding this setting could disrupt normal behavior of the systematics manager.
            This option defaults to True.

            Parameters
            ----------
            bool val Value representing whether to store taxa types that have died out.
        )mydelimiter")
        .def("set_store_position", static_cast<void (sys_t::*) (bool)>(&sys_t::SetStorePosition), R"mydelimiter(
            A setter method to configure whether to store the position of each taxa.
            This option defaults to True.

            Parameters
            ----------
            bool val Value representing whether to store the position of each taxa.
        )mydelimiter")
        .def("set_track_synchronous", static_cast<void (sys_t::*) (bool)>(&sys_t::SetTrackSynchronous), R"mydelimiter(
            A setter method to configure whether a synchronous population is being tracked. A synchronous population is one where parents must be extanct for their offspring to be born. As such, there are effectively two populations: a currently-active one, and one being created. This is in opposition to steady-state populations, where organisms might die as their offspring are born (for example, in a world where offspring replaces the parent).
            The accuracy of the systematics tracking relies on this option, so it is imperative it be configured properly.
            This option defaults to False.

            Parameters
            ----------
            bool val Value representing whether a synchronous population is being tracked.
        )mydelimiter")
        .def("set_update", static_cast<void (sys_t::*) (size_t)>(&sys_t::SetUpdate), R"mydelimiter(
            A setter method to modify the current time step. This should be used if you want PhylotrackPy to track when events occur.

            Parameters
            ----------
            int update An integer value representing the current time step.
        )mydelimiter")

        // Getting systematics manager state
        .def("get_store_active", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreActive), R"mydelimiter(
            Whether the Systematics Manager is configured to store all alive taxa in the population.
            Can be set using the `set_store_active()` method.
        )mydelimiter")
        .def("get_store_ancestors", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreAncestors), R"mydelimiter(
            Whether the Systematics Manager is configured to store all taxa that are ancestors of living organisms in the population.
            Can be set using the `set_store_ancestors()` method.
        )mydelimiter")
        .def("get_store_outside", static_cast<bool (sys_t::*) () const>(&sys_t::GetStoreOutside), R"mydelimiter(
            Whether the Systematics Manager is configured to store all taxa that are ancestors of living organisms in the population.
            Can be set using the `set_store_ancestors()` method.
        )mydelimiter")
        .def("get_store_archive", static_cast<bool (sys_t::*) () const>(&sys_t::GetArchive), R"mydelimiter(
            Whether the Systematics Manager is configured to store extinct taxa types. Note that this is just `get_store_ancestors() || get_store_outside()`.
            Can be set using the `set_store_archive()` method.
        )mydelimiter")
        .def("get_store_position", static_cast<bool (sys_t::*) () const>(&sys_t::GetStorePosition), R"mydelimiter(
            Whether the Systematics Manager is configured to store taxa positions.
            Can be set using the `set_store_position()` method.
        )mydelimiter")
        .def("get_track_synchronous", static_cast<bool (sys_t::*) () const>(&sys_t::GetTrackSynchronous), R"mydelimiter(
            Whether the Systematics Manager is configured to track a synchronous population.
            The accuracy of the tracking depends on this option being correctly set.
            It is recommended to verify this setting before any tracking.
            Can be set using the `set_track_synchronous()` method.
        )mydelimiter")
        .def("get_update", static_cast<size_t (sys_t::*) () const>(&sys_t::GetUpdate), R"mydelimiter(
            Returns the current timestep of the simulation.
            This time step can be overriden using the `set_update()` method.
        )mydelimiter")
        .def("get_total_orgs", static_cast<size_t (sys_t::*) () const>(&sys_t::GetTotalOrgs), R"mydelimiter(
            Returns the number of living organisms currently present in the population.
        )mydelimiter")
        .def("get_num_active", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumActive), R"mydelimiter(
            Returns the number of active taxa in the population.
        )mydelimiter")
        .def("get_num_ancestors", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumAncestors), R"mydelimiter(
            Returns the number of *extinct* taxa that are ancestors of living organisms currently present in the population.
        )mydelimiter")
        .def("get_num_outside", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumOutside), R"mydelimiter(
            Returns the number of *extinct* taxa that have no living descendants.
            By definition, these are the taxa that are not in the current phylogenetic tree.
        )mydelimiter")
        .def("get_tree_size", static_cast<size_t (sys_t::*) () const>(&sys_t::GetTreeSize), R"mydelimiter(
            Returns the number of taxa in the current phylogenetic tree.
        )mydelimiter")
        .def("get_num_taxa", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumTaxa), R"mydelimiter(
            Returns the number of total taxa.
            Empirically, this is equal to the number of taxa in the current phylogenetic tree, plus the number of extinct taxa with no living descendants.
        )mydelimiter")
        .def("get_max_depth", static_cast<int (sys_t::*) () >(&sys_t::GetMaxDepth), R"mydelimiter(
            Returns the lineage length (phylogenetic depth) from time zero to the active taxon with the longest lineage.
        )mydelimiter")
        .def("get_num_roots", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNumRoots), R"mydelimiter(
            Returns the number of independent phylogenies currently being tracked.
        )mydelimiter")
        .def("get_next_id", static_cast<size_t (sys_t::*) () const>(&sys_t::GetNextID), R"mydelimiter(
            Returns the ID of the next taxon that will be created.
        )mydelimiter")
        .def("get_ave_depth", static_cast<double (sys_t::*) () const>(&sys_t::GetAveDepth), R"mydelimiter(
            Returns the average phylogenetic depth of all organisms currently present in the population.
        )mydelimiter")
        .def("get_active_taxa_reference", static_cast<std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > * (sys_t::*) ()>(&sys_t::GetActivePtr), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a reference to the set of extant taxa.
        )mydelimiter")
        .def("get_active_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetActive), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a reference to the set of extant taxa.
        )mydelimiter")
        .def("get_ancestor_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetAncestors), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a reference to the set of ancestor taxa.
            These are extinct taxa with extant descendants.
        )mydelimiter")
        .def("get_outside_taxa", static_cast<const std::unordered_set< emp::Ptr<taxon_t>, emp::Ptr<taxon_t>::hash_t > & (sys_t::*) () const>(&sys_t::GetOutside), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a reference to the set of outside taxa.
            These are extinct taxa with extinct descendants.
        )mydelimiter")
        .def("get_next_parent", static_cast<emp::Ptr<taxon_t> (sys_t::*) ()>(&sys_t::GetNextParent), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns the taxon that corresponds to the parent of the next taxon. This will only be set if parents are being specified through calls to `set_next_parent()`.
        )mydelimiter")
        .def("get_most_recent", static_cast<emp::Ptr<taxon_t> (sys_t::*) ()>(&sys_t::GetMostRecent), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns the most recently-created taxon.
        )mydelimiter")
        .def("parent", [](sys_t & self, taxon_t * tax){return self.Parent(tax);}, py::return_value_policy::reference_internal, R"mydelimiter(
            Returns the parent of a given taxon, if any.

            Parameters
            ----------
            taxon_t tax The taxon to return the parent of.
        )mydelimiter")
        .def("is_taxon_at", static_cast<bool (sys_t::*) (emp::WorldPosition)>(&sys_t::IsTaxonAt), R"mydelimiter(
            Returns whether a taxon is present at the given location. This will only work if the systematics manager is set to track positions (which can be checked with `get_store_position()`).

            Parameters
            ----------
            WorldPosition id Location to check for taxon.
        )mydelimiter")
        .def("get_taxon_at", static_cast<emp::Ptr<taxon_t> (sys_t::*) (emp::WorldPosition)>(&sys_t::GetTaxonAt), R"mydelimiter(
            Returns the taxon at the given location, if any. This will only work if the systematics manager is set to track positions (which can be checked with `get_store_position()`).

            Parameters
            ----------
            WorldPosition id Location of taxon.
        )mydelimiter")
        .def("get_mrca", static_cast<emp::Ptr<taxon_t> (sys_t::*) () const>(&sys_t::GetMRCA), py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a temporary, non-owning object (reference) representing the Most-Recent Common Ancestor of the population.
        )mydelimiter")
        .def("get_shared_ancestor", [](sys_t & self, taxon_t * t1, taxon_t * t2){return self.GetSharedAncestor(t1, t2);}, py::return_value_policy::reference_internal, R"mydelimiter(
            Returns a temporary, non-owning object (reference) representing the Most-Recent Common Ancestor shared by two given taxa.
            The order of the taxa does not matter.

            Parameters
            ----------
            taxon_t t1 Taxon to return MCRA of.
            taxon_t t2 Taxon to return MCRA of.
        )mydelimiter")

        // Birth notification
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org_by_position", static_cast<void (sys_t::*) (org_t &, emp::WorldPosition, emp::WorldPosition)>(&sys_t::AddOrg), "Add an organism to systematics manager")
        .def("add_org", [](sys_t & self, org_t & org){return self.AddOrg(org, nullptr);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)
        .def("add_org", [](sys_t & self, org_t & org, taxon_t * parent){return self.AddOrg(org, parent);}, "Add an organism to systematics manager", py::return_value_policy::reference_internal)

        // Death notification
        .def("remove_org_by_position", static_cast<bool (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrg), R"mydelimiter(
            Notify the systematics manager that an organism has died. Use this method if you're having the systematics manager keep track of each organism's position and want to use the position to tell the systematics manager which organism died. Otherwise use remove_org.

            Parameters
            ----------
            WorldPosition position: The location of the organism that died.
            )mydelimiter")
        .def("remove_org", [](sys_t & self, taxon_t * tax){return self.RemoveOrg(tax);}, R"mydelimiter(
            Notify the systematics manager that an organism has died. Use this method if you are keeping track of taxon objects yourself (rather than having the systematics manager handle it by tracking position).

            Parameters
            ----------
            Taxon tax: The taxon of the organism that died.
            )mydelimiter")
        .def("remove_org_by_position_after_repro", static_cast<void (sys_t::*) (emp::WorldPosition)>(&sys_t::RemoveOrgAfterRepro), R"mydelimiter(
            Notify the systematics manager that an organism has died but that it shouldn't record the death until the next reproduction event.
            You might want to do this if there's a chance that the organism simultaneously died and reproduced (e.g. if the organism's offspring might have replaced it in the population).

            Calling remove_org_by_position in that scenario could result in a segmentation fault if it was the last organism of its taxon. It could either be incorrectly marked extinct (if the offspring is part of the same taxon) and/or incorrectly pruned (if the offspring is part of a new taxon).

            Use this method if you're having the systematics manager keep track of each organism's position and want to use the position to tell the systematics manager which organism died. Otherwise use remove_org_after_repro.

            Parameters
            ----------
            WorldPosition position: The location of the organism that died.
            )mydelimiter")
        .def("remove_org_after_repro", [](sys_t & self, taxon_t * tax){self.RemoveOrgAfterRepro(tax);}, R"mydelimiter(
            Notify the systematics manager that an organism has died but that it shouldn't record the death until the next reproduction event.
            You might want to do this if there's a chance that the organism simultaneously died and reproduced (e.g. if the organism's offspring might have replaced it in the population).

            Calling remove_org_by_position in that scenario could result in a segmentation fault if it was the last organism of its taxon. It could either be incorrectly marked extinct (if the offspring is part of the same taxon) and/or incorrectly pruned (if the offspring is part of a new taxon).

            Use this method if you are keeping track of taxon objects yourself (rather than having the systematics manager handle it by tracking position).

            Parameters
            ----------
            Taxon tax: The taxon of the organism that died.
            )mydelimiter")
        .def("set_next_parent", [](sys_t & self, taxon_t * tax){self.SetNextParent(tax);}, R"mydelimiter(
            Sometimes, due to the flow of your program, you may not have access to the taxon object for the parent and the offspring at the same time. In these cases, you can use set_next_parent to tell the systematics manager what the taxon of the parent of the next offspring should be. The next time you call one of the add_org methods without a specified parent, the systematics manager will used the specified taxon as the parent for that organism.

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
            If you are tracking taxa by positions it is important that you notify the systematics manager any time a taxon changes position for any reason. This function allows you to do so. Assumes that all changes in position take the form of the contents of one location being replaced with the contents of a different location. If only one organism is moving, the location it is moving to is presumably empty and it is presumably leaving its former position empty. Thus, you will swap an organism with an empty space.

            Parameters
            ----------
            WorldPosition pos1: The position of one of the organisms being swapped.
            WorldPosition pos2: The position of one of the organisms being swapped.
            )mydelimiter")

        // Signals
        .def("on_new", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t, org_t & org)> & fun){self.OnNew(fun);}, R"mydelimiter(
            Set a custom function that is triggered every time a new taxon is created.
            The function must take two arguments: the first must be a Taxon object that represents the newly-minted taxon, and the second must be an object representing the organism the taxon was created from.
            The custom function will be triggered during the taxon creation process: after its origination time has been set, but before its organism or location have been recorded. This allows the user to customize the way objects are represented interlally by the systematics manager, or to implement extra bookkeeping functionality.

            Parameters
            ----------
            Callable[[Taxon, Organism], None] fun: Function to run during new taxon creation. It must take a Taxon object corresponding to the new taxon as its first argument, and an object representing the organism the taxon was created from as its second argument.
        )mydelimiter")
        .def("on_extinct", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t)> & fun){self.OnExtinct(fun);}, R"mydelimiter(
            Set a custom function that is triggered every time a taxon goes extinct.
            The function must take a single argument: a `taxon_t` object representing the taxon going extinct.
            The custom function will be triggered near the beginning of the taxon descruction process: after its descruction time has been set, but before any information has been destroyed. This allows the user to customize the way objects are represented interlally by the systematics manager, or to implement extra bookkeeping functionality.

            Parameters
            ----------
            Callable[taxon_t, None] fun: Function to run during taxon destruction. It must take a `taxon_t` object corresponding to the destroyee taxon.
        )mydelimiter")
        .def("on_prune", [](sys_t & self, std::function<void(emp::Ptr<taxon_t> t)> & fun){self.OnPrune(fun);}, R"mydelimiter(
            Set a custom function that is triggered every time a taxon is pruned from the tree. This occurs when a taxon and all its descendants go extinct.
            The function must take a single argument: a `taxon_t` object representing the taxon getting pruned.
            The custom function will be triggered at the beginning of the taxon pruning process.
            This allows the user to customize the way objects are represented interlally by the systematics manager, or to implement extra bookkeeping functionality.

            Parameters
            ----------
            Callable[taxon_t, None] fun: Function to run during taxon pruning. It must take a `taxon_t` object corresponding to the pruned taxon.
        )mydelimiter")

        // Phylostatistics
        .def("calc_diversity", static_cast<double (sys_t::*) () const>(&sys_t::CalcDiversity), R"mydelimiter(
            Calculates and returns the Shannon Diversity Index of the current extant population. This is done by weighing each active taxon by the number of organisms in it.
        )mydelimiter")
        .def("mrca_depth", static_cast<int (sys_t::*) () const>(&sys_t::GetMRCADepth), R"mydelimiter(
            This function returns the depth of the Most-Recent Common Ancestor for the active population -- that is, this returns the distance between the latest (i.e., newest) generation and the newest taxon that is an ancestor of the active taxa on the latest generation.
            If no MRCA exists, this returns -1.
        )mydelimiter")
        .def("colless_like_index", static_cast<double (sys_t::*) () const>(&sys_t::CollessLikeIndex), R"mydelimiter(
            Calculates and returns the Colless-like Index of the currently-active phylogenetic tree. This metric is used to measure tree balance in multifurcating trees, as described in doi:10.1371/journal.pone.0203401.
            The standard Colless Index is more well-known. However, that version is only applicable to bifurcating trees.
        )mydelimiter")
        .def("sackin_index", static_cast<int (sys_t::*) () const>(&sys_t::SackinIndex), R"mydelimiter(
            Calculates and returns the Sackin Index of the currently-active phylogenetic tree. This metric is used to measure phylogenetic tree balance.
            It is calcualted by adding together the depth of every leaf to its Most-Recent Common Ancestor (or its subtree root if none is found).
            For more information, see doi:10.2307/2992186.
        )mydelimiter")
        .def("get_branches_to_root", [](sys_t & self, taxon_t * tax){return self.GetBranchesToRoot(tax);}, R"mydelimiter(
            Given a taxon, this function calculates and returns the number of branches leading to multiple extant taxa on the path to its Most-Recent Common Ancestor (or its subtree root if none is found). Only extant taxa are considered since most phylogeny reconstruction algorithms are not designed to additionaly handle extinct taxa. This function does not count unifurcations -- that is, points at which each taxon only has a single ancestor.

            Parameters
            ----------
            Taxon tax: Taxon to find branches to MRCA (or subroot) of.
        )mydelimiter")
        .def("get_distance_to_root", [](sys_t & self, taxon_t * tax){return self.GetDistanceToRoot(tax);}, R"mydelimiter(
            Given a taxon, this function calculates and returns the distance to its Most-Recent Common Ancestor (or its subtree root if none is found).
            This function considers both unifurcations and extinct taxa. Consider using `get_branches_to_root()` instead for phylogeny reconstruction purposes.

            Parameters
            ----------
            Taxon tax: Taxon to find distance to MRCA (or subroot) of.
        )mydelimiter")
        .def("get_variance_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetVariancePairwiseDistance), R"mydelimiter(
            This method calculates the variance of distance between all pairs of extant taxa. This is a measure of phylogenetic regularity (Tucker et. al., 2017).
            This assumes the phylogenetic tree is fully connected. If this is not the case, it will return -1.

            If `branch_only` is set, this method will only consider distances in terms of nodes that represent branches between two extant taxa. This is potentially useful as a comparison to real-world, biological data, where non-branching nodes cannot be inferred.

            Parameters
            ----------
            bool branch_only: Only counts distance in terms of nodes that represent a branch between two extant taxa.
        )mydelimiter")
        .def("get_mean_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetMeanPairwiseDistance), R"mydelimiter(
        )mydelimiter")
        .def("get_sum_pairwise_distance", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetSumPairwiseDistance), R"mydelimiter(
            This method calculates the mean distance between all pairs of extant taxa, also known as the Average Taxonomic Diversity. This is a measure of community distinctness (Webb and Losos, 2000; Warwick and Clark, 1998).
            This assumes the phylogenetic tree is fully connected. If this is not the case, it will return -1.

            If `branch_only` is set, this method will only consider distances in terms of nodes that represent branches between two extant taxa. This is potentially useful as a comparison to real-world, biological data, where non-branching nodes cannot be inferred.

            Parameters
            ----------
            bool branch_only: Only counts distance in terms of nodes that represent a branch between two extant taxa.
        )mydelimiter")
        .def("get_phylogenetic_diversity", static_cast<int (sys_t::*) () const>(&sys_t::GetPhylogeneticDiversity), R"mydelimiter(
            This method calculates the sum of edges of the Minimum Spanning Tree of the currently-active phylogenetic tree (Faith 1992, reviewed in Winters et al., 2013). Assuming all parent-child edges have a length of 1 (i.e., there are no unifurcations), this is a measure of phylogenetic diversity.
        )mydelimiter")
        .def("get_average_origin_time", static_cast<double (sys_t::*) (bool) const>(&sys_t::GetAverageOriginTime), py::arg("normalize") = false, R"mydelimiter(
            This method calculates the average origin time for the whole phylogenetic tree by iterating through its taxa.

            It can optionally normalize the result to make it comparable to that of a strictly bifurcating tree--this is the kind of tree produced by traditional reconstruction methods. Normalization is achieved by multiplying the value of each taxon by its number of offspring minus one.

            Parameters
            ----------
            bool normalize: Whether to normalize result.
        )mydelimiter")
        .def("get_out_degree_distribution", static_cast<std::unordered_map<int, int> (sys_t::*) () const>(&sys_t::GetOutDegreeDistribution), R"mydelimiter(
            This method creates a histogram of outnode degrees. In other words, it iterates through the whole phylogenetic tree and counts the number of outgoing edges for each node, returning this data in the form of a dictionary, with keys being outnode degrees and values being counts.

            For example, {0:12, 1:10, 2:33} means the tree has 12 zero-furcations (leaf nodes), 10 unifurcations, and 33 bifurcations.
        )mydelimiter")
        .def("get_mean_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetMeanEvolutionaryDistinctiveness), R"mydelimiter(
            This method calculates the mean evolutionary distinctiveness of all extant taxa.
            Mean evolutionary distinctiveness is a metric that measures how distinct any given taxa is from the rest of the population, weighted by the amount of evolutionary history it represents (Isaac, 2007; Winter et. al., 2013).
            This method takes the *current* time as a parameter, in whichever units the systematics manager is using. Using a non-present time will produce innacurate results, since the only known state of the tree is the current one.

            Parameters
            __________
            double time: Current time in the appropiate units (e.g., generations, seconds, etc.)
        )mydelimiter")
        .def("get_sum_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetSumEvolutionaryDistinctiveness), R"mydelimiter(
            This method calculates the sum of evolutionary distinctiveness of all extant taxa.
            Mean evolutionary distinctiveness is a metric that measures how distinct any given taxa is from the rest of the population, weighted by the amount of evolutionary history it represents (Isaac, 2007; Winter et. al., 2013).
            This method takes the *current* time as a parameter, in whichever units the systematics manager is using. Using a non-present time will produce innacurate results, since the only known state of the tree is the current one.

            Parameters
            __________
            double time: Current time in the appropiate units (e.g., generations, seconds, etc.)
        )mydelimiter")
        .def("get_variance_evolutionary_distinctiveness", static_cast<double (sys_t::*) (double) const>(&sys_t::GetVarianceEvolutionaryDistinctiveness), R"mydelimiter(
            This method calculates the variance of evolutionary distinctiveness across all extant taxa.
            Mean evolutionary distinctiveness is a metric that measures how distinct any given taxa is from the rest of the population, weighted by the amount of evolutionary history it represents (Isaac, 2007; Winter et. al., 2013).
            This method takes the *current* time as a parameter, in whichever units the systematics manager is using. Using a non-present time will produce innacurate results, since the only known state of the tree is the current one.

            Parameters
            ----------
            double time: Current time in the appropiate units (e.g., generations, seconds, etc.)
        )mydelimiter")

        // Input
        .def("load_from_file", static_cast<void (sys_t::*) (const std::string &, const std::string &, bool, bool)>(&sys_t::LoadFromFile), py::arg("file_path"), py::arg("info_col") = "info", py::arg("assume_leaves_extant") = true, py::arg("adjust_total_offspring") = true, R"mydelimiter(
            This method loads phylogenies into the systematics manager from a given file, replacing the currently-present phylogenies, if any. It is only successful when the `info_col` type is convertible to the systematics manager's ORG_INFO type. Such a phylogeny file can be obtained by calling `snapshot()` on a systematics manager with an active phylogeny.

            Parameters
            ----------
            string file_path: Path to file containing phylogenies to be loaded. Either absolute or relative to the Python executable.
            string info_col: Name of file column containing taxa information. Defaults to `"info"`.
            bool assume_leaves_extant: Whether leaves are assumed to be extant. Defaults to `True`.
            bool adjust_total_offstring: Whether total offspring count should be adjusted for all taxa. Defaults to `True`.
            )mydelimiter")

        // Output
        .def("snapshot", static_cast<void (sys_t::*) (const std::string &) const>(&sys_t::Snapshot), R"mydelimiter(
            This method takes a snapshot of the current state of the phylogeny and stores it to a file. This file can then be loaded through `load_from_file()`.
            Note that this assumes each taxon only has one parent taxon.

            Parameters
            ----------
            string file_path: File path to save snapshot to.
        )mydelimiter")
        .def("add_snapshot_fun", static_cast<void (sys_t::*) (const std::function<std::string(const taxon_t &)> &, const std::string &, const std::string &) >(&sys_t::AddSnapshotFun), R"mydelimiter(
            This method adds a new snapshot function that will run in addition to the default functions when a snapshot is taken. A custom snapshot function should be created whenever storage and retrival of custom taxon data is desired.

            Custom snapshot functions must take a Taxon object as a single argument and return the data to be saved as a string. The second argument to this method is the key the custom information will be stored under in the snapshot file. Optionally, a short description of the custom information can be provided as its third argument.

            Parameters
            ----------
            Callable[Taxon, str] fun: Custom snapshot function, taking a Taxon object as its singular parameter, and returning the custom data as a string.
            str key: Key to store the custom information under inside the snapshot file.
            str desc: Optional description for the custom information.
        )mydelimiter")
        .def("print_status", [](sys_t & self){self.PrintStatus();}, R"mydelimiter(
            This method prints details about the systematics manager.
            It first prints all settings. Then, it prints all stored active, ancestor, and outside taxa in that order.
            The format for a taxon is [id | number of orgs, number of offspring | parent taxon].
        )mydelimiter")
        .def("print_lineage", [](sys_t & self, taxon_t * tax){self.PrintLineage(tax);}, R"mydelimiter(
            This method prints the complete lineage of a given taxon. The output format consists of the string "Lineage:" followed by each taxon in the lineage on a new line.

            Parameters
            ----------
            Taxon tax: Taxon to print the lineage of.
        )mydelimiter")

        // Time tracking
        .def("update", static_cast<void (sys_t::*) ()>(&sys_t::Update), R"mydelimiter(
            Calling this method advances the tracking by one time step. This can be useful for tracking taxon survival times, as well as population positions in synchronous generation worlds.
        )mydelimiter")

        // Efficiency functions
        .def("remove_before", static_cast<void (sys_t::*) (int)>(&sys_t::RemoveBefore), R"mydelimiter(
            This method removes all taxa that went extinct before the given time step, and that only have ancestors taht went extinct before the given time step. While this invalidates most tree topology metrics, it is useful when limited ancestry tracking is necessary, but complete ancestry tracking is not computationally possible.

            Parameters
            ----------
            int ud: Time step before which to remove taxa.
        )mydelimiter")
        ;
}
