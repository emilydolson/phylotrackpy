# Nuances of Exact Phylogenies

There are certain quirks associated with real-time phylogenies that you might not be used to thinking about if you're used to dealing with reconstructed phylogenies. Many of these discrepancies are the result of the very different temporal resolutions on which these types of phylogenies are measured, and the fact that the taxonomic units we work with are often at a finer resolution than species. We document some here so that they don't catch you off guard:

- **Multifurcations are real**: In phylogenetic reconstructions, there is usually an assumption that any multifurcation/polytomy (i.e. a node that has more than two child nodes) is an artifact of having insufficient data. In real-time phylogenies, however, we often observe multifurcations that we know for sure actually happened.
- **Not all extant taxa are leaf nodes**: In phylogenetic reconstructions, there is usually an assumption that all extant (i.e. still living) taxa are leaf nodes in the phylogeny (i.e. none of them are parents/offspring of each other; similar taxa are descended from a shared common ancestor). In real-time phylogenies it is entirely possible that one taxon gives birth to something that we have defined as a different taxon and then continues to coexist with that child taxon.
- **Not all nodes are branch points**: In phylogenetic reconstructions, we only attempt to infer where branch points (i.e. common ancestors of multiple taxa) occurred. We do not try to infer how many taxa existed on a line of descent between a branch point and an extant taxa. In real-time phylogenies we observe exactly how many taxa exist on this line of descent and we keep a record of them. In practice there are often a lot of them, depending on you define your taxa. It is unclear whether we should include these non-branching nodes when calculating phylogenetic statistics (which is why Phylotrackpy lets you choose whether you want to).

![An example of a full digital evolution phylogeny](images/FullPhylogeny.png)

The above image represents an actual phylogeny measured from digital evolution. Each rectangle represents a different taxon. It's position along the x axis represents the span of time it existed for. Note that there are often sections along a single branch where multiple taxa coexisted for a period of time. Circles represent extant taxa at the end of this run.

# Glossary

Some useful terminology that might be useful in understanding the documentation (and especially the code base) for Phylotrackpy, particularly in light of the fact that different sub-fields of evolutionary biology tend to use different words in many of these contexts.

- **Taxon**: a generic word for a specific taxonomic unit. We use "taxon" as a generic term to represent a node in a phylogeny. For example, species are a common type of taxon to use when depicting portions of the phylogeny of life on earth. However, sometimes people choose to use higher-order types of taxa (e.g. genus, family, order, class, etc.) when they are trying to depict a larger swath of the whole phylogeny.
- **Taxa**: Plural of taxon.
- **Multifurcation/polytomy**: A node in a phylogeny that has more than two child nodes
- **Bifurcation**: A node in a phylogeny that has exactly two child nodes.
- **Non-branch node**: A node in a phylogeny with only one child node.
- **Leaf node**: A node in a phylogeny with no children.
- **Most Recent Common Ancestor (MRCA)**: The most recent node in a phylogeny that is a common ancestor of all nodes associated with extant taxa. If the phylogeny is pruned, there won't be any branch points before the MRCA (because any branches not leading to the MRCA would lead to taxa that are now extinct).
- **Coalescence events**: Occur when the most recent common ancestor changes (i.e. all descendants from one side of the deepest branch of the phylogeny have gone extinct). In the absence of diversity-preserving features coalescence events are expected to occur by chance with a frequency dependent on population size and spatial structure (but be careful of distributional assumptions). Observing coalescence less frequently than you would expect by chance can be an indication that ecological interactions are present (we have discussed this more [here](https://direct.mit.edu/artl/article/26/1/58/93272/Interpreting-the-Tape-of-Life-Ancestry-Based) and [here](https://direct.mit.edu/artl/article/25/1/50/2915/The-MODES-Toolbox-Measurements-of-Open-Ended)).
