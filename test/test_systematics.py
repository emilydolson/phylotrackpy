#!/usr/bin/env python3
import os
from phylotrackpy import systematics
import pytest
from pytest import approx, mark
from copy import deepcopy
import tempfile
 
assets_path = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), 
    "assets",
)


class ExampleOrg:
    def __init__(self, genotype):
        self.genotype = genotype

    def __repr__(self):
        return "ExampleOrg object " + str(self.genotype)


def taxon_info_fun(org):
    return org.genotype


def test_world_position():
    wp = systematics.WorldPosition(1, 0)
    assert wp.get_index() == 1
    assert wp.get_pop_ID() == 0


def test_systematics_by_position():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, True)
    assert sys.get_store_position()
    org_pos = systematics.WorldPosition(1, 0)
    org = ExampleOrg("hello")
    sys.add_org_by_position(org, org_pos)
    child_pos = systematics.WorldPosition(2, 0)
    child_org = ExampleOrg("hello2")
    sys.add_org_by_position(child_org, child_pos, org_pos)
    sys.remove_org_by_position(org_pos)
    # sys.remove_org_by_position((2,0))


def test_construct_systematics():
    sys1 = systematics.Systematics(taxon_info_fun, True, True, False, True)
    assert sys1.get_store_position()

    sys2 = systematics.Systematics(taxon_info_fun, True, True, False, False)
    assert not sys2.get_store_position()
    assert sys2.get_store_ancestors()
    assert not sys2.get_store_outside()
    assert sys2.get_store_active()


