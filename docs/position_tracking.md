# Position based tracking

A phylogeny is essentially a collection of interconnected [`Taxon`](phylotrackpy.systematics.Taxon) objects. These objects mostly live inside the [`Systematics`](phylotrackpy.systematics.Systematics) object, which manages them. However, you will, in some cases, need access to the Taxon object associated with a particular organism. Most notably, any time an organism is born, you will need to provide its parent's Taxon. And time an organism dies, you will need to provide that organism's taxon. If your organisms are represented by classes, it is likely easiest to just store a reference to each organism's taxon in that organism (as shown in the [quickstart](quickstart) guide).

However, sometimes your organisms may be raw data (e.g. ints, strings, numpy arrays) or objects that you cannot modify for some reason. In these cases, keeping track of taxa can be a pain. To help, phylotrackpy can handle it for you using position-based tracking. This option assumes that the organism objects in your population are being stored in some manner where they each have a knowable location within a broader data structure(s) (e.g. they are all in a list). This position can be used to associate organisms with Taxon objects.

To use position-based tracking, we need to initialize a systematics object with it turned on, using the `store_pos` keyword argument:

```py
from phylotrackpy import systematics
syst = systematics.Systematics(store_pos = True)
```

For this example, we will assume our population is a list of integers. Let's imagine it starts out with just one number in it. We want to start out by adding that individual to the systematics manager with no parent:

```py
# The population 
population = [2]

# If we're tracking position, we add by position
# First argument is the organism, second is the position
# Note that there is no return value because the
# Taxon object is stored internally 
syst.add_org_by_position(population[0], 0)
```

Let's say we want to add three more individuals to the population with 2 as their parent:

```py
import random

for i in range(3):
    # Make new org with random variation from parent
    new_org = population[0] + random.randint(5)
    # Add new_org to the population
    population.append(new_org)
    position = len(population) - 1
    # Notify systematics manager about new_org
    # 2nd arg is org's position, 3rd is parent's position
    syst.add_org_by_position(new_org, position, 0)
```

Note that we didn't need to tell the systematics manager what the parent's Taxon object was. Instead, we just told it where in the population the parent is and the systematics manager looked up its Taxon for us.

We can also notify the systematics manager that an organism has died using its position:

```py
# Imagine the organism at position 1 dies
# Let's say we're using -1 to indicate "no organism"
population[1] = -1

# We could then notify the systematics manager like this:
syst.remove_org_by_position(1)

```

Importantly, if we're tracking by position, we also need to notify the systematics manager if an organism moves:

```py
# Let's say we want to fill in the gap at position 1 by moving the 
# organism at position 3
population[1] = population[3]
population.pop()

# We can notify the systematics manager with swap_positions
# Note that we can use swap_positions to move a single organism
# because we're swapping the organism with an empty spot
syst.swap_positions(1, 3)
```

## Handling multiple populations

In evolutionary computation, it is common to maintain two populations: the current generation and the next generation. In other contexts, you might have reason to maintain even more populations. Phylotrackpy supports these use cases.

So far, we've just been passing integers in to represent positions. Under the hood, though, they are getting converted to [`Worldposition`](phylotrackpy.systematics.WorldPosition) objects. `WorldPosition`s have two components: the index within the population, and the id of the population. When we only specify one number, it is assumed to be the index and the id is assumed to be 0. If we specify two numbers, however, the first will be the index and the second will be the id. 

Let's imagine a new scenario where we have two populations (a current generation and the next generation):

```py
pop_size = 5

# The populations 
curr_generation = [1, 2, 3, 1, 2]
# Assume -1 means there is no organism there
next_generation = [-1, -1, -1, -1, -1]

# Start by adding everything in the current generation
for i in range(pop_size):
    syst.add_org_by_position(curr_generation[i], i)

# Create new generation
for i in range(pop_size):
    # Choose random parent to reproduce
    parent = random.randint(pop_size)

    # Offspring is a mutated version of the parent
    new_org = curr_generation[parent] + random.randint(-1, 1)

    # Add the offspring to the systematics manager. Because
    # we're putting it in the next generation, we specify its
    # position as (i, 1), index i of population 1.
    # We could also have written this as systematics.WorldPosition(i, 1)
    syst.add_org_by_position(new_org, (i, 1), parent)

    # Add org to next generation
    next_generation[i] = new_org

# New generation is done being created. Everything in the current generation  
# dies and we swap in the next one
curr_generation = next_generation
for i in range(pop_size):
    # We're removing from population 0, which is the default
    sys.remove_org_by_position(i)
    # Swap the ith element of population 0 and the ith element of population 1
    syst.swap_positions((i,0), (i,1))

```