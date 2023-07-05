#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"

ResourceManager::ResourceManager(const std::string& executablePath) 
{
	size_t found = executablePath.find_last_of("/\\");
	m_path = executablePath.substr(0, found);
}