#include "OpenGLDriver.h"
#include "FileHelper.h"
#include "Scene.h"
#include "RenderableComponent.h"
#include "Shader.h"
#include "Engine.h"
#include "EngineConstants.h"
#include "Logger.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>

#define SET_TEXTURE_ID(n, id) glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), n), id);

void debugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        }
    }();

    if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
        MiniEngine::Logger::wprint("{}, {}, {}, {}, {}", src_str, type_str, severity_str, id, message);
}

namespace MiniEngine::Backend
{
    void OpenGLDriver::setupGlWindowParams(MiniEngine::Types::EngineInitParams& params, Engine* engine)
    {
        mWidth = params.screenWidth;
        mHeight = params.screenHeight;

        mEngine = engine;
        mParams = &params;

        glViewport(0, 0, mWidth, mHeight);

        glClearColor(
            params.clearColor.r(),
            params.clearColor.g(),
            params.clearColor.b(),
            params.clearColor.a()
        );

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glEnable(GL_DEPTH_TEST);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
    }

    void OpenGLDriver::setupFrameBuffer()
    {
        GLuint depth;
        glCreateFramebuffers(1, &mMainFrameBuffer);
        glCreateRenderbuffers(1, &depth);
        glCreateTextures(GL_TEXTURE_2D, 1, &mColorBuffer);
        glCreateTextures(GL_TEXTURE_2D, 1, &mPositionBuffer);
        glCreateTextures(GL_TEXTURE_2D, 1, &mNormalBuffer);
        glCreateTextures(GL_TEXTURE_2D, 1, &mRoughnessBuffer);
        glCreateTextures(GL_TEXTURE_2D, 1, &mAccumBuffer1);
        glCreateTextures(GL_TEXTURE_2D, 1, &mAccumBuffer2);

        glTextureParameteri(mAccumBuffer1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer1, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer1, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(mAccumBuffer1, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(mAccumBuffer2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer2, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(mAccumBuffer2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(mAccumBuffer2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureStorage2D(mAccumBuffer1, 1, GL_RGBA16F, mWidth, mHeight);
        glTextureStorage2D(mAccumBuffer2, 1, GL_RGBA16F, mWidth, mHeight);
        glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT0, mAccumBuffer1, 0);

        glTextureStorage2D(mColorBuffer, 1, GL_RGBA16F, mWidth, mHeight);
        glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT1, mColorBuffer, 0);

        glTextureStorage2D(mPositionBuffer, 1, GL_RGBA16F, mWidth, mHeight);
        glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT2, mPositionBuffer, 0);

        glTextureStorage2D(mNormalBuffer, 1, GL_RGBA16F, mWidth, mHeight);
        glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT3, mNormalBuffer, 0);

        glTextureStorage2D(mRoughnessBuffer, 1, GL_RGBA16F, mWidth, mHeight);
        glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT4, mRoughnessBuffer, 0);

        glNamedRenderbufferStorage(depth, GL_DEPTH24_STENCIL8, mWidth, mHeight);
        glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

        glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);

        if (glCheckNamedFramebufferStatus(mMainFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            MiniEngine::Logger::print("Framebuffer not complete");
        }
    }

