# Mandelbrot Benchmark
## Lightweight cli tool for benchmarking cpu [now with gui]
Its just a lightweight cli tool that i made while upgrading my CPU to test how much of an upgrade it was.  

GUI works but still needs work  
Presets in GUI doesnt work yet  


## Installation
### Requirements:  
To automatically install required packages run requirements/install.py  
```bash
python3 requirements/install.py
```
If you want to install manually here is list of requirements:  
  - make  
  - libboost-all-dev [may be named differently on different distros]  
  - g++  
<br/>

````md
To install manually you can use this command (works on Debian-based distros):

```bash
sudo apt install make g++ libboost-all-dev
```

**To use the GUI, you will also need Python with the CustomTkinter library installed.**


### Compiling CLI [gui version also requires it]
To compile use make command in main folder.  

## Usage
CLI usage:  
```bash
./mandelbrot [c] [s] [i] [p] [ps]  
```
[c] - number of cores to use  
[s] - size of rendered image[square]  
[i] - max iteration number  
[p] - 1-generate preview 0-dont generate preview  
[ps] - size of preview  
