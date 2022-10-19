#include "ProjectSerializer.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Core/Scene.h"
#include "Core/Entity.h"
#include "Core/Components.h"
#include "Core/Application.h"
#include "Core/Logger.h"

namespace Snow {
    static void SerializeScene(Scene* scene, nlohmann::json& json) {
        json["name"] = scene->Name();
        json["isActive"] = scene == Application::GetInstance()->CurrentProject()->ActiveScene();
        
        nlohmann::json entities{ nlohmann::json::array() };
        scene->ExecuteForEachEntity([&](Entity e) {
            nlohmann::json entityJson;
            entityJson["id"] = e.Id();

            if (auto* comp = e.TryGetComponent<TransformComponent>(); comp) {
                nlohmann::json transformJson;
                transformJson["position"] = { comp->Position.x, comp->Position.y, comp->Position.z };
                transformJson["rotation"] = { comp->Rotation.x, comp->Rotation.y, comp->Rotation.z };
                transformJson["scale"] = { comp->Scale.x, comp->Scale.y, comp->Scale.z };
                entityJson["transform"] = transformJson;
            }
            if (auto* comp = e.TryGetComponent<TagComponent>(); comp) {
                nlohmann::json transformJson;
                transformJson["name"] = comp->Tag;
                entityJson["tag"] = transformJson;
            }
            
            entities.push_back(entityJson);
        });
        json["entities"] = entities;
    }

    static void DeserializeScene(Project* project, const nlohmann::json& json) {
        Scene* scene = new Scene();
        scene->SetName(json["name"]);
        
        for (auto& entityJson : json["entities"]) {
            Entity e = scene->CreateOrGetEntity(entityJson["id"]);

            if (entityJson.find("transform") != entityJson.end()) {
                auto& transformJson = entityJson["transform"];
                auto& transform = e.GetComponent<TransformComponent>();
                
                transform.Position = { transformJson["position"][0], transformJson["position"][1], transformJson["position"][2] };
                transform.Rotation = { transformJson["rotation"][0], transformJson["rotation"][1], transformJson["rotation"][2] };
                transform.Scale = { transformJson["scale"][0], transformJson["scale"][1], transformJson["scale"][2] };
            }
            
            if (entityJson.find("tag") != entityJson.end()) {
                auto& tagJson = entityJson["tag"];
                auto& tag = e.GetComponent<TagComponent>();

				tag.Tag = tagJson["name"];
            }
        }
        
        project->AddScene(scene);
        if (json["isActive"]) {
            project->SetActiveScene(scene);
        }
    }

    void ProjectSerializer::Serialize(Project& project) {
        if (!std::filesystem::exists(project.RootFolder())) {
            std::filesystem::create_directory(project.RootFolder());
        }

        nlohmann::json json;
        json["project"]["name"] = project.Name();
        json["project"]["root"] = project.RootFolder().string();
        
        nlohmann::json scenes{ nlohmann::json::array() };
        for (auto* scene : project.Scenes()) {
            nlohmann::json sceneJson;
            SerializeScene(scene, sceneJson);
            scenes.push_back(sceneJson);
        }
        json["project"]["scenes"] = scenes;

        std::ofstream file{ project.RootFolder() / (project.Name() + ".snow") };
        file << json.dump(4);

        file.close();
    }

    Project* ProjectSerializer::Deserialize(const std::filesystem::path& path) {
        std::ifstream file{ path };
        if (!file.is_open()) {
            LOG_ERROR("Failed to open project file: {}", path.string());
            return nullptr;
        }

        nlohmann::json json{ nlohmann::json::parse(file) };
        file.close();

        Project* project = new Project{ json["project"]["name"], json["project"]["root"] };

        for (auto& sceneJson : json["project"]["scenes"]) {
           DeserializeScene(project, sceneJson);
        }
        
        return project;
    }
}