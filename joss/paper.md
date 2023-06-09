---
title: 'the phylotrack suite: header-only C++ and Python tools for *in silico* phylogenetic tracking'
tags:
  - Python
  - C++
  - artificial life
  - digital evolution
  - phylogenetics
authors:
  - name: Emily Dolson
    orcid: 0000-0001-8616-4898
    affiliation: 1
  - name: Matthew Andres Moreno
    orcid: 0000-0003-4726-4479
    affiliation: 2
affiliations:
 - name: Michigan State University
   index: 1
 - name: University of Michigan
   index: 2
date: 10 June 2023
bibliography: paper.bib
---

# Summary

*In silico* evolution instantiates the evolutionary mechanisms of heredity, variation, and differential reproductive success within agent-based computational models.
Under these conditions, populations of virtual agents undergo evolution via natural selection and/or drift [@pennock2007models].
This experimental paradigm --- used across biological modeling, artificial life, and evolutionary computation --- provides distinct capabilities that complement research with natural model systems.
One key benefit is complete, exact observability.
For instance, compiling the full set of parent-child relationships over the history of a population yields complete, exact records of evolutionary lineage.
This information reveals sequences of events behind gain, loss, or maintenance of specific traits, but also has provides robust inferential power over general factors driving evolution and adaptation like ecology and selection pressure [@dolsonInterpretingTapeLife2020].

The phylotrack suite packages simulation lineage tracking and analysis capabilities.
The suite is composed of a header-only C++ library, developed under the umbrella of the Empirical project [@ofria2020empirical], and a Python wrapper around that library created with Pybind11 [@pybind11].
Both components supply a public-facing API to attach phylogenetic tracking to external digital evolution systems, as well as interfaces for stand-alone application of a variety of popular phylogenetic topology metrics [@tuckerGuidePhylogeneticMetrics2017].
Underlying algorithm and data structure engineering prioritizes efficiency to support large systems with rapid generational turnover.
The suite's underlying native implementation ensures fast, memory-efficient runtime, with multiple explicit features (e.g., lineage pruning and consolidation, etc.) included to reduce the memory footprint of phylogenetic information.

# Statement of Need

<!-- was going to say a rich history of sophisticated phylogenetic analyses... -->
*In silico* evolution work enjoys a rich history of phylogenetic record-keeping and analysis [@ray1992evolution].
However, to our knowledge, all pre-existing work has relied on bespoke system- or framework-specific phylogeny tracking implementations [@ofria2004avida;@bohm2017mabe;@de2012deap;@Garwood_REvoSim_Organism-level_simulation_2019].
In contrast, the Phylotrack suite seeks to supply ready-built tracking flexible enough to attach to any system modeling replicating entities.
To our knowledge, no such general-purpose library currently exists.

The hstrat and Automated Phylogeny Over Geological Timescales (APOGeT) libraries relate closely to, but do not entirely overlap, the Phylotrack suite's goals.
Both are also designed as general-purpose attachments to digital evolution systems, but they differ in the mode of phylogenetic tracking supplied.
Whereas Phylotrackpy uses a graph-based approach to perfectly record asexual phylogenies, the hstrat library implements hereditary stratigraphy, a recently developed method for approximate, inference-based approach for robust decentralized phylogenetic tracking in parallel and distributed systems [@moreno2022hstrat].
APOGeT, in turn, focuses on tracking speciation in sexually-reproducing populations [@godin2019apoget].

Vast amounts of [bioinformatics-oritented phylogenetics software](https://en.wikipedia.org/wiki/List_of_phylogenetics_software) is currently available.
These programs' purposes typically include
- inferring phylogenies from extant organisms (and sometimes fossils) [@challa2019phylogenetic],
- sampling phylogenies from theoretical models of population and species dynamics [@stadler2011simulating], and
- interoperating phylogeneies with other data (e.g., spatial species distributions) [@emerson2008phylogenetic],
- analyzing and manipulating tree structure [@smith2020treedist;@sand2014tqdist;@sukumaran2010dendropy;@cock2009biopython].

Phylotrack overlaps only in providing tree statistic implementations, under the rationale of facilitating fast during-simulation calculations.
The problem of tracking a phylogeny within an agent-based simulation differs substantially from bioinformatics-oriented tools' paradigm and purposes.
(Users new to working with recorded phylogenies should refer to Phylotrackpy documentation for [notes on subtle structural differences](https://phylotrackpy.readthedocs.io/en/latest/#useful-background-information) from more common reconstructed phylogenies.)
The Phylotrack suite has already facilitated several research projects.
Originally developed as part of the Empirical library [@ofria2020empirical], the C++ Phylotrack backend has been integrated into packages such as Modular Agent-Based Evolver (MABE) 2.0 [@bohm2019mabe], Symbulation [@vostinarSpatialStructureCan2019], and even a fork of the Avida digital evolution platform [@ofria2004avida;@dolsonInterpretingTapeLife2020].
These applications have facilitated research on open-ended evolution [@dolsonMODESToolboxMeasurements2019], the origin of endosymbiosis [@johnsonEndosymbiosisBustInfluence2022a], the importance of phylogenetic diversity for machine learning via evolutionary computation [@hernandez2022can;@shahbandegan2022untangling], and more.
The Python wrapper for Phylotrackpy is newer, but it has already served as a point of comparison in the development of other phylogenetic tools [@moreno2022hereditary;@moreno2023toward].

# Future Work

The primary limitation of Phylotrackpy at this point in time is the fact that it is limited to tracking asexual phylogenies (i.e. each taxon can have only one parent). In the future, we plan to extend Phylotrackpy to allow multiple parents per taxon.

# Acknowledgements

This research was supported in part by Michigan State University through the computational resources provided by the Institute for Cyber-Enabled Research.

# References

<div id="refs"></div>

\pagebreak
\appendix
