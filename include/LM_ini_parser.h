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
	ini_key(const string raw_line_data);
	
	void assign_key_value_pair(const string raw_line_data);
	void assign_key(int eq_sign_idx);
	void assign_val(int eq_sign_idx);

	bool is_valid_key();

	string get_key();
	string get_val();
};

class ini_section {
private:
	string section_name = "";
	string raw_section_data = "";

	vector<ini_key> keys_in_section = {};

public:
	ini_section(const string raw_section_data);

	void process_attributes(const string raw_section_data);

	void parse_section_name();
	void parse_keys_in_section();

	string get_ini_key_val(const string key_name);

	string get_section_name();
};

class LM_ini_parser {
private:
	string file_path = "";
	string raw_file_data = "";

	vector<ini_section> sections_in_file = {};

public:
	LM_ini_parser(filesystem::path file_path);
	void process_attributes(filesystem::path file_path);

	void parse_raw_file_data();
	void parse_sections_in_file();
	void iterate_single_section(int idx_in_raw_data);

	string get(const string section_name, const string key_name);
};

class util {
public:
	//read a string starting from ref index until finding target is found. Will also set ref_index so that the index at which the finding target is found can be had
	static string read_until_char(const string& input, char finding_target, int& ref_index);

	static bool read_until_newline(const string& input, string& output, int& ref_index);

	static bool is_whitespace(const char input);

	static string trim_whitespace(const string& input);
};