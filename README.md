# Data Storage (DS) library


The library is intended to serialize and de-serialize the large massive of primitives.
The data is organized as a set of named columns that allows to store heterogeneous-type matrices on the disk.
The main purpose of the library is to replace CSV format which is extremely slow.


Supported data types are: Booleans, integers, floating point numbers and strings.

In order to serialize the string the dictionary is used.
So, it is quite efficient when the column of large length contains small number of different strings.


## Also:
 1. Data is stored in a binary format
 2. Byte order swap is applied only when it is needed
 3. Each column is saved in a separate file. This allows to add efficiently new columns and new rows
 4. It supports conversion between ds and csv formats

## Supported languages:

 1. C/C++
 2. Java
 3. Matlab
 4. More will be added


## Building

 1. edit cmake.config file
 2. mkdir build
 3. cd build
 4. cmake .. 
 5. make 
 6. make install

### matlab wrapper
Please enter mex directory from Matlab and then run compile.m

## License
AS IS, FREE for Non-Commercial usage


## Site and documentations
http://ds.temk.org/


