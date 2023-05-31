#pragma once
#include <fstream>
#include <iostream>
#include <optional>
#include "../wrapper/gltf_file_resource.h"
#include "../control/fly_through_camera.h"
#include "../control/orbit_camera.h"

enum class SceneRenderMode : int
{
    Gooch,
    Phong,
    PBR
};

enum class SceneCameraMode : int
{
    Orbit,
    FlyThrough
};

struct SceneSettings
{
    bool debugNormals = false;
    int width;
    int height;
    OrbitCamera orbitCamera;
    FlyThroughCamera flyThroughCamera;
    SceneCameraMode cameraMode = SceneCameraMode::Orbit;

    ICamera& getCamera()
    {
        switch (cameraMode)
        {
        case SceneCameraMode::Orbit:
            return orbitCamera;
        case SceneCameraMode::FlyThrough:
            return flyThroughCamera;
        }

        throw std::runtime_error("Invalid camera mode");
    }

    SceneRenderMode renderMode = SceneRenderMode::PBR;
	std::optional<std::string> loadedModelPath;
	std::optional<GltfFileResource> fileResource;
    glm::vec3 lightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(0.0f, 0.0f, 0.0f);

	void loadModel(const std::string& path)
	{
        try {
            glTF::File file = glTF::read_from_path(path);
 
            fileResource = GltfFileResource(file, path);
            loadedModelPath = path;
        }
        catch (const glTF::GLTFReadException& e)
        {
            std::cout << "could not load file: " << path << " with error: " << e.what() << std::endl;
            return;
        }
	}
};