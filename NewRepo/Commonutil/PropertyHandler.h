#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <deque>
#include <map>
#include <string>

typedef std::pair<std::string, std::string> Property;
typedef std::deque<std::string> Strings;

class Properties
{
protected:
	std::map<std::string, std::string> _properties;

public:
	bool GetProperty(std::string key, std::string& value);
	bool GetProperty(std::string key, int& value);
	bool PutProperty(std::string key, std::string value, bool force = true, bool ignorecomment = true);
	void PrintAllProperties();
};

class File
{
private:
	std::string filename;
	std::string lastread;
	std::ifstream filestream;
	char delimiter;
	int linesread;
	Strings lines;

public:
	File(std::string filename);
	void SetFileDelimiter(char delimiter);
	std::istream& ReadLine(std::string& line);
	Strings ReadAllLines(bool newline = false);
};

class PropertyStream :
	public Properties
{
public:
	PropertyStream(std::string filename, std::string delimiter = "=");
	PropertyStream(std::istringstream& stream, std::string delimiter = "=");
};
