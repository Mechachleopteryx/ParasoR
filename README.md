# ParasoR

<img src="https://lh3.googleusercontent.com/NPBIhLdufi8ps62htyBJ1mVCoYCGHThLlV6KwW7BCm9ydXBR3cT7vGjLHdr7YujFO-LbXk3BjAdMlEXh4eVjp0v9AzlRSaSTbhMpcOjruToAcehE_CM=w371" width="400">

## Features 
ParasoR can compute these features for RNA sequences even if they are longer than human genome sequences with computer clusters.

* Base pairing probability (bpp)
* Stem probability
* Accessibility
* Structure profiles (probability and motif sequence)
* γ-centroid structure (in not parallel) or credible structures having base pairs (bpp >= 1/(1+γ)) with the color code of stem probability.

<img src="https://lh6.googleusercontent.com/RDO_jDvrtOhdqJxNz5UB-rnP6Eky4ag3HfXGaym4af2sKnG2xrrnoTGsOaKGG8nSCS69DSFwZt7q4tCa12p2XBIlnjaFdlJYyVg0-hkAhxo08IDrmQk=w371" width="400">
* γ-centroid structure or credible structures with the color code of structure profiles.
	* Color: Exterior (light green), Stem (red), Bulge (orange), Multibranch (green), Hairpin (violet), Internal (blue).

<img src="https://lh4.googleusercontent.com/pN0sg041nLIOJKp1_8q8_nbVF9y324i2KvdFnR5PISSWvJ5QJvyaz7y11gbVms2axKysB62H5Bb5uSP9MOiOJcCJtexSpEcenDWGRAsuvWwcuY5EDBA=w371" width="450">

In addition, ParasoR simulates structure arrangements caused by a single point mutation.

## Requirements

* C++11

We already tested ParasoR running with Apple LLVM version 6.0 and GCC 4.8.1.

## How to install

```
git clone https://github.com/carushi/ParasoR
cd ParasoR
./configure
make
make install
```

Or download from "Download ZIP" button and unzip it.

As a default, 'double' option is valid for the precision of floating point.
You can also change an option for the precision of floating point like

```
make VAR=LONG
# use long double.
make VAR=SHORT
# use float.
```

If you have a trouble about automake setting, please try to type as below.

```
cd src
make -f _Makefile
```
or

```
 autoreconf -ivf
```

## Example
We prepared a shell script 'check.sh' for test run.
This script runs by the commands as follows.

```
cd script/
sh check.sh
cat ../doc/pre.txt
# stem probability based on previous algorithm (Rfold model)
cat ../doc/stem.txt
# stem probability based on ParasoR algorithm
python test.py
# Output numerical error between the result of ParasoR with single core and multiple core
```

For more sample, please visit our <a href="https://github.com/carushi/ParasoR/wiki">wiki</a>.

## Reference

### Citation
* Kawaguchi R. et al. (2016) Parallel computation of genome-scale RNA secondary structure to detect structural constraints on human genome. BMC Bioinformatics, 17:203.  

### Algorithm
* Kiryu H. et al. (2008) Rfold: an exact algorithm for computing local base pairing probabilities. Bioinformatics, 24 (3), 367–373.
* Hamada M. et al. (2009) Prediction of RNA secondary structure using generalized centroid estimators. Bioinformatics, 25 (4), 465-473.
* Kiryu H. et al. (2011) A detailed investigation of accessibilities around target sites of siRNAs and miRNAs. Bioinformatics, 27 (13), 1789-97.
* Fukunaga T. et al. (2014) CapR: revealing structural specificities of RNA-binding protein target recognition using CLIP-seq data. Genome Biol., 15 (1), R16.


### Implementation

* Hamada M. et al. (2009) Prediction of RNA secondary structure using generalized centroid estimators. Bioinformatics, 25(4), 465–473.
* Gruber AR. et al. (2008) The Vienna RNA websuite. Nucleic Acids Res., 36 (Web Server issue), W70–W74.

### Energy model

* Turner DH. et al. (2010) NNDB: the nearest neighbour parameter database for predicting stability of nucleic acid secondary structure. Nucleic Acids Res., 38(Database issue), D280–D282.
* Andronescu M. et al. (2010) Computational approaches for RNA energy parameter estimation. RNA, 16(12), 2304–2318.
