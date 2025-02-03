#pragma once

#include <fstream>
#include <iostream>
#include <ios>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <random>
#include <stdlib.h>
#include <mutex>
#include <concurrent_queue.h>
#include <functional>
#include <queue>

#include <json.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <ImGuizmo.h>




#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>

#include <filesystem>


//TODO override malloc of stb image, track where images live in memory :)
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

#include <tiny_gltf.h>

#include <entt/entt.hpp>

#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_common.h>
#undef ERROR

#include "ISubSystem.h"
#include <PerformanceAPI.h>

#include <LaunchParameters.h>
#include "Profiler.h"

#include "VectorHash.h"

#include "EnttEditor/MetaReflect.h"
#include "EnttEditor/Include.hpp"
#include "EnttEditor/MetaContext.h"

#include "EnttEditor/Header/MetaInspectors/MetaInspectors.h"
#include "EnttEditor/MetaDefines.h"
#include "EnttEditor/MetaReflectImplement.h"

#include "JobSystem/PFiber.h"
#include "TypeDefs.h"
#include "ScopedLock.h"
#include "ImHexGenerator.h"

#include "Window.h"
#include "Platform.h"

#include "Easings.h"
#include "FileIO.h"
#include "Logger/LoggerSystem.h"
#include "Log.h"
#include "KeyCodes.h"
#include "Input.h"

#include "ECS/Serializer.h"

#include "Utilities.h"

#include "ResourceManager/IResourceType.h"
#include "ResourceManager/IResourceType.h"
#include "ResourceManager/Resource.h"
#include "ResourceManager/ResourceManager.h"