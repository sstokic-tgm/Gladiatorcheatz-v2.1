#pragma once

#include "../Singleton.hpp"

class ConfigFile;

class Config : public Singleton<Config>
{

public:

	void CreateConfigFolder(std::string path);
	bool FileExists(std::string file);

	void SaveConfig(const std::string path);
	void LoadConfig(const std::string path);
	
	std::vector<std::string> GetAllConfigs();

private:

	std::vector<ConfigFile> GetAllConfigsInFolder(const std::string path, const std::string ext);

	template<typename T>
	void Load(T &value, std::string str);

	void LoadArray(float_t value[4], std::string str);
	void LoadArray(bool value[14], std::string str);

	template<typename T>
	void Save(T &value, std::string str);

	void SaveArray(float_t value[4], std::string str);
	void SaveArray(bool value[14], std::string str);
};

class ConfigFile
{

public:

	ConfigFile(const std::string name, const std::string path)
	{
		this->name = name;
		this->path = path;
	}

	std::string GetName() { return name; };
	std::string GetPath() { return path; };

private:

	std::string name, path;
};