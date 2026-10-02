# LM_ini_parser_v2
simple homemade ini parser written + writer in C++. Where is v1, you may ask? I don't know:D

# How LM_ini_parser works
On instantiation of LM_ini_parser, all the data from the ini file will be read and processed. The result is that any procession that can prove heavy is only done once.
 * LM_ini_parser reads the raw data of the ini file through std::ifstream
 * The raw data is broken down into parts defining the sections, which will be fed to ini_section_reader class
 * ini_section class breaks down the per-section raw data into parts which define key-value pairs, which are fed into ini_key class

# How LM_ini_writer works
On instantiation of LM_ini_writer, a copy of the content of a read file is made. anything added afterwards will supplement the read file
 * On instantiation of LM_ini_writer, users can optionally specify a pointer to an LM_ini_parser object.
 * Data from the LM_ini_parser object will be adapted to LM_ini_writer, ini_section_reader objects will be adapted to ini_section_writer objects. Keys use the same class for both writers and readers
 * add() adds to the internal vector storing the ini_section_writer objects
 * In writing, parse_written_data is called in each ini_section_writer. and then parse_written_data in LM_ini_writer puts the written data parsed from each ini_section_writer together.

# Intergration
To include the library in your project. You only need include/LM_ini_parser.h and src/LM_ini_parser.cpp. include LM_ini_parser.h in your project and you will be up and running!

# How to use library
Step 1: Instantiate an LM_ini_parser object OR LM_ini_writer object<br>
Step 2: 
  * To Read file: use LM_ini_parser::get() to Read a key from the ini file. The first argument is what section the key is in, the second what the key name is.
  * To Write to file: use LM_ini_writer::add() to write value to a key. First, second and third argument are respectively: section_name, key_name, key_value
For more information see src\impl_example.cpp
