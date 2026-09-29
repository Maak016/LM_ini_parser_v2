# LM_ini_parser_v2
simple homemade ini parser written in C++

# How LM_ini_parser works
On instantiation of LM_ini_parser, all the data from the ini file will be read and processed. The result is that any procession that can prove heavy is only done once.
 * LM_ini_parser reads the raw data of the ini file through std::ifstream
 * The raw data is broken down into parts defining the sections, which will be fed to ini_section class
 * ini_section class breaks down the per-section raw data into parts which define key-value pairs, which are fed into ini_key class

# Intergration
Step 1: Instantiate an LM_ini_parser object<br>
Step 2: 
  * To Read file: use LM_ini_parser::get() to Read a key from the ini file. The first argument is what section the key is in, the second what the key name is.
  * To Write to file: WIP<br>
For more information see src\impl_example.cpp
