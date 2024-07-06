# phylotrackpy: a python phylogeny tracker

[![Tests](https://github.com/emilydolson/python-phylogeny-tracker/actions/workflows/ci.yml/badge.svg)](https://github.com/emilydolson/python-phylogeny-tracker/actions/workflows/ci.yml)
[![Documentation Status](https://readthedocs.org/projects/phylotrackpy/badge/?version=latest)](https://phylotrackpy.readthedocs.io/en/latest/?badge=latest)
[![PyPi Package Version](https://img.shields.io/pypi/v/phylotrackpy.svg)](https://pypi.python.org/pypi/phylotrackpy)
![PyPI - Wheel](https://img.shields.io/pypi/wheel/phylotrackpy)
[![codecov](https://codecov.io/github/emilydolson/phylotrackpy/graph/badge.svg?token=ORZS21AXQM)](https://codecov.io/github/emilydolson/phylotrackpy)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10888780.svg)](https://doi.org/10.5281/zenodo.10888780)

<img src=https://github.com/emilydolson/phylotrackpy/raw/main/docs/images/phylogeny.jpg width="50%" height="50%">

In _in silico_ evolution experiments, we have the luxury of being able to perfectly track the phylogenies of our populations, rather than having to just infer them after the fact. Phylotrackpy is a Python package designed to help you do so as efficiently as possible.


At face value, measuring a phylogeny in *in silico* evolution may seem very straightforward: you just need to keep track of what gives birth to what. However, multiple aspects turn out to be non-trivial. The goal of Phylotrackpy is to implement these things the right way once so that we all can stop needing to re-implement them over and over. Phylotrackpy is a python library designed to flexibly handle all aspects of recording phylogenies in *in silico* evolution.

## Phylogeny Trackers in Other Languages

### C++

Phylotrackpy is essentially a wrapper around [Phylotracklib](https://empirical--466.org.readthedocs.build/en/466/library/Evolve/systematics.html), which is implemented in C++. If you need a C++ phylogeny tracker, you can use that one directly (it is part of the larger Empirical library, which is header-only so you can just include the parts you want).

### Julia

A phylogeny tracker [written in Julia](https://github.com/jarbus/PhylogeneticTrees.jl/tree/master) is also available.

## Features

- [**Pruning**](https://phylotrackpy.readthedocs.io/en/latest/features.html#pruning): Ability to prune out taxa that are extinct and have no extant descendants (to keep memory use under control)
- [**Flexible taxon definitions**](https://phylotrackpy.readthedocs.io/en/latest/features.html#flexible-taxon-definitions): Flexible control of how taxa are defined (e.g. by genotype, by phenotype, by trait, or by something more complex)
- [**Efficiency**](https://phylotrackpy.readthedocs.io/en/latest/features.html#efficiency): Highly efficient (implemented in C++ under the hood)
- [**Phylostatistics**](https://phylotrackpy.readthedocs.io/en/latest/features.html#phylostatistics-calculations): Includes various phylogenetic topology metrics
- [**Flexible output**](https://phylotrackpy.readthedocs.io/en/latest/features.html#flexible-output-options): Easily add columns to output files.

*Running a parallel/distributed simulation?
Check out [hstrat](https://github.com/mmore500/hstrat), which provides an alternate parallel/distributed-friendly methodology for decentralized phylogenetic tracking.*

## High level usage

There are three main steps to tracking a phylogeny using phylotrackpy:

- [Construct a `Systematics` object](https://phylotrackpy.readthedocs.io/en/latest/quickstart.html#creating-a-systematics-object)
- [Notify the systematics object whenever anything is born](https://phylotrackpy.readthedocs.io/en/latest/quickstart.html#notifying-the-systematics-object-of-births)
- [Notify the systematics object whenever anything dies](https://phylotrackpy.readthedocs.io/en/latest/quickstart.html#notifying-the-systematics-object-of-deaths)

You may also want to:

- [Import and export data](https://phylotrackpy.readthedocs.io/en/latest/quickstart.html#importing-and-exporting-data)

For more detailed instructions, see the [documentation](https://phylotrackpy.readthedocs.io/en/latest/)

## Installation

Phylotrackpy is available through pip:

```bash
pip install phylotrackpy
```

To install the latest development version:

```bash
pip install git+https://github.com/emilydolson/phylotrackpy
```

To install from a local sorce copy:
```bash
pip install . --upgrade
```

Note that development and local installs will require local compilation of C++ bindings.
Pre-built wheels are available with the PyPi distribution.
See [our documentation](https://phylotrackpy.readthedocs.io/en/latest/contributing.html) for more complete information on local builds.

## Useful background information

There are certain quirks associated with real-time phylogenies (especially digital ones) that you might not be used to thinking about if you're used to dealing with reconstructed phylogenies. Many of these discrepancies are the result of the very different temporal resolutions on which these types of phylogenies are measured, and the fact that the taxonomic units we work with are often at a finer resolution than species. We document some here so that they don't catch you off guard:

- **Multifurcations are real**: In phylogenetic reconstructions, there is usually an assumption that any multifurcation/polytomy (i.e. a node that has more than two child nodes) is an artifact of having insufficient data. In real-time phylogenies, however, we often observe multifurcations that we know for sure actually happened.
- **Not all extant taxa are leaf nodes**: In phylogenetic reconstructions, there is usually an assumption that all extant (i.e. still living) taxa are leaf nodes in the phylogeny (i.e. none of them are parents/offspring of each other; similar taxa are descended from a shared common ancestor). In real-time phylogenies it is entirely possible that one taxon gives birth to something that we have defined as a different taxon and then continues to coexist with that child taxon.
- **Not all nodes are branch points**: In phylogenetic reconstructions, we only attempt to infer where branch points (i.e. common ancestors of multiple taxa) occurred. We do not try to infer how many taxa existed on a line of descent between a branch point and an extant taxa. In real-time phylogenies we observe exactly how many taxa exist on this line of descent and we keep a record of them. In practice there are often a lot of them, depending on you define your taxa. It is unclear whether we should include these non-branching nodes when calculating phylogenetic statistics (which is why Phylotrackpy lets you choose whether you want to).

![An example of a full digital evolution phylogeny](docs/images/FullPhylogeny.png)

The above image represents an actual phylogeny measured from digital evolution. Each rectangle represents a different taxon. It's position along the x axis represents the span of time it existed for. Note that there are often sections along a single branch where multiple taxa coexisted for a period of time. Circles represent extant taxa at the end of this run.

## Dependencies

- [pybind11](https://pybind11.readthedocs.io/en/stable/) (for wrapping C++ code into Python)
- [Empirical](https://github.com/devosoft/Empirical) (where the C++ version of this code lives)

### Testing dependencies

- pytest

### Documentation dependencies

- myst_parser (for writing documentation in markdown)
- sphinx_rtd_theme (theme for [readthedocs](https://readthedocs.org/))

## Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md).

## Citing

If Phylotrack contributes to a scientific publication, please cite it as

> Dolson, E., Rodriguez-Papa, S., & Moreno, M. A. (2024). Phylotrack: C++ and Python libraries for in silico phylogenetic tracking. arXiv preprint arXiv:2405.09389. https://doi.org/10.48550/arXiv.2405.09389

```bibtex
@misc{dolson2024phylotrack,
      doi={10.48550/arXiv.2405.09389},
      url={https://arxiv.org/abs/2405.09389},
      title={Phylotrack: C++ and Python libraries for in silico phylogenetic tracking},
      author={Emily Dolson and Santiago Rodriguez-Papa and Matthew Andres Moreno},
      year={2024},
      eprint={2405.09389},
      archivePrefix={arXiv},
      primaryClass={q-bio.PE}
}
```

Consider also citing [pybind11](https://pybind11.readthedocs.io/en/stable/faq.html#how-to-cite-this-project) if you are using PhylotrackPy.
And don't forget to leave a [star on GitHub](https://github.com/emilydolson/phylotrackpy/stargazers)!

## Developers

- [Emily Dolson](https://emilyldolson.com) (lead developer)
- [Matthew Andres Moreno](https://mmore500.com/)
