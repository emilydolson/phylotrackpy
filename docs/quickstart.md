# Quickstart

## Installation

You can install phylotrackpy with pip:

```bash
pip install phylotrackpy
```

## Usage

### Creating a systematics object

The first step in tracking a phylogeny with phylotrackpy is to make a systematics object. The most important decision to make at this point is how to define {term}`taxa` in your phylogeny (for more information, see the [flexible taxon definition](introduction.md#flexible-taxon-definitions) section). You can do so by passing a function to the systematics constructor which takes an organism object and returns a string that specifies a {term}`taxon<Taxon>`.

For example, to build a phylogeny based on genotypes, you could do the following:

```py
from phylotrackpy import systematics

# Assuming that the objects being used as organisms have a member variable called genotype that stores their genotype,
# this will created a phylogeny based on genotypes
syst = systematics.Systematics(lambda org: org.genotype)
```

If you want to base your phylogeny on raw organism objects (e.g. if your organisms are lists and you want to make a new taxon any time a list is produced that doesn't match its parent), you can rely on default arguments to the Systematics constructor (assuming your organism objects aren't anything too weird):

```py
from phylotrackpy import systematics

# By default the objects being used as organisms will be
# the basis for the phylogeny.
syst = systematics.Systematics()
```

There are a couple of other decisions that you also need to make at this point. The first is which set of taxa to store in the systematics manager. The defaults here are most likely what you want to use, but in case they aren't, the systematics manager can be told to store or not store the following sets of taxa:

- **active**: the taxa that still currently have living members. You almost certainly want to store these (without them you don't really have a phylogeny), but can technically disable them by setting the `store_active` keyword argument in the constructor to false.
- **ancestors**: the taxa that are ancestors of active taxa. You almost certainly want to store these too (without them you don't really have a phylogeny), but can technically disable them by setting the `store_ancestors` keyword argument in the constructor to false.
- **outside**: the taxa that are not in either of the other two groups (i.e. taxa that have gone extinct and all of their ancestors have gone extinct). If you store these, your phylogeny will get very large very fast, so doing so is generally not recommended. It is occasionally useful, though, so you can enable storing these taxa by setting the `store_all` keyword argument in the constructor to true.

The second decision is slightly trickier. Once you start adding organisms to the systematics manager, it will create [`Taxon`](phylotrackpy.systematics.Taxon) objects associated with each one to keep track of which taxon it is part of. You will need to use these taxon objects when adding future organisms, to specify which taxon their parent was part of. If you have control over your organism class, it is likely that the easiest option is to add a `self.taxon` attribute and store the taxon there. However, if you cannot add arbitrary data to your organism class, keeping track of taxon objects can get annoying. For this reason, the systematics manager gives you the option of letting it manage them. To do so, it needs a way to map individuals to taxa (since its possible there are duplicate taxa, simply running the organism to taxon function again won't work). It achieves this mapping by keeping track of each organism's position in the population. Thus, to have the systematics manager keep track of taxon objects itself, you must set the `store_pos` keyword argument in the constructor to true. You must also use the position-based versions of add_org and remove_org, and make sure to notify the systematics manager if any organism ever changes position during its lifetime for any reason.

Once you have created the [`Systematics`](phylotrackpy.systematics.Systematics) object, you just need to do two things: 1) notify it when something is born, and 2) notify it when something dies.

### Notifying the systematics object of births

You must notify the systematics manager of births using the `add_org` family of functions. These functions require that you provide the newly born organism as well as either the taxon object of its parent or the position of its parent (if the systematics manager is tracking positions).

Example of tracking taxa as object attributes (assume we're building on our example above, and already have created a systematics manager called `sys`):

```py
# Do whatever you would normally do to create your first organism
# Here, we're assuming we can just call a constructor called Organism()
my_org = Organism()

# Notify systematics manager of this organism's birth
# This is the first org, so it doesn't have a parent
# so we do not pass a second argument/
# add_org will return this organism's taxon object, which we
# store in my_org.taxon for future reference
my_org.taxon = syst.add_org(my_org)

# Assume stuff happens here that leads to my_org having offspring
# Here, we'll pretend that our organism class has a Reproduce method that
# returns a new offspring organism. You should handle this however you
# normally would
org_2 = my_org.Reproduce()

# Notify the systematics manager of org_2's birth. Since it has a parent,
# we pass the taxon of that parent in as the second argument
# Again, we store the returned taxon object in the organism for safe keeping
org_2.taxon = syst.add_org(org_2, my_org.taxon)

```

An example of tracking positions is coming soon. For now, feel free to contact us with questions!

### Notifying the systematics object of deaths

You must notify the systematics manager of deaths using the `remove_org` family of functions.

As an example (again, building on the previous examples):
```py
# Assume stuff happens that causes my_org to die

# We notify the systematics manager that this has happened by calling remove_org
# Note that remove_org takes the taxon of the dead organism as an argument, not
# the organism itself
syst.remove_org(my_org.taxon)

```

### Importing and exporting data

Phylotrackpy has native support to load and save phylogenies via CSV files formatted according to the [Alife data standard phylogeny format](https://alife-data-standards.github.io/alife-data-standards/phylogeny.html).

```py
# save our phylogeny to file
syst.snapshot("phylo.csv")

# load a phylogeny from file
loaded_sys = systematics.Systematics(lambda org: org.genotype)
loaded_sys.load_from_file("phylo.csv", "id")
```

Capability to interoperate with other phylogenetic computing libraries (BioPython, DendroPy, ETE) and bioinformatics serialization schemas (newick, nexml, nexus) is provided through support from [`alifedata-phyloinformatics-convert`](https://github.com/mmore500/alifedata-phyloinformatics-convert/).
```py
import io
from pathlib import Path

import alifedata_phyloinformatics_convert as apc
import Bio
import dendropy
import ete3 as ete


# 1) initialize RosettaTree converter
# ... with PhylotrackPy data
converter = apc.RosettaTree(syst)
# ... or from other libraries' data structures
converter = apc.RosettaTree(
  Bio.Phylo.read(io.StringIO("(A,B,(C,D));"), "newick"),
)
converter = apc.RosettaTree(
  dendropy.Tree.get(data="(A,B,(C,D));", schema="newick"),
)
converter = apc.RosettaTree(
  ete.Tree("(A,B,(C,D));"),
)
# ... or by deserializing from newick, nexml, or nexus formats
converter = apc.RosettaTree.from_newick("(A,B,(C,D));")
converter = apc.RosettaTree.from_nexml(Path("path/to/phylo.nemxl"))
with open("path/to/phlo.nexus", "r") as fp:
  converter = apc.RosettaTree.from_nexus(fp)

# 2) transform data as desired 
# ... coerce to PhylotrackPy data structure
converter.as_phylotrack
# ... coerce to other libraries' data structures
converter.as_alife  # pandas DataFrame
converter.as_biopython
converter.as_dendropy
converter.as_ete
converter.as_networkx
# ... serialize to standard formats
converter.to_newick()  # returns newick string
converter.to_nexml(Path("phylo.nexml"))  # writes to path
with open("phylo.nexus", "w") as fp:  # writes to file object
  converter.to_nexus(fp)
```

See the [`alifedata-phyloinformatics-convert` documentation](https://alifedata-phyloinformatics-convert.readthedocs.io/en/latest/#) for further information.
