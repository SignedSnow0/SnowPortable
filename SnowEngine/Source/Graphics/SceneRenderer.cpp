#include "SceneRenderer.h"
#include "Core/Utils.h"
#include "Core/Entity.h"
#include "Core/Components.h"
#include "Core/Input.h"
#include "Importers.h"

namespace Snow {
    struct SceneSettings {
        vec4f ambientColor = vec4f(1.0f);
    };

    SceneRenderer* SceneRenderer::sInstance{ nullptr };

    SceneRenderer::SceneRenderer(RenderTarget& target)
        : mCamera{ } {
        mShader = Importer::ImportShader({
            { Utils::GetShadersPath() / "default.vert", ShaderStage::Vertex },
            { Utils::GetShadersPath() / "default.frag", ShaderStage::Fragment }
        }, "Default");

        mRenderPass = RenderPass::Create({ RenderPassUsage::Image, target.GetSurface()->Width(), target.GetSurface()->Height(), target.GetSurface()->ImageCount(), nullptr, true });
        
        mPipeline = Pipeline::Create({ mShader, mRenderPass });

        mDescriptorSet = mPipeline->CreateDescriptorSet(0);

        sInstance = this;
    }

    SceneRenderer::~SceneRenderer() {
        sInstance = nullptr;

        delete mPipeline;
        delete mRenderPass;
    }

    RenderPass* SceneRenderer::OutputRenderPass() { return mRenderPass; }

    void SceneRenderer::SetScene(Scene* scene) { mScene = scene; }

    void SceneRenderer::Update() {
        if (!mScene) {
            return;
        }
        
        vec3f position = mCamera.mPosition;
        
        if (Input::KeyPressed(Key::W)) {
            position += mCamera.mFront * 0.01f;
        }
        
        if (Input::KeyPressed(Key::S)) {
            position -= mCamera.mFront * 0.01f;
        }
        
        if (Input::KeyPressed(Key::A)) {
            position -= glm::normalize(glm::cross(mCamera.mFront, vec3f(0.0f, 1.0f, 0.0f))) * 0.01f;
        }
        
        if (Input::KeyPressed(Key::D)) {
            position += glm::normalize(glm::cross(mCamera.mFront, vec3f(0.0f, 1.0f, 0.0f))) * 0.01f;
        }
		
        mCamera.SetPosition(position);        
        mCamera.SetAspectRatio(mRenderPass->Width() / mRenderPass->Height());
    }
    
    void SceneRenderer::Render() {
        if (!mScene) {
            return;
        }
        
        mRenderPass->Begin();

        mPipeline->Bind();

        static struct Camera{
            mat4f View;
            mat4f Projection;
        } camera;

        camera.View = mCamera.ViewMatrix();
        camera.Projection = mCamera.ProjectionMatrix();


        static SceneSettings settings{};
        settings.ambientColor = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
        mDescriptorSet->SetUniform("SceneSettings", &settings);
        mDescriptorSet->SetUniform("Camera", &camera);

        mDescriptorSet->Bind();

        mScene->ExecuteForEachEntity([this](Entity e) {
            TransformComponent& comp = e.GetComponent<TransformComponent>();
            mat4f model = comp.Transform();
            mPipeline->PushConstant("model", &model, sizeof model);

            if (auto* meshComp = e.TryGetComponent<MeshComponent>(); meshComp && meshComp->Mesh) {
                meshComp->Mesh->Draw();
            }
        });

        mRenderPass->End();
    }

    Pipeline* SceneRenderer::DefaultPipeline() { return sInstance->mPipeline; }
}