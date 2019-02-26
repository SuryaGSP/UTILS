#include <PropertyHandler.h>
#include <algorithm>
#include <cctype>

bool Properties::GetProperty(std::string key, std::string& value)
{
	if (_properties.count(key))
	{
		value = _properties[key];
		return true;
	}
	return false;
}

bool Properties::GetProperty(std::string key, int& value)
{
	if (_properties.count(key))
	{
		value = std::atol(_properties[key].c_str());
		return true;
	}
	return false;
}

bool Properties::PutProperty(std::string key, std::string value, bool force, bool ignorecomment)
{
	bool keyexists = _properties.count(key);
	value.erase(std::remove_if(value.begin(), value.end(), [](char x) { return std::isspace(x); }), value.end());
	if (key[0] == '#')
	{
		if (ignorecomment)
		{
			return false;
		}
	}
	if (value == "" || key == "")
	{
		return false;
	}
	if (force)
	{
		_properties.insert({key, value});
		if (keyexists)
		{
			return false;
		}
		return true;
	}
	if (keyexists)
	{
		return false;
	}
	_properties.insert({key, value});
	return true;
}

void Properties::PrintAllProperties()
{
	for (auto& property : _properties)
	{
		std::cout << property.first << " : " << property.second << std::endl;
	}
}


File::File(std::string filename):
	filename(filename),
	delimiter('\n'),
	linesread(0)
{
	filestream.open(filename);
	if (!filestream.is_open())
	{
		std::cout << filename << " not opened" << std::endl;
	}
}

void File::SetFileDelimiter(char delimiter)
{
	this->delimiter = delimiter;
}

std::istream& File::ReadLine(std::string& line)
{
	return std::getline(filestream, line, delimiter);
}

Strings File::ReadAllLines(bool newline)
{
	if (filestream.is_open())
	{
		++linesread;
		std::string line;
		while (ReadLine(line))
		{
			if (newline)
			{
				line.append("\n");
			}
			lines.push_back(line);
		}
	}

	return lines;
}

PropertyStream::PropertyStream(std::string filename, std::string delimiter)
{
	File propertyfile(filename);
	std::string confPair;
	while (propertyfile.ReadLine(confPair))
	{
		auto pos = confPair.find(delimiter);
		if (pos != std::string::npos)
		{
			auto key = confPair.substr(0, pos);
			auto value = confPair.substr(pos + 1);
			PutProperty(key, value);
		}
	}
}

PropertyStream::PropertyStream(std::istringstream& stream, std::string delimiter)
{
	std::string confPair;
	while (getline(stream, confPair, '\n'))
	{
		auto pos = confPair.find(delimiter);
		if (pos != std::string::npos)
		{
			auto key = confPair.substr(0, pos);
			auto value = confPair.substr(pos + 1);
			PutProperty(key, value);
		}
	}
}

/*
#ifdef TEST
int main()
{
    auto properties = PropertyStream("properties.ini");
    properties.PrintAllProperties();
    return 0;
}
#endif
*/
