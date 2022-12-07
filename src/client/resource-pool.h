#pragma once

#include "client/gl-base-technique.h"
#include "client/gl-buffer.h"
#include "client/gl-program.h"
#include "client/gl-sampler.h"
#include "client/gl-shader.h"
#include "client/gl-texture.h"
#include "client/gl-vertex-array.h"
#include "client/rendering-unit.h"
#include "client/resource-container.h"
#include "client/virtual-object.h"
#include "core/common.h"

namespace zen::remote::client {

using VirtualObjectContainer =
    ResourceContainer<VirtualObject, ResourceContainerType::kLoopIntensive>;

using RenderingUnitContainer =
    ResourceContainer<RenderingUnit, ResourceContainerType::kFindByIdIntensive>;

using GlBufferContainer =
    ResourceContainer<GlBuffer, ResourceContainerType::kFindByIdIntensive>;

using GlShaderContainer =
    ResourceContainer<GlShader, ResourceContainerType::kFindByIdIntensive>;

using GlProgramContainer =
    ResourceContainer<GlProgram, ResourceContainerType::kFindByIdIntensive>;

using GlTextureContainer =
    ResourceContainer<GlTexture, ResourceContainerType::kFindByIdIntensive>;

using GlSamplerContainer =
    ResourceContainer<GlSampler, ResourceContainerType::kFindByIdIntensive>;

using GlVertexArrayContainer =
    ResourceContainer<GlVertexArray, ResourceContainerType::kFindByIdIntensive>;

using GlBaseTechniqueContainer = ResourceContainer<GlBaseTechnique,
    ResourceContainerType::kFindByIdIntensive>;

/**
 * @brief Retain all resources
 *
 * Resources are updated by the "update thread" and the updated states are
 * temporarily held in the "pending state" of the respective resource.
 * When a set of resources are commited, commands to update the "rendering
 * state" based on the "pending state" are pushed to
 * `Remote::update_rendering_queue`. The pushed commands are executed by the
 * "rendering thread" just before the resource pool is traversed to render
 * scene. At this time, commands issued in a single commit are executed
 * atomically, thus maintaining the integrity of the entire "rendering state".
 * These commands should not read the pending state, since the pending state may
 * be updated between when these commands are pushed and when they are executed.
 * Instead, use a lambda capture or something similar to preserve the state at
 * the time of commit. This also helps keep multi-threading safe.
 */
class ResourcePool {
 public:
  DISABLE_MOVE_AND_COPY(ResourcePool);
  ResourcePool() = default;

  inline VirtualObjectContainer *virtual_objects();
  inline RenderingUnitContainer *rendering_units();
  inline GlBufferContainer *gl_buffers();
  inline GlShaderContainer *gl_shaders();
  inline GlProgramContainer *gl_programs();
  inline GlTextureContainer *gl_textures();
  inline GlSamplerContainer *gl_samplers();
  inline GlVertexArrayContainer *gl_vertex_arrays();
  inline GlBaseTechniqueContainer *gl_base_techniques();

 private:
  VirtualObjectContainer virtual_objects_;
  RenderingUnitContainer rendering_units_;
  GlBufferContainer gl_buffers_;
  GlShaderContainer gl_shaders_;
  GlProgramContainer gl_programs_;
  GlTextureContainer gl_textures_;
  GlSamplerContainer gl_samplers_;
  GlVertexArrayContainer gl_vertex_arrays_;
  GlBaseTechniqueContainer gl_base_techniques_;
};

inline VirtualObjectContainer *
ResourcePool::virtual_objects()
{
  return &virtual_objects_;
}

inline RenderingUnitContainer *
ResourcePool::rendering_units()
{
  return &rendering_units_;
}

inline GlBufferContainer *
ResourcePool::gl_buffers()
{
  return &gl_buffers_;
}

inline GlShaderContainer *
ResourcePool::gl_shaders()
{
  return &gl_shaders_;
}

inline GlProgramContainer *
ResourcePool::gl_programs()
{
  return &gl_programs_;
}

inline GlTextureContainer *
ResourcePool::gl_textures()
{
  return &gl_textures_;
}

inline GlSamplerContainer *
ResourcePool::gl_samplers()
{
  return &gl_samplers_;
}

inline GlVertexArrayContainer *
ResourcePool::gl_vertex_arrays()
{
  return &gl_vertex_arrays_;
}

inline GlBaseTechniqueContainer *
ResourcePool::gl_base_techniques()
{
  return &gl_base_techniques_;
}

}  // namespace zen::remote::client
