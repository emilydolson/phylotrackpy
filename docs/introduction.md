# Introduction

Digital/computational/*in silico* evolution is a [powerful technique](https://www.frontiersin.org/articles/10.3389/fevo.2021.750779/full) for better understanding the process of evolution. One of the major benefits of doing *in silico* evolution experiments (instead of or in addition to laboratory or field experiments) is that they allow perfect measurement of quantities that can only be inferred in nature. Once such property is the precise phylogeny (i.e. ancestry tree) of the population.

![An example phylogeny](images/phylogeny.jpg)

At face value, measuring a phylogeny in *in silico* evolution may seem very straightforward: you just need to keep track of what gives birth to what. However, multiple aspects turn out to be non-trivial (see below). The goal of Phylotrackpy is to implement these things the right way once so that we all can stop needing to re-implement them over and over. Phylotrackpy is a python library designed to flexibly handle all aspects of recording phylogenies in *in silico* evolution.

Note: this library is essentially a wrapper around the [Empirical Systematics Manager](https://empirical--466.org.readthedocs.build/en/466/library/Evolve/systematics.html), which is implemented in C++. If you need a C++ phylogeny tracker, you can use that one directly (it is part of the larger Empirical library, which is header-only so you can just include the parts you want).
