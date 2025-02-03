#pragma once

namespace Perry
{
	class Mesh;
	class Model;
	struct Entity;

	class GLTFLoader
	{
		typedef void (*ExtensionFunc)(Perry::Entity& e, const tinygltf::Model&, const tinygltf::Value&);
		typedef void (*PropertyFunc)(Perry::Entity& e, const tinygltf::Model&, const Perry::Mesh*);
	public:
		//static void AddExtensionTemplate(const std::string& name, ExtensionFunc callback);
		static void AddProperty(const std::string& name, PropertyFunc callback);
		static void AddLight(Perry::Entity& e, const tinygltf::Model&, const tinygltf::Value&);

	private:
		static void AddShadow(Perry::Entity& e, const tinygltf::Model&, const Perry::Mesh*);

		friend class Model;
		static inline std::map<std::string, ExtensionFunc> m_EntityExtensions{
			{"KHR_lights_punctual", &AddLight}
			//{"Shadow", &AddShadow }
		};

		static inline std::map<std::string, PropertyFunc> m_EntityProperties{
			{"Shadow",&AddShadow}
		};
	};
}
