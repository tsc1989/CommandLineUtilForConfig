# Command line utility to access or set configuration parameters of configuration file


This utility is written in C language with simple interface to open input file modify requested configuration parameters and save in the samefile


# Build

```
mkdir build
cd build
cmake ../
```

# Run application

```
./CliCfg -e <0/1> -f <Configuration file name>
```
-e option is to indicate if file is encrypted or not encrypted

-e 0 : Input file human redable plain text configuration file

-e 1: Input file is encrypted with AES256 algorithm with key stored in **key** file of this repository

