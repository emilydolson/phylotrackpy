# Glossary

Some useful terminology that might be useful in understanding the documentation (and especially the code base) for Phylotrackpy, particularly in light of the fact that different sub-fields of evolutionary biology tend to use different words in many of these contexts.

{.glossary}
Taxon
: a generic word for a specific taxonomic unit. We use "taxon" as a generic term to represent a node in a phylogeny. For example, species are a common type of taxon to use when depicting portions of the phylogeny of life on earth. However, sometimes people choose to use higher-order types of taxa (e.g. genus, family, order, class, etc.) when they are trying to depict a larger swath of the whole phylogeny.

Taxa
: Plural of taxon.

Multifurcation
: A node in a phylogeny that has more than two child nodes

Polytomy
: See multifurcation

Bifurcation
: A node in a phylogeny that has exactly two child nodes.

Non-branch node
: A node in a phylogeny with only one child node.

Leaf node
: A node in a phylogeny with no children.

Most Recent Common Ancestor (MRCA)
: The most recent node in a phylogeny that is a common ancestor of all nodes associated with extant taxa. If the phylogeny is pruned, there won't be any branch points before the MRCA (because any branches not leading to the MRCA would lead to taxa that are now extinct).

Coalescence events
: Occur when the most recent common ancestor changes (i.e. all descendants from one side of the deepest branch of the phylogeny have gone extinct). In the absence of diversity-preserving features coalescence events are expected to occur by chance with a frequency dependent on population size and spatial structure (but be careful of distributional assumptions). Observing coalescence less frequently than you would expect by chance can be an indication that ecological interactions are present (we have discussed this more [here](https://direct.mit.edu/artl/article/26/1/58/93272/Interpreting-the-Tape-of-Life-Ancestry-Based) and [here](https://direct.mit.edu/artl/article/25/1/50/2915/The-MODES-Toolbox-Measurements-of-Open-Ended)).
