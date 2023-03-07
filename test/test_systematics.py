from phylotrackpy import systematics


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
    # sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
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

# def test_data():
#     sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
#     org = ExampleOrg("hello")
#     org_tax = sys.add_org(org)
#     org_tax.get_data().set_data({"val":4, "test": 9})
#     assert(org_tax.get_data().data["val"] == 4)


if __name__ == "__main__":
    test_systematics()
