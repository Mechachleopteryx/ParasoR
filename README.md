# ParasoR

<img src="https://sites.google.com/site/cawatchm/software/parasor/logo.png" width="400">

## Features 
ParasoR can compute these features for RNA sequences even if they are longer than human genome sequences.

* Base pairing probability (bpp)
* Stem probability
* Accessibility
* RNA profile (probability and motif sequence)
* Local γ-centroid structure (in not parallel) or structures with base pairs (bpp >= 1/(1+γ)) with the color code of stem probability.

<img src="https://sites.google.com/site/cawatchm/software/parasor/stem.png" width="400">
* Local γ-centroid structure with the color code of RNA profile.
	* Color: Exterior (light green), Stem (red), Bulge (orange), Multibranch (green), Hairpin (violet), Internal (blue).

<img src="https://sites.google.com/site/cawatchm/software/parasor/prof.png" width="450">

In addition, ParasoR simulates structure arrangements caused by a single point mutation.

## Requirements

* c++11

We already tested ParasoR running with Apple LLVM version 6.0, GCC 4.5.3, and 4.8.1.

## How to install

```
git clone https://github.com/carushi/ParasoR
cd ParasoR/src
make
# make install (optional)
```

Or download from "Download ZIP" button and unzip it.

```
cd ParasoR-master/src
make
# make install (optional)
```

As a default, 'std=c++11' and 'double' is valid.

You can also change an option for c++11 and precision of floating point like

```
make C++11=c++0x VAR=LONG
# use c++0x and long double.
make C++11=c++11 VAR=SHORT
# use c++11 and float.
```
from a default, where 'std=c++11' option with 'double' precision.

## Example
We prepare a shell script for test run in 'check.sh'.
This script runs

```
make test
cat ../doc/pre.txt
# stem probability based on previous algorithm (Rfold model)
cat ../doc/stem.txt
# stem probability based on ParasoR algorithm
```

For more sample, please type `./ParasoR --help`.


ParasoR needs directories for temporal files which are designated in advance such as outer/ and prob/. In these directories, there is a possibility that ParasoR removes unrelated files. Therefore, we would recommend that you not place any other files under the ParasoR directory.
If you change the path of ParasoR directory, please recompile ParasoR.

## Reference

###Algorithm

* Kiryu H. et al. (2008) Rfold: an exact algorithm for computing local base pairing probabilities. Bioinformatics., 24 (3), 367–373.
* Hamada M. et al. (2009) Prediction of RNA secondary structure using generalized centroid estimators. Bioinformatics., 25 (4), 465-473.
* Kiryu H. et al. (2011) A detailed investigation of accessibilities around target sites of siRNAs and miRNAs. Bioinformatics., 27 (13), 1789-97.
* Fukunaga T. et al. (2014) CapR: revealing structural specificities of RNA-binding protein target recognition using CLIP-seq data. Genome Biol., 15 (1), R16.


###Implementation

* Hamada M. et al. (2009) Prediction of RNA secondary structure using generalized centroid estimators. Bioinformatics., 25(4), 465–473.
* Gruber AR. et al. (2008) The Vienna RNA websuite. Nucleic Acids Res., 36 (Web Server issue), W70–W74.

###Energy model

* Turner DH. et al. (2010) NNDB: the nearest neighbour parameter database for predicting stability of nucleic acid secondary structure. Nucleic Acids Res., 38(Database issue), D280–D282.
* Andronescu M. et al. (2010) Computational approaches for RNA energy
parameter estimation. RNA., 16(12), 2304–2318.
