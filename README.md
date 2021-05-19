Contributors:
  Anirudha Kulkarni (2019CS50421)
  Pratyush Saini (2019CS10444)

## Task1: Traffic density estimation using OpenCV functions

Clone the repo to run locally:

```
git clone https://github.com/anirudhakulkarni/COP290-A1.git

cd COP290-A1
```

### Directory Structure:

- `code` directory contains all the code required for running
- `analysis` directory contains all the postpocessing scripts and outputs
- `code/src` contains source files corresponding to each method `main.cpp`, `method_1.cpp`, `method_2.cpp`, `method_3.cpp`, `method_4.cpp`, `method_ec.cpp`
- `code/assets` folder contains video and images that need to be processed `trafficvideo.mp4`, `empty.jpg`, `traffic.jpg`
- `code/build` folder stores build file of c++ program and stackdumps in case of errors
- `analysis/outputs` folder stores output corresponding to each method with all parameteres and corresponding analyzer scripts

### Commands:

#### Main Program:

to build

```bash
$ make build
```

To run:

```bash
$ make run
```

To Plot:

```bash
$ make plot
```

To clean:

```bash
$ make clean
```

#### To build method:

```bash
$ make obuild METHOD=1
```

#### To build with pthreads:

```bash
$ make pbuild METHOD=3
```

Instead project can be re-run after making changes by

```bash
$ make all
```

#### Processing:

```bash
make optimize METHOD=1
```

This will iterate over all parameters in method given and store corresponding outputs in output/{methodname} directory.

Modify `parametersArray` in `iterate.py` for custom parameters on which you wish to run.
