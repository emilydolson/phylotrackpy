import systematics


class ExampleOrg:
    def __init__(self, genotype):
        self.genotype = genotype


def taxon_info_fun(org):
    return org.genotype


def test_world_position():
    wp = systematics.WorldPosition(1, 0)
    assert(wp.getIndex() == 1)
    assert(wp.getPopID() == 0)


def test_systematics():
    sys = systematics.Systematics(taxon_info_fun)
    wp = systematics.WorldPosition(1, 0)
    org = ExampleOrg("hello")
    sys.addOrg(org, wp)