@mark.parametrize(
    "taxa",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_systematics(taxa):
    tax1, tax2 = taxa
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    assert not sys.get_store_position()

    org = ExampleOrg(tax1)
    org2 = ExampleOrg(tax2)
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    assert org2_tax.get_parent() == org_tax
    assert org3_tax.get_parent() is None
    assert org4_tax.get_parent() == org2_tax
    assert org_tax != org2_tax
    assert org4_tax != org2_tax
    assert org5_tax == org4_tax
    assert sys.get_num_active() == 4
    assert sys.get_num_ancestors() == 0

    assert not sys.remove_org(org2_tax)
    assert sys.get_num_active() == 3
    assert sys.get_num_ancestors() == 1


@mark.nowheel
def test_systematics_numpy():
    import numpy as np
    taxa = [np.array([1]), np.array([1, 2])]
    test_systematics(taxa)


@mark.parametrize(
    "taxa",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_string_systematics(taxa):
    tax1, tax2 = taxa
    sys = systematics.Systematics(lambda x: str(x), True, True, False, False)
    assert not sys.get_store_position()

    org = tax1
    org2 = tax2
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    assert org2_tax.get_info() == str(tax2)
    assert org3_tax.get_info() == str(tax2)
    assert org4_tax.get_info() == str(tax1)
    assert org5_tax.get_info() == str(tax1)
    assert org2_tax.get_parent() == org_tax
    assert org3_tax.get_parent() is None
    assert org4_tax.get_parent() == org2_tax
    assert org_tax != org2_tax
    assert org4_tax != org2_tax
    assert org5_tax == org4_tax
    assert sys.get_num_active() == 4
    assert sys.get_num_ancestors() == 0

    assert not sys.remove_org(org2_tax)
    assert sys.get_num_active() == 3
    assert sys.get_num_ancestors() == 1


@mark.parametrize(
    "taxa",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_raw_systematics(taxa):
    tax1, tax2 = taxa
    sys = systematics.Systematics()
    assert not sys.get_store_position()

    org = tax1
    org2 = tax2
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    assert org2_tax.get_parent() == org_tax
    assert org3_tax.get_parent() is None
    assert org4_tax.get_parent() == org2_tax
    assert org_tax != org2_tax
    assert org4_tax != org2_tax
    assert org5_tax == org4_tax
    assert sys.get_num_active() == 4
    assert sys.get_num_ancestors() == 0

    assert not sys.remove_org(org2_tax)
    assert sys.get_num_active() == 3
    assert sys.get_num_ancestors() == 1


@mark.parametrize(
    "orgs",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_taxa_serialization(orgs):
    org_info_1, org_info_2 = orgs
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    org = ExampleOrg(org_info_1)
    org2 = ExampleOrg(org_info_2)
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2, org_tax)
    sys.remove_org(org_tax)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    with tempfile.NamedTemporaryFile() as f:
        f.file.close()
        sys.add_snapshot_fun(systematics.encode_taxon, "info")
        sys.snapshot(f.name)
        sys = systematics.Systematics(lambda x: x, True, True, False, False)
        sys.load_from_file(f.name)

    assert sys.get_mrca().get_info() == org_info_1


@mark.nowheel
def test_numpy_serialization():
    import numpy as np
    test_taxa_serialization([np.array([1]), np.array([1, 2])])


@mark.parametrize(
    "orgs",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_string_taxa_serialization(orgs):
    org_info_1, org_info_2 = orgs
    sys = systematics.Systematics(lambda x: str(x))
    org = org_info_1
    org2 = org_info_2
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2, org_tax)
    sys.remove_org(org_tax)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    with tempfile.NamedTemporaryFile() as f:
        f.file.close()
        sys.add_snapshot_fun(lambda x: x.get_info(), "info")
        sys.snapshot(f.name)
        sys = systematics.Systematics(lambda x: str(x))
        sys.load_from_file(f.name)

    assert sys.get_mrca().get_info() == org_info_1


@mark.parametrize(
    "orgs",
    (
        ["hello", "hello 2"],
        [1, 2],
        [1.0, 2.0],
        [[1], [1, 2]],
    ),
)
def test_raw_taxa_serialization(orgs):
    org_info_1, org_info_2 = orgs
    sys = systematics.Systematics()
    org = org_info_1
    org2 = org_info_2
    org_tax = sys.add_org(org)
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2, org_tax)
    sys.remove_org(org_tax)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    with tempfile.NamedTemporaryFile() as f:
        f.file.close()
        sys.add_snapshot_fun(systematics.encode_taxon, "info")
        sys.snapshot(f.name)
        sys = systematics.Systematics()
        sys.load_from_file(f.name)

    assert sys.get_mrca().get_info() == org_info_1


def test_shared_ancestor():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    org1 = ExampleOrg("hello")
    org2 = ExampleOrg("hello 2")
    org3 = ExampleOrg("hello 3")
    org4 = ExampleOrg("hello 4")

    org1_tax = sys.add_org(org1)
    org2_tax = sys.add_org(org2, org1_tax)
    org3_tax = sys.add_org(org3, org2_tax)
    org4_tax = sys.add_org(org4, org1_tax)

    shared = sys.get_shared_ancestor(org3_tax, org4_tax)
    assert shared == org1_tax


def test_load_data():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    sys.load_from_file(f"{assets_path}/systematics_snapshot.csv", "genome", True)

    assert sys.get_num_roots() == 1
    assert sys.get_num_active() == 6
    assert sys.get_num_ancestors() == 4
    assert sys.get_num_outside() == 0
    assert sys.get_num_taxa() == 10
    assert sys.get_max_depth() == 4

    mrca = sys.get_mrca()
    assert mrca.get_id() == 1
    assert mrca.get_total_offspring() == 6
    assert mrca.get_num_offspring() == 3

    offspring = mrca.get_offspring()
    for off in offspring:
        assert off.get_id() in [7, 2, 3]

    sys2 = systematics.Systematics()
    sys2.load_from_file(f"{assets_path}/extant_root.csv", "id")


def test_phylostatistics():
    sys = systematics.Systematics(str, True, True, False, False)

    sys.set_update(0)
    id1 = sys.add_org(25)
    assert sys.get_average_origin_time() == 0
    assert sys.get_average_origin_time(True) == 0
    assert sys.get_pairwise_distance(id1, id1) == approx(0)

    sys.set_update(6)
    id2 = sys.add_org(1, id1)
    assert sys.get_average_origin_time() == 3
    assert sys.get_average_origin_time(True) == 0
    assert sys.get_pairwise_distance(id2, id1) == approx(1)

    sys.set_update(10)
    id3 = sys.add_org(26, id1)
    assert sys.get_average_origin_time() == approx(5.3333333)
    assert sys.get_average_origin_time(True) == 0
    assert sys.get_pairwise_distance(id2, id3) == approx(2)

    sys.set_update(25)
    id4 = sys.add_org(27, id2)
    assert sys.get_average_origin_time() == approx(10.25)
    assert sys.get_average_origin_time(True) == 0
    assert sys.get_pairwise_distance(id4, id3) == approx(3)

    sys.set_update(32)
    id5 = sys.add_org(28, id2)
    assert sys.get_average_origin_time() == approx(14.6)
    assert sys.get_average_origin_time(True) == 3
    assert sys.get_pairwise_distance(id5, id4) == approx(2)
    assert sys.get_pairwise_distance(id5, id3) == approx(3)
    assert sys.get_pairwise_distance(id5, id2) == approx(1)
    assert sys.get_pairwise_distance(id5, id3, True) == approx(3)

    id6 = sys.add_org(30, id5)
    assert sys.get_pairwise_distance(id6, id3, True) == approx(3)
    assert sys.get_pairwise_distance(id6, id3) == approx(4)

    out_dist = sys.get_out_degree_distribution()
    assert out_dist[0] == 3
    assert out_dist[1] == 1
    assert out_dist[2] == 2

    assert sys.calc_diversity() == approx(2.58496)
    assert sys.get_ave_depth() == approx(1.5)


def test_loading_stats():
    sys = systematics.Systematics()
    sys.load_from_file(f"{assets_path}/consolidated.csv", "id")

    assert sys.get_ave_depth() > 0
    # assert sys.calc_diversity() > 0
    assert sys.get_mean_evolutionary_distinctiveness(32766) > 0
    assert sys.get_sum_evolutionary_distinctiveness(32766) > 0
    assert sys.get_variance_evolutionary_distinctiveness(32766) > 0
    assert len(sys.get_active_taxa()) > 0
    assert len(sys.get_active_taxa()) > 0  # Check for object lifetime bug

    sys.load_from_file(f"{assets_path}/full.csv", "id", True, False)


def test_deepcopy():
    sys = systematics.Systematics(lambda x: x, True, True, False, False)
    tax = sys.add_org("hello")
    tax2 = deepcopy(tax)
    assert tax.get_id() == tax2.get_id()
    
    sys.add_org("hello", tax)
    assert tax.get_num_orgs() == 2
    assert tax2.get_num_orgs() == 2
    sys.remove_org(tax)
    sys.remove_org(tax2)
    # sys.remove_org(tax)


def test_taxon():
    tax = systematics.Taxon(0, "hello")
    assert tax.get_info() == "hello"


# def test_data():
#     sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
#     org = ExampleOrg("hello")
#     org_tax = sys.add_org(org)
#     org_tax.get_data().set_data({"val":4, "test": 9})
#     assert(org_tax.get_data().data["val"] == 4)
