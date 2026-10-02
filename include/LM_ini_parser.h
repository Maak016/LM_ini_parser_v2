#pragma once

#include <iostream>

#include <vector>

#include <filesystem>
#include <fstream>
#include <sstream>

using std::string; using std::vector;
namespace filesystem = std::filesystem;

class ini_key {
private:
	string raw_line_data = "";

	string key_name		= "";
	string key_value	= "";

	bool   is_valid     = false;

	static int  get_equal_sign_index(const string raw_line_data);
	static bool is_valid_line(const string raw_line_data);
	
public:
	//Will be used for the argument of the constructor when a new key is created in ini_writer (where the key_name and keyvalue are known)
	static string convert_data_to_raw_line(const string key_name, const string key_value);

	ini_key(const string raw_line_data);
	
	void assign_key_value_pair(const string raw_line_data);
	void assign_key(int eq_sign_idx);
	void assign_val(int eq_sign_idx);

	bool is_valid_key();

	string get_key();
	string get_val();
};

class ini_section_reader {
private:
	string section_name = "";
	string raw_section_data = "";

	vector<ini_key> keys_in_section = {};

public:
	ini_section_reader(const string raw_section_data);

	void process_attributes(const string raw_section_data);

	void parse_section_name();
	void parse_keys_in_section();

	string get_ini_key_val(const string key_name);

	string get_section_name();
	vector<ini_key> get_keys_in_section();
};

class LM_ini_parser {
private:
	string file_path = "";
	string raw_file_data = "";

	vector<ini_section_reader> sections_in_file = {};

public:
	LM_ini_parser(filesystem::path file_path);
	void process_attributes(filesystem::path file_path);

	void parse_raw_file_data();
	void parse_sections_in_file();
	void iterate_single_section(int idx_in_raw_data);

	vector<ini_section_reader> get_read_sections();

	string get(const string section_name, const string key_name);
};

class ini_section_writer {
	string section_name = "";
	vector<ini_key> keys_in_section = {};

	string written_data = "";

	static string get_header_line(const string section_name);
	void append_key_to_written_data(string* buf, ini_key& key);
public:
	ini_section_writer(ini_section_reader* read_section);
	ini_section_writer(string new_section_name);

	string get_section_name();

	void adapt_from_read_section(ini_section_reader* read_section);

	void write_key(const string key_name, const string key_val);

	void parse_written_data();
	string get_written_data();

	int get_key_index(const string key_name);
};

class LM_ini_writer {
private:
	string file_path = "";

	vector<ini_section_writer> sections_in_file = {};

	string written_data = "";

	int get_section_index(const string section_name);
	void append_section_to_written_data(string* buf, ini_section_writer& writer);
public:
	LM_ini_writer(filesystem::path file_path, LM_ini_parser* read_file);

	void adapt_from_read_file(LM_ini_parser* read_file);

	void add(const string section_name, const string key_name, const string key_val);
	void add_section(const string section_name);
	void write_key(const int section_index, const string key_name, const string key_val);

	void parse_written_data();

	void write();
};

class util {
public:
	//read a string starting from ref index until finding target is found. Will also set ref_index so that the index at which the finding target is found can be had
	static string read_until_char(const string& input, char finding_target, int& ref_index);

	static bool read_until_newline(const string& input, string& output, int& ref_index);

	static bool is_whitespace(const char input);

	static string trim_whitespace(const string& input);

	static bool write_to_file(const string file_path, const string& write_data);

	static bool read_from_file(string* output, const string file_path);
};