# Project - Antagonistic Lexicase Selection

This project explores the use of lexicase selection as a framework for
antagonistic coevolution.

**Navigation**

<!-- TOC -->

- [Case study problems](#case-study-problems)
  - [Minimal Sorting Networks](#minimal-sorting-networks)
    - [Implementation issues/decisions to make](#implementation-issuesdecisions-to-make)
    - [Experimental design](#experimental-design)
- [Stretch goals](#stretch-goals)
- [References](#references)

<!-- /TOC -->

## Case study problems

- Minimal sorting networks
  - (Hillis, 1990) - original application of antagonistic co-evolution to optimization
    problems.
- Genetic improvement - fixing bugs in software
  - (Arcuri and Yao, 2008) - original application of co-evolving test cases with
    programs in context of GI
- Program synthesis benchmarks
  - (Helmuth et al., 2015) - set of program synthesis benchmarks on which lexicase
    selection has been applied extensively

### Minimal Sorting Networks

Loose replication of (Hillis, 1990) experiment.

Original work has a one-one parasite-host interactions. Lexicase will have many-many
antagonistic interactions (host-parasite analogy doesn't hold up as well here).

Sorting networks - What are they?

- A sorting network is sorting algorithm in which the sequence of comparisons and
  exchanges of data take place in a predetermined order.
- Finding minimal sorting networks has practical applications (as of (Hillis, 1990);
  no idea about the current practical applications).
- A sorting network is defined by a sequence of compare-exchange operations where
  each operation is a pair of integers (i, j).
  - Compare-exchange operator - given (i, j), which is interpreted as i ==> j,
    if [j] < [i] => swap([i],[j])
- Sorting networks operate on (i.e., sort) sequences of fixed length (e.g., n=16).

(Hillis, 1990) Setup

- Evolving sorting networks:
  - Mutations
    - Changing the indices (i, j) of the exchange operators
    - Recombination by splicing the first part of one sorting network with the last
      part of another
  - Reproduction
    - Gamete pool is generated by crossover among pairs of chromosomes. For each
      chromosome pair in the (surviving) population, do single point (choose randomly)
      crossover.
      - Haploid gamete is produced by taking codons before the crossover point
        from the first member of each chromosome pair, and the codons after the
        crossover point from the second member.
      - Thus, there is exactly one crossover per chromosome pair per generation.
    - Point mutations are introduced to the gamete pool at a rate of one mutation
      per 1,000 sites. (i.e., per-site rate of 0.001).
    - Mate selection
      - Local mate selection, mating partners chosen to be nearby in the grid
      - x,y displacement of an individual from its mate is a binomial approximation
        of a gaussian distribution.
    - Mating
      - Mates exchange haploid gametes.
      - Mates are replaced by their offspring in the parents' location in grid
  - Genotype
    - Diploid
    - The genotype of each individual consists of 15 pairs of chromosomes, each
      consisting of 8 codons, representing the digits of 4 chromosome pairs
    - I.e., the genome of an individual is represented by 30 strings (15 pairs),
      each 32 bits
    - Each string (strand) has 4 genes (i,j pairs), each 8 bits long (i.e., 2 
      4-bit numbers per comparison operator).
    - There is one pair for each compare-exchange operator within the network.
      - Values given by pair give elements to be compared (i.e., i,j).
    - Individuals can have between 60 and 120 exchanges.
    - Phenotype generated from genome by traversing chromosomes of genotype in fixed
      order, reading off pairs as they appear in the phenotype.
      - If a pair of chromosomes is homozygous at a given position, then only
        a single pair is generated in the phenotype; if heterozygous, both pairs
        are generated, giving the 60 to 120 range of solutions.
  - Fitness
    - Score according to how well it sorts = percentage of input cases for which
      the network produces the correct output.
      - Offers partial credit for partial solutions.
      - Can be approximated by trying out the network on a random sample of test
        cases.
  - Selection
    - First, bottom 50% (fitness) of population is culled. Only top 50% of individuals
      allowed to contribute to gene pool of next generation.
- Evolving test cases
  - Three methods for limiting number of redundant test cases
    - varying test cases over time
    - varying test cases spatially
    - evolving test cases** => Most interesting results, the one discussed in paper
  - Coevolution
    - Both populations evolve on the same grid, and they interact via the fitness
      function (i.e., fitnesses are relative to antagonist).
    - Sorting networks are scored according to the test cases provided by the parasites
      at the same grid location.
    - Parasites are scored according to how well they find flaws in sorting networks.
  - Test case representation
    - Each parasite is a group of 10 to 20 test cases
  - Fitness
    - Score of a parasite (test case group) is the number of test cases that
      the corresponding sorting network fails

Project implementation

- Sorting networks
- Test cases
- Selection
  - Objectives
    - Parasites (test cases)
      - Maximize host failure (minimize host passes)
    - Hosts (sorting networks)
      - Maximize test passes/minimize failures
      - Minimize network size
  - Lexicase selection
    - Parasite (test cases) - host (sorting networks) groupings
      - Well-mixed cohorts
      - Local cohorts
      - Full pairwise
    - Parasites
      - 1 parasite = 1 test case (1 sequence of 0s/1s)
      - 1 parasite = 10-20 test cases (as in Hillis, 1990)

#### Implementation issues/decisions to make

- How should we promote small networks over large networks?
  - Add cost to size
  - Break ties by size
  - Add a size test case (smallest networks win)
  - Give bonus for small size (fitness += (1 - # operators)/(max number of operations)
    i.e., proportion of not-used operations)
  - Use a strange encoding that promotes small networks
- How do we want to setup lexicase selection? (see options described above)
- How do we want to encode sorting networks?
  - Don't *really* want to use the original encoding. Should nearby indices
    be closer together in genotype-space? 1111 and 0111 are very different numbers.
    Bit flips don't uniformly affect the phenotype. Do we care? Should we move
    to a direct encoding?

#### Experimental design

- Population structure
  - Well mixed
    - Varying cohort size
  - Local (original setup)
- Selection regimes
  - No antagonism
    - Original (cull 50%, use remaining to generate next population)
    - Tournament
    - Lexicase
  - Antagonism (co-evolution)
    - Original
    - Tournament
    - Lexicase

## Stretch goals

- [ ] Sorting network visualization

## References

Hillis, W. D. (1990). Co-evolving parasites improve simulated evolution
as an optimization procedure, 42, 228–234.

Arcuri, A., & Yao, X. (2008). A novel co-evolutionary approach to
automatic software bug fixing. 2008 IEEE Congress on Evolutionary Computation,
CEC 2008, 162–168. [https://doi.org/10.1109/CEC.2008.4630793](https://doi.org/10.1109/CEC.2008.4630793)

Helmuth, T., & Spector, L. (2015). General Program Synthesis Benchmark
Suite. In Proceedings of the 2015 on Genetic and Evolutionary Computation
Conference - GECCO ’15 (pp. 1039–1046). New York, New York, USA: ACM Press.
[https://doi.org/10.1145/2739480.2754769](https://doi.org/10.1145/2739480.2754769)
