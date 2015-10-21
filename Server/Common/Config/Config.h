#pragma once

#include <string>
#include <list>
#include <mutex>
#include <boost/property_tree/ptree.hpp>

class ConfigMgr
{
	ConfigMgr() { }
	~ConfigMgr() { }

public:
	/// Method used only for loading main configuration files
	bool LoadInitial(std::string const& file, std::string& error);

	static ConfigMgr* instance()
	{
		static ConfigMgr instance;
		return &instance;
	}

	bool Reload(std::string& error);

	std::string GetStringDefault(std::string const& name, const std::string& def);
	bool GetBoolDefault(std::string const& name, bool def);
	int GetIntDefault(std::string const& name, int def);
	float GetFloatDefault(std::string const& name, float def);

	std::string const& GetFilename();
	std::list<std::string> GetKeysByString(std::string const& name);

private:
	std::string _filename;
	boost::property_tree::ptree _config;
	std::mutex _configLock;

	ConfigMgr(ConfigMgr const&);
	ConfigMgr& operator=(ConfigMgr const&);
};

#define sConfigMgr ConfigMgr::instance()

