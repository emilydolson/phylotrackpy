# phylotrackpy: a python phylogeny tracker

[![Tests](https://github.com/emilydolson/python-phylogeny-tracker/actions/workflows/ci.yml/badge.svg)](https://github.com/emilydolson/python-phylogeny-tracker/actions/workflows/ci.yml)
[![Documentation Status](https://readthedocs.org/projects/phylotrackpy/badge/?version=latest)](https://phylotrackpy.readthedocs.io/en/latest/?badge=latest)
[![PyPi Package Version](https://img.shields.io/pypi/v/phylotrackpy.svg)](https://pypi.python.org/pypi/phylotrackpy)

In _in silico_ evolution experiments, we have the luxury of being able to perfectly track the phylogenies of our populations, rather than having to just infer them after the fact. Phylotrackpy is a Python package designed to help you do so as efficiently as possible.

## Features

- Ability to prune out taxa that are extinct and have no extant descendants (to keep memory use under control)
- Flexible control of how taxa are defined (e.g. by genotype, by phenotype, by trait, or by something more complex)
- Highly efficient (implemented in C++ under the hood)
- Includes various phylogenetic topology metrics

## How to use

There are three main steps in using phylotrackpy:

- Construct a `Systematics` object
- Notify the systematics object whenever anything is born
- Notify the systematics object whenever anyything dies

For more detailed instructions, see the [documentation](https://phylotrackpy.readthedocs.io/en/latest/)

## Installation

Phylotrackpy is available through pip:

```bash
pip install phylotrackpy
```
