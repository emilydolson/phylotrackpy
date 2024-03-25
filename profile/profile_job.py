#!/usr/bin/env python3
import itertools as it
import os
import sys
import time

import numpy as np
import pandas as pd

np.random.seed(1)

from phylotrackpy import systematics


# CONFIGURE
##############################################################################
run_seconds, mutation_rate = 60, 0.2
print(f"{run_seconds=}, {mutation_rate=}")

batch_uuid, job_uuid = sys.argv[1:3]
print(f"{batch_uuid=}, {job_uuid=}")

replicate, pop_size = map(int, sys.argv[3:])
print(f"{replicate=}, {pop_size=}")


# SETUP
##############################################################################
population = np.random.uniform(0, 1, pop_size)

sys_ = systematics.Systematics(str)
taxa = np.array([sys_.add_org(x) for x in population])
assert sys_.get_num_active() == pop_size

start_time = time.time()

# EVOLVE
##############################################################################
for generation in it.count():
    # do selection
    selections = np.random.randint(0, pop_size, pop_size)
    next_population = population[selections]
    # do mutation
    mutation_mask = np.random.uniform(0, 1, pop_size) < mutation_rate
    next_population[mutation_mask] = np.random.uniform(
        0, 1, np.sum(mutation_mask)
    )

    # elapse generation
    next_taxa = np.array(
        [
            sys_.add_org(org, taxa[selection])
            for org, selection in zip(population, selections)
        ],
    )
    assert len(next_taxa) == pop_size
    for taxon in taxa:
        sys_.remove_org(taxon)

    taxa = next_taxa
    population = next_population

    if time.time() - start_time >= run_seconds:
        break

# SAVE
##############################################################################
batch_dir = f"batch={batch_uuid}+what=rawdata"
os.makedirs(batch_dir, exist_ok=True)

sys_.snapshot(f"{batch_dir}/a=systematics+job={job_uuid}+ext=.csv")
pd.DataFrame(
    {
        "replicate": [replicate],
        "generations elapsed": [generation],
        "seconds elapsed": [run_seconds],
        "generations per second": [generation / run_seconds],
        "population size": [pop_size],
        "job": [job_uuid],
        "batch": [batch_uuid],
    },
).to_csv(f"{batch_dir}/a=timeprof+job={job_uuid}+ext=.csv", index=False)
