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
    wp = systematics.WorldPosition(1, 0)
    org = ExampleOrg("hello")
    sys.addOrgByPosition(org, wp)


def test_systematics():
    # sys = systematics.Systematics(taxon_info_fun, True, True, False, False)
    sys = systematics.Systematics(str, True, True, False, False)
    org = ExampleOrg("hello")
    org2 = ExampleOrg("hello 2")
    org_tax = systematics.Taxon(0, "hello")
    org2_tax = sys.addOrg(org2, org_tax)


if __name__ == "__main__":
    test_systematics()
