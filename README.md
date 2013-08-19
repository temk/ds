# DS library

The library intended for serialization/deserialization of large massive of primitives.
The data organized as a set of named columns, which allows store a heretogenious-type matrices on the disk.

The main purpose of the library, is to be replacement for CSV format, which is extremely slow. 

Supported types are: booleans, integers, floating point numbers and strings. 
In order to serialize string, the dictionary is used. So, it is quite efficient when the column of large length, contains actually small number of different strings.

Also:
1. data stored in binary format
2. byte order swap applyed only when it need. 
3. each column saved in separated file, which allows efficiently add new columns and new rows as well.
4. converters between ds format and csv.

Supported languages:
1. C/C++
2. Java
3. Matlab
4. will added more


## building

 1. edit cmake.config file
 2. mkdir build
 3. cd build
 4. cmake .. 
 5. make 
 6. make install

### matlab wrapper
from matlab, enter to mex directory and run compile.m

## License
AS IS, free for non-commertial using.