    void OpenGLDriver::setupDebugInfo()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugCallback, nullptr);

        glCreateQueries(GL_TIME_ELAPSED, 1, &mDefaultFrameQuery);
    }

    void OpenGLDriver::setupImguiInterface(OpenGLImgui* imguiInterface)
    {
        mOpenGlImgui = imguiInterface;
    }

    void OpenGLDriver::setupSkybox(MiniEngine::Components::SkyboxComponent* skybox)
    {
        constexpr float skyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f,  1.0f,//        7--------6
             1.0f, -1.0f,  1.0f,//       /|       /|
             1.0f, -1.0f, -1.0f,//      4--------5 |
            -1.0f, -1.0f, -1.0f,//      | |      | |
            -1.0f,  1.0f,  1.0f,//      | 3------|-2
             1.0f,  1.0f,  1.0f,//      |/       |/
             1.0f,  1.0f, -1.0f,//      0--------1
            -1.0f,  1.0f, -1.0f
        };

        constexpr unsigned int skyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3
        };

        unsigned int vao;
        glCreateVertexArrays(1, &vao);

        skybox->vaoId = vao;

        glBindVertexArray(vao);

        unsigned int vbo, ebo;

        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ebo);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(float));
        glVertexArrayElementBuffer(vao, ebo);

        glEnableVertexAttribArray(0);

        glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(0, 0);

        glNamedBufferData(
            vbo, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        glNamedBufferData(
            ebo, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

        // Set-up cubemap for irradiance convolution =====================

        glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &(skybox->environmentCubemapId));
        glTextureStorage2D(skybox->environmentCubemapId, 1, GL_RGBA16F, 512, 512);
        
        glTextureParameteri(skybox->environmentCubemapId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->environmentCubemapId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->environmentCubemapId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->environmentCubemapId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(skybox->environmentCubemapId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Draw env map ====================================================

        const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        const glm::mat4 captureViews[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getSkyboxShader());
        setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_projection", captureProjection);

        glViewport(0, 0, 512, 512);
        glBindTextureUnit(0, skybox->mainTexture.getId());

        for (auto i = 0; i < 6; ++i)
        {
            setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_view", captureViews[i]);

            glBindImageTexture(0, skybox->environmentCubemapId, 0, GL_FALSE, i, GL_WRITE_ONLY, GL_RGBA16F);
            glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        }

        // Convolute ========================================================

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &(skybox->irradianceCubemapId));
        glTextureStorage2D(skybox->irradianceCubemapId, 1, GL_RGBA16F, 32, 32);
        
        glTextureParameteri(skybox->irradianceCubemapId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->irradianceCubemapId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->irradianceCubemapId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->irradianceCubemapId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(skybox->irradianceCubemapId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getSkyboxConvoluter());

        glBindTextureUnit(0, skybox->environmentCubemapId);
        setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_projection", captureProjection);

        glViewport(0, 0, 32, 32);

        for (auto i = 0; i < 6; ++i)
        {
            setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_view", captureViews[i]);

            glBindImageTexture(0, skybox->irradianceCubemapId, 0, GL_FALSE, i, GL_WRITE_ONLY, GL_RGBA16F);
            glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        }

        // Generate PreFiltered Map ==========================================

        constexpr unsigned int maxMipLevels = 5;

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &skybox->prefilteredCubemapId);
        glTextureStorage2D(skybox->prefilteredCubemapId, maxMipLevels, GL_RGBA16F, 128, 128);

        glTextureParameteri(skybox->prefilteredCubemapId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->prefilteredCubemapId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->prefilteredCubemapId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->prefilteredCubemapId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(skybox->prefilteredCubemapId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        

        glGenerateTextureMipmap(skybox->prefilteredCubemapId);

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getEnvPrefilterShader());
        
        glBindTextureUnit(0, skybox->environmentCubemapId);
        setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_projection", captureProjection);

        for (unsigned int level = 0; level < maxMipLevels; ++level)
        {
            unsigned int width = 128 * std::pow(0.5, level);
            unsigned int height = 128 * std::pow(0.5, level);

            glViewport(0, 0, width, height);

            float roughness = (float)level / (float)(maxMipLevels - 1);
            setFloat(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_roughness", roughness);

            for (unsigned int i = 0; i < 6; ++i)
            {
                setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_view", captureViews[i]);

                glBindImageTexture(0, skybox->prefilteredCubemapId, level, GL_FALSE, i, GL_WRITE_ONLY, GL_RGBA16F);
                glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
            }

        }

        // Generate Scale-Bias Lookup =======================================

        glCreateTextures(GL_TEXTURE_2D, 1, &skybox->precomputedBrdfId);
        glTextureStorage2D(skybox->precomputedBrdfId, 1, GL_RG16F, 512, 512);

        glTextureParameteri(skybox->precomputedBrdfId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->precomputedBrdfId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(skybox->precomputedBrdfId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(skybox->precomputedBrdfId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glViewport(0, 0, 512, 512);

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getEnvPreComputeBrdfShader());
        glBindVertexArray(mScreenQuadVertexArray);

        glBindImageTexture(0, skybox->precomputedBrdfId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup ==========================================================
        
        glBindVertexArray(0);
        glViewport(0, 0, mWidth, mHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLDriver::setupMesh(MiniEngine::Components::RenderableComponent* component)
    {
        unsigned int vao;
        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);

        component->vaoId = vao;

        glBindVertexArray(vao);

        unsigned int vbo, ebo;

        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ebo);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, component->stride * sizeof(float));
        glVertexArrayElementBuffer(vao, ebo);

        int start = 0;

        for (auto i = 0; i < component->attributes.size(); ++i)
        {
            glEnableVertexAttribArray(i);

            glVertexAttribFormat(i, component->attributes[i], GL_FLOAT, GL_FALSE, (start * sizeof(float)));
            glVertexAttribBinding(i, 0);

            start += component->attributes[i];
        }

        glNamedBufferData(
            vbo, component->buffer.size() * sizeof(float), component->buffer.data(), GL_STATIC_DRAW);
        glNamedBufferData(
            ebo, component->indices.size() * sizeof(unsigned int), component->indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void OpenGLDriver::setupDefaultMaps()
    {
        GLuint attachments[] = { GL_COLOR_ATTACHMENT0 };
        unsigned int fb;

        glCreateFramebuffers(1, &fb);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);

        glCreateTextures(GL_TEXTURE_2D, 1, &mDefaultMaps[(int)Types::TextureType::Diffuse]);
        glTextureStorage2D(mDefaultMaps[(int)Types::TextureType::Diffuse], 1, GL_RGBA16F, 2, 2);
        glNamedFramebufferTexture(fb, GL_COLOR_ATTACHMENT0, mDefaultMaps[(int)Types::TextureType::Diffuse], 0);
        glNamedFramebufferDrawBuffers(fb, 1, attachments);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glCreateTextures(GL_TEXTURE_2D, 1, &mDefaultMaps[(int)Types::TextureType::Normal]);
        glTextureStorage2D(mDefaultMaps[(int)Types::TextureType::Normal], 1, GL_RGBA16F, 2, 2);
        glNamedFramebufferTexture(fb, GL_COLOR_ATTACHMENT0, mDefaultMaps[(int)Types::TextureType::Normal], 0);
        glNamedFramebufferDrawBuffers(fb, 1, attachments);

        glClearColor(0.5f, 0.5f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glCreateTextures(GL_TEXTURE_2D, 1, &mDefaultMaps[(int)Types::TextureType::Roughness]);
        glTextureStorage2D(mDefaultMaps[(int)Types::TextureType::Roughness], 1, GL_RGBA16F, 2, 2);
        glNamedFramebufferTexture(fb, GL_COLOR_ATTACHMENT0, mDefaultMaps[(int)Types::TextureType::Roughness], 0);
        glNamedFramebufferDrawBuffers(fb, 1, attachments);

        glClearColor(1.0f, 1.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDeleteFramebuffers(1, &fb);
    }

    void OpenGLDriver::draw(Scene* scene)
    {
        #ifdef GRAPHICS_DEBUG
        glBeginQuery(GL_TIME_ELAPSED, mDefaultFrameQuery);
        #endif

        const auto& db = scene->getRenderableComponentDatabase();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // geometry pass ===================================

        glEnable(GL_DEPTH_TEST);

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getDeferredShader());
        SET_TEXTURE_ID("_Diffuse", 0);
        SET_TEXTURE_ID("_Normal", 1);
        SET_TEXTURE_ID("_Roughness", 2);

        for (int i = 0; i < db.size(); ++i)
        {
            glBindVertexArray(db[i].vaoId);

            for (int j = 0; j < MAX_TEXTURE_TYPES; ++j)
            {
                auto mask = db[i].materialInstance->textureMask;

                glBindTextureUnit(j, 
                    ((mask >> j) & 0x1) == 1 ? 
                    db[i].materialInstance->textureReference[j].getId() : 
                    mDefaultMaps[j]
                );
            }
            
            auto m = glm::mat4{ 1.0f };

            m = glm::rotate(m, glm::radians(db[i].rotation.x), glm::vec3{1, 0, 0});
            m = glm::rotate(m, glm::radians(db[i].rotation.y), glm::vec3{0, 1, 0});
            m = glm::rotate(m, glm::radians(db[i].rotation.z), glm::vec3{0, 0, 1});

            m = glm::translate(m, glm::vec3{db[i].worldPosition.x, db[i].worldPosition.y, db[i].worldPosition.z});

            setMat4(db[i].shader.getShaderProgram(), "_model", m);
            setFloat(db[i].shader.getShaderProgram(), "_baseRoughness", db[i].materialInstance->materialProperties[(int)MiniEngine::Material::PropertyType::Roughness]);
            setFloat(db[i].shader.getShaderProgram(), "_baseMetallic", db[i].materialInstance->materialProperties[(int)MiniEngine::Material::PropertyType::Metallic]);

            glDrawElements(GL_TRIANGLES, static_cast<int>(db[i].indices.size()), GL_UNSIGNED_INT, 0);
        }

        auto mainPassSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        // light pass ======================================

        glDisable(GL_DEPTH_TEST);

        glBindTextureUnit(0, mColorBuffer);
        glBindTextureUnit(1, mPositionBuffer);
        glBindTextureUnit(2, mNormalBuffer);
        glBindTextureUnit(3, mRoughnessBuffer);
        glBindTextureUnit(4, scene->getSkyBox()->irradianceCubemapId);
        glBindTextureUnit(5, scene->getSkyBox()->environmentCubemapId);
        glBindTextureUnit(6, scene->getSkyBox()->prefilteredCubemapId);
        glBindTextureUnit(7, scene->getSkyBox()->precomputedBrdfId);

        mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getPbrShader());
        SET_TEXTURE_ID("_Diffuse", 0);
        SET_TEXTURE_ID("_Position", 1);
        SET_TEXTURE_ID("_Normal", 2);
        SET_TEXTURE_ID("_Roughness", 3);
        SET_TEXTURE_ID("_irradianceMap", 4);
        SET_TEXTURE_ID("_environmentMap", 5);
        SET_TEXTURE_ID("_prefilteredMap", 6);
        SET_TEXTURE_ID("_precomputedBrdf", 7);

        glBindVertexArray(mScreenQuadVertexArray);

        glWaitSync(mainPassSync, 0, GL_TIMEOUT_IGNORED);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDeleteSync(mainPassSync);

        // enviroment pass =================================

        const auto& skybox = scene->getSkyBox();

        if (skybox != nullptr && skybox->skyboxType == Skybox::SkyboxType::Skybox)
        {
            mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getSkyboxRenderShader());

            // explicitly use fov 90 perspective
            const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
            setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_projection", captureProjection);

            //set rotation
            auto rotMatrix = glm::rotate(glm::mat4{1.0}, skybox->rotation, glm::vec3 { 0, 1, 0 });
            setMat4(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_view", rotMatrix);

            glBindVertexArray(skybox->vaoId);
            glEnable(GL_DEPTH_TEST);

            glBindTextureUnit(0, skybox->environmentCubemapId);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // post-process pass ===============================

        if (mParams->enablePostProcess)
        {
            std::swap(mAccumBuffer1, mAccumBuffer2);
            glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT0, mAccumBuffer2, 0);

            mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getPostProcessShader());
            SET_TEXTURE_ID("_Accum", 0);
            glBindTextureUnit(0, mAccumBuffer2);

            setFloat(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_width", 1.0f / mWidth);
            setFloat(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_height", 1.0f / mHeight);

            glBindVertexArray(mScreenQuadVertexArray);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // ================================================

        #ifdef GRAPHICS_DEBUG
        int done = 0;
        GLint64 elapsedTime;
        glEndQuery(GL_TIME_ELAPSED);

        while (!done) glGetQueryObjectiv(mDefaultFrameQuery, GL_QUERY_RESULT_AVAILABLE, &done);

        glGetQueryObjecti64v(mDefaultFrameQuery, GL_QUERY_RESULT, &elapsedTime);

        mOpenGlImgui->pushFrameTimeData(elapsedTime / 1000000.0f);

        #endif
    }

    void OpenGLDriver::finalBlit()
    {
        glBlitNamedFramebuffer(
            mMainFrameBuffer, 0,
            0, 0, mWidth, mHeight,
            0, 0, mWidth, mHeight,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void OpenGLDriver::beginRenderpass()
    {
        GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

        glEnable(GL_SCISSOR_TEST);
        glScissor(0, 0, mWidth, mHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);
        glNamedFramebufferDrawBuffers(mMainFrameBuffer, 5, attachments);
    }

    void OpenGLDriver::endRenderpass()
    {
        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint OpenGLDriver::loadShader(const char* path, ShaderType type) const
    {
        auto shaderFile = FileHelper::loadFile(path);
        auto shaderStr = shaderFile.c_str();
        auto shaderId = glCreateShader((GLenum)type);

        glShaderSource(shaderId, 1, &shaderStr, nullptr);
        glCompileShader(shaderId);

        int isCompiled;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);

        if (!isCompiled)
        {
            char log[512];
            glGetShaderInfoLog(shaderId, 512, nullptr, log);
            MiniEngine::Logger::eprint(" Shader compilation error: \n\n {}", log);
        }

        return shaderId;
    }

    void OpenGLDriver::useShaderProgram(unsigned int program) const
    {
        glUseProgram(program);
    }

    void OpenGLDriver::registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const
    {
        auto blockIndex = glGetUniformBlockIndex(program->getShaderProgram(), blockName);
        glUniformBlockBinding(program->getShaderProgram(), blockIndex, layoutIndex);
    }

    unsigned int OpenGLDriver::createUniformBlock(size_t dataSize, unsigned int bindIndex) const
    {
        unsigned int uniformBufferId;

        glCreateBuffers(1, &uniformBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferId);
        glNamedBufferStorage(uniformBufferId, dataSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, uniformBufferId);

        return uniformBufferId;
    }

    void OpenGLDriver::updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const
    {
        glNamedBufferSubData(bufferId, offset, size, data);
    }

    unsigned int OpenGLDriver::createTexture(int width, int height, int channels, void* data, Texture::TextureType type)
    {
        GLuint tex;
        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, 1, channels == 3 ? GL_RGB32F : GL_RGBA32F, width, height);
        glTextureSubImage2D(tex, 0, 0, 0, width, height, channels == 3 ? GL_RGB : GL_RGBA,
            type == Texture::TextureType::CubeMap ? GL_FLOAT : GL_UNSIGNED_BYTE, data);
        return tex;
    }

    void OpenGLDriver::setFloat(unsigned int program, const char* name, float value) const
    {
        auto location = glGetUniformLocation(program, name);
        glUniform1f(location, value);
    }

    void OpenGLDriver::setVec3(unsigned int program, const char* name, Vector3 value) const
    {
        auto location = glGetUniformLocation(program, name);
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLDriver::setMat4(unsigned int program, const char* name, Matrix4x4 value) const
    {
        auto location = glGetUniformLocation(program, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, &(value[0].x));
    }

    void OpenGLDriver::setupScreenQuad()
    {
        constexpr float screenQuad[] =
        {
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f , 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f
        };

        constexpr unsigned int screenIndex[] =
        {
            0, 1, 2,
            0, 2, 3
        };

        static GLuint sQuad, sEbo;

        glCreateVertexArrays(1, &mScreenQuadVertexArray);
        glBindVertexArray(mScreenQuadVertexArray);

        glCreateBuffers(1, &sQuad);
        glCreateBuffers(1, &sEbo);

        glVertexArrayVertexBuffer(mScreenQuadVertexArray, 0, sQuad, 0, 4 * sizeof(float));
        glVertexArrayElementBuffer(mScreenQuadVertexArray, sEbo);

        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(0, 0);

        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexAttribBinding(1, 0);

        glNamedBufferData(sQuad, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
        glNamedBufferData(sEbo, sizeof(screenIndex), screenIndex, GL_STATIC_DRAW);
    }

    unsigned int OpenGLDriver::createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const
    {
        auto program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        int isCompiled;
        glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);

        if (!isCompiled)
        {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            MiniEngine::Logger::eprint(" Shader linking error : \n\n {}", log);
        }

        return program;
    }
}