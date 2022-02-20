import systematics


class ExampleOrg:
    def __init__(self, genotype):
        self.genotype = genotype

    def __repr__(self):
        return "ExampleOrg object " + self.genotype


def taxon_info_fun(org):
    return org.genotype


def test_world_position():
    wp = systematics.WorldPosition(1, 0)
    assert(wp.getIndex() == 1)
    assert(wp.getPopID() == 0)


def test_systematics_by_position():
    sys = systematics.Systematics(taxon_info_fun, True, True, False, True)
    org_pos = systematics.WorldPosition(1, 0)
    org = ExampleOrg("hello")
    sys.add_org_by_position(org, org_pos)
    child_pos = systematics.WorldPosition(2, 0)
    child_org = ExampleOrg("hello2")
    sys.add_org_by_position(child_org, child_pos, org_pos)
    # sys.remove_org()


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

if __name__ == "__main__":
    test_systematics()
