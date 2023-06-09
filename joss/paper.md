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

*In silico* evolution is a powerful paradigm in which a population of individuals is directly represented in a computer program. These individuals are allowed to reproduce and mutate, and are often subjected to selective pressures leading to differential reproductive success. Consequently, they undergo evolution via natural selection and/or drift. This paradigm is used across biological modeling, artificial life, and evolutionary computation. One of the benefits of this approach is that it provides the researcher with perfect knowledge of what happened over the entire history of evolution. A powerful tool for tracking this information is recording the phylogeny of the population (i.e. the full set of parent-child relationships within the population) [@dolsonInterpretingTapeLife2020].

Phylotrackpy is a library for tracking digital phylogenies, available both as a C++ library and as a Python wrapper around that library (created via Pybind11 [@pybind11]). It is designed to be easily added to pre-existing pieces of software, or used for stand-alone phylogenetic analysis. As phylogeny-tracking can be computationally-expensive, Phylotrackpy is written with an eye towards efficiency and has multiple features designed to reduce the memory footprint of phylogenetic information. In addition to its support for recording phylogenies, Phylotrackpy also supports calculating a variety of popular phylogenetic topology metrics [@tuckerGuidePhylogeneticMetrics2017].

# Statement of Need

While multiple computational evolution software packages include system-specific phylogeny tracking [ofria2004avida;@bohm2017mabe;@de2012deap;@Garwood_REvoSim_Organism-level_simulation_2019], to our knowledge no other general-purpose perfect phylogeny tracking library exists. The two most closely related tools are heriditary stratigraphy (hstrat) [@moreno2022hstrat] and Automated Phylogeny Over Geological Timescales (APOGeT) [@godin2019apoget]. Both of these tools are designed to plug in to computational evolution software and record phylogenetic information, however they both fill slightly more advanced roles than Phylotrackpy. Hstrat sacrifices maintaining perfect information to improve scalability, while the emphasis of APOGeT is on tracking speciation in sexually-reproducing populations. In contrast, the role of Phylotrackpy is simply to perfectly record asexual phylogenies.

It is important to note that the problem of tracking a phylogeny in real time is different than the problem of phylogenetic reconstruction traditionally faced by biologists. Essentially all bioinformatics tools related to phylogenies are focused on inferring phylogenies from extant organisms (and sometimes fossils) and working with these reconstructed phylogenies. Recorded phylogenies have numerous subtle differences from reconstructed phylogenies; users working with recorded phylogenies for the first time should refer to [Phylotrackpy documentation on the subject](https://phylotrackpy.readthedocs.io/en/latest/#useful-background-information).

Phylotrackpy has already facilitated a variety of research projects. Originally developed as part of the Empirical library [@ofria2020empirical], the C++ backend has been integrated into packages such as Modular Agent-Based Evolver (MABE) 2.0 [@bohm2019mabe], Symbulation [@vostinarSpatialStructureCan2019;@johnsonEndosymbiosisBustInfluence2022a], and even a fork of the Avida digital evolution platform [@ofria2004avida;@dolsonInterpretingTapeLife2020]. It has facilitated research on open-ended evolution [@dolsonMODESToolboxMeasurements2019], the origin of endosymbiosis [@johnsonEndosymbiosisBustInfluence2022a], the importance of phylogenetic diversity for machine learning via evolutionary computation [@hernandez2022can;@shahbandegan2022untangling], and more. The Python wrapper for Phylotrackpy is newer, but it has already served as a point of comparison in the development of other phylogenetic tools [@moreno2022hereditary;@moreno2023toward].

To facilitate interoperability with other phylogeny tools, Phylotrackpy outputs data in the Artificial Life Standard Phylogeny format [@lalejiniDataStandardsArtificial2019]. An ecosystem of converters exists between this format and other commonly used phylogeny data formats.


# Future Work

The primary limitation of Phylotrackpy at this point in time is the fact that it is limited to tracking asexual phylogenies (i.e. each taxon can have only one parent). In the future, we plan to extend Phylotrackpy to allow multiple parents per taxon.

# Acknowledgements

This research was supported in part by Michigan State University through the computational resources provided by the Institute for Cyber-Enabled Research.

# References

<div id="refs"></div>

\pagebreak
\appendix
