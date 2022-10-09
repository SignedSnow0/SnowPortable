#include "SceneRenderer.h"
#include "Core/Utils.h"
#include "Core/Entity.h"
#include "Core/Components.h"
#include "Core/Input.h"

namespace Snow {
    SceneRenderer::SceneRenderer(Scene* scene, RenderTarget& target)
        : mScene{ scene }, mCamera{ } {
        mShader = Shader::Create({
            { Utils::GetShadersPath() / "default.vert", ShaderStage::Vertex },
            { Utils::GetShadersPath() / "default.frag", ShaderStage::Fragment }
        });

        mRenderPass = RenderPass::Create({ RenderPassUsage::Image, target.GetSurface()->Width(), target.GetSurface()->Height(), target.GetSurface()->ImageCount(), nullptr, true });
        
        mPipeline = Pipeline::Create({ mShader, mRenderPass });

        mDescriptorSet = mPipeline->CreateDescriptorSet(0);

        std::vector<Vertex> vertices = {
            { { -0.5f, -0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  0.5f, -0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.5f,  0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.0f }, { 1.0f, 1.0f, 1.0f } },

            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } }
        };

            std::vector<u32> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };
        
        mVertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size());
        mIndexBuffer = IndexBuffer::Create(indices.data(), indices.size());
    }

    SceneRenderer::~SceneRenderer() {
        delete mVertexBuffer;
        delete mIndexBuffer;
        
        delete mDescriptorSet;
        delete mPipeline;
        delete mRenderPass;
        delete mShader;
    }

    RenderPass* SceneRenderer::OutputRenderPass() { return mRenderPass; }

    void SceneRenderer::Update() {
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
    }
    
    void SceneRenderer::Render() {
        mRenderPass->Begin();

        mPipeline->Bind();

        static struct Camera{
            mat4f View;
            mat4f Projection;
        } camera;

        camera.View = mCamera.ViewMatrix();
        camera.Projection = mCamera.ProjectionMatrix();

        mDescriptorSet->SetUniform("Camera", &camera);
        
        mDescriptorSet->Bind();

        mScene->ExecuteForEachEntity([this](Entity e) {
            TransformComponent& comp = e.GetComponent<TransformComponent>();
            mat4f model = comp.Transform();

            mPipeline->PushConstant("model", &model, sizeof model);

            mVertexBuffer->Bind();
            mIndexBuffer->Bind();
            mIndexBuffer->Draw();
        });

        mRenderPass->End();
    }
}