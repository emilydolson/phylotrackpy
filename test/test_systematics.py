from phylotrackpy import systematics
from pytest import approx
from copy import deepcopy


class ExampleOrg:
    def __init__(self, genotype):
        self.genotype = genotype

    def __repr__(self):
        return "ExampleOrg object " + self.genotype


def taxon_info_fun(org):
    return org.genotype


def test_world_position():
    wp = systematics.WorldPosition(1, 0)
    assert(wp.get_index() == 1)
    assert(wp.get_pop_ID() == 0)


def test_systematics_by_position():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, True)
    org_pos = systematics.WorldPosition(1, 0)
    org = ExampleOrg("hello")
    sys.add_org_by_position(org, org_pos)
    child_pos = systematics.WorldPosition(2, 0)
    child_org = ExampleOrg("hello2")
    sys.add_org_by_position(child_org, child_pos, org_pos)
    sys.remove_org_by_position(org_pos)
    # sys.remove_org_by_position((2,0))


def test_systematics():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    org = ExampleOrg("hello")
    org2 = ExampleOrg("hello 2")
    org_tax = systematics.Taxon(0, "hello")
    org2_tax = sys.add_org(org2, org_tax)
    org3_tax = sys.add_org(org2)
    org4_tax = sys.add_org(org, org2_tax)
    org5_tax = sys.add_org(org, org4_tax)

    assert(org2_tax.get_parent() == org_tax)
    assert(org3_tax.get_parent() is None)
    assert(org4_tax.get_parent() == org2_tax)
    assert(org_tax != org2_tax)
    assert(org4_tax != org2_tax)
    assert(org5_tax == org4_tax)
    assert(sys.get_num_active() == 3)
    assert(sys.get_num_ancestors() == 0)

    assert(not sys.remove_org(org2_tax))
    assert(sys.get_num_active() == 2)
    assert(sys.get_num_ancestors() == 1)


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
    assert(shared == org1_tax)


def test_load_data():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    sys.load_from_file("test/assets/systematics_snapshot.csv", "genome", True)

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


def test_phylostatistics():
    sys = systematics.Systematics(lambda x: str(x), True, True, False, False)

    sys.set_update(0)
    id1 = sys.add_org(25)
    assert sys.get_average_origin_time() == 0
    assert sys.get_average_origin_time(True) == 0

    sys.set_update(6)
    id2 = sys.add_org(1, id1)
    assert sys.get_average_origin_time() == 3
    assert sys.get_average_origin_time(True) == 0

    sys.set_update(10)
    id3 = sys.add_org(26, id1)
    assert sys.get_average_origin_time() == approx(5.3333333)
    assert sys.get_average_origin_time(True) == 0

    sys.set_update(25)
    id4 = sys.add_org(27, id2)
    assert sys.get_average_origin_time() == approx(10.25)
    assert sys.get_average_origin_time(True) == 0

    sys.set_update(32)
    id5 = sys.add_org(28, id2)
    assert sys.get_average_origin_time() == approx(14.6)
    assert sys.get_average_origin_time(True) == 3
    
    id6 = sys.add_org(30, id5)

    out_dist = sys.get_out_degree_distribution()
    assert out_dist[0] == 3
    assert out_dist[1] == 1
    assert out_dist[2] == 2

    assert sys.calc_diversity() == approx(2.58496)
    assert sys.get_ave_depth() == approx(1.5)


def test_loading_stats():
    sys = systematics.Systematics(lambda x: x)
    sys.load_from_file("test/assets/consolidated.csv", "id")

    assert sys.get_ave_depth() > 0
    # assert sys.calc_diversity() > 0
    assert sys.get_mean_evolutionary_distinctiveness(32766) > 0
    assert sys.get_sum_evolutionary_distinctiveness(32766) > 0
    assert sys.get_variance_evolutionary_distinctiveness(32766) > 0

    sys.load_from_file("test/assets/full.csv", "id", True, False)


def test_deepcopy():
    sys = systematics.Systematics(lambda x: x, True, True, False, False)
    tax = sys.add_org("hello")
    tax2 = deepcopy(tax)
    assert tax.get_id() == tax2.get_id()
    
    sys.add_org("hello", tax)
    assert tax.get_num_orgs() == 2
    assert tax2.get_num_orgs() == 2


def test_taxon():
    tax = systematics.Taxon(0, "hello")
    assert tax.get_info() == "hello"


# def test_data():
#     sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
#     org = ExampleOrg("hello")
#     org_tax = sys.add_org(org)
#     org_tax.get_data().set_data({"val":4, "test": 9})
#     assert(org_tax.get_data().data["val"] == 4)


if __name__ == "__main__":
    # test_loading_stats()
    pass
