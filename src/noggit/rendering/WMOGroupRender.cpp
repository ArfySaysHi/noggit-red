// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/data/WMOData.hpp>
#include <noggit/rendering/WMOGroupRender.hpp>
#include <noggit/wmo/WMOMaterial.hpp>

using namespace Noggit::Rendering;

WMOGroupRender::WMOGroupRender() {}

void WMOGroupRender::upload(
    WMOData::GroupGeometry const &geometry,
    std::vector<WMOMaterial> const &materials,
    std::vector<scoped_blp_texture_reference> const &textures) {
  bool texture_not_uploaded = false;

  std::size_t batch_counter = 0;
  for (auto &batch : geometry.batches) {
    WMOMaterial const &mat(materials.at(batch.texture));

    auto &tex1 = textures.at(mat.texture1_index);

    tex1->wait_until_loaded();
    tex1->upload();

    std::uint32_t tex_array0 = tex1->texture_array();
    std::uint32_t array_index0 = tex1->array_index();

    std::uint32_t tex_array1 = 0;
    std::uint32_t array_index1 = 0;
    bool use_tex2 = mat.shader == 6 || mat.shader == 5 || mat.shader == 3;

    if (use_tex2) {
      auto &tex2 = textures.at(mat.texture2_index);
      tex2->wait_until_loaded();
      tex2->upload();

      tex_array1 = tex2->texture_array();
      array_index1 = tex2->array_index();
    }

    _render_batches[batch_counter].tex_array0 = tex_array0;
    _render_batches[batch_counter].tex_array1 = tex_array1;
    _render_batches[batch_counter].tex0 = array_index0;
    _render_batches[batch_counter].tex1 = array_index1;

    batch_counter++;
  }

  if (texture_not_uploaded) {
    return;
  }

  _draw_calls.clear();
  WMOCombinedDrawCall *draw_call = nullptr;
  std::vector<WMORenderBatch *> _used_batches;

  batch_counter = 0;
  for (auto &batch : geometry.batches) {
    WMOMaterial const &mat = materials.at(batch.texture);
    bool backface_cull = !mat.flags.unculled;
    bool use_tex2 = mat.shader == 6 || mat.shader == 5 || mat.shader == 3;

    bool create_draw_call = false;
    if (draw_call && draw_call->backface_cull == backface_cull &&
        batch.index_start == draw_call->index_start + draw_call->index_count) {
      // identify if we can fit this batch into current draw_call
      unsigned n_required_slots = use_tex2 ? 2 : 1;
      unsigned n_avaliable_slots =
          static_cast<unsigned>(draw_call->samplers.size()) -
          draw_call->n_used_samplers;
      unsigned n_slots_to_be_occupied = 0;

      std::vector<int>::iterator it2;
      auto it =
          std::find(draw_call->samplers.begin(), draw_call->samplers.end(),
                    _render_batches[batch_counter].tex_array0);

      if (it == draw_call->samplers.end()) {
        if (n_avaliable_slots)
          n_slots_to_be_occupied++;
        else
          create_draw_call = true;
      }

      if (!create_draw_call && use_tex2) {
        it2 = std::find(draw_call->samplers.begin(), draw_call->samplers.end(),
                        _render_batches[batch_counter].tex_array1);

        if (it2 == draw_call->samplers.end()) {
          if (n_slots_to_be_occupied < n_avaliable_slots)
            n_slots_to_be_occupied++;
          else
            create_draw_call = true;
        }
      }

      if (!create_draw_call) {
        if (it != draw_call->samplers.end()) {
          _render_batches[batch_counter].tex_array0 =
              it - draw_call->samplers.begin();
        } else {
          draw_call->samplers[draw_call->n_used_samplers] =
              _render_batches[batch_counter].tex_array0;
          _render_batches[batch_counter].tex_array0 =
              draw_call->n_used_samplers;
          draw_call->n_used_samplers++;
        }

        if (use_tex2) {
          if (it2 != draw_call->samplers.end()) {
            _render_batches[batch_counter].tex_array1 =
                it2 - draw_call->samplers.begin();
          } else {
            draw_call->samplers[draw_call->n_used_samplers] =
                _render_batches[batch_counter].tex_array1;
            _render_batches[batch_counter].tex_array1 =
                draw_call->n_used_samplers;
            draw_call->n_used_samplers++;
          }
        }
      }

    } else {
      create_draw_call = true;
    }

    if (create_draw_call) {
      // create new combined draw call
      draw_call = &_draw_calls.emplace_back();
      draw_call->samplers = std::vector<int>{-1, -1, -1, -1, -1, -1, -1, -1,
                                             -1, -1, -1, -1, -1, -1, -1};
      draw_call->index_start = batch.index_start;
      draw_call->index_count = 0;
      draw_call->n_used_samplers = use_tex2 ? 2 : 1;
      draw_call->backface_cull = backface_cull;

      draw_call->samplers[0] = _render_batches[batch_counter].tex_array0;
      _render_batches[batch_counter].tex_array0 = 0;

      if (use_tex2) [[unlikely]] {
        draw_call->samplers[1] = _render_batches[batch_counter].tex_array1;
        _render_batches[batch_counter].tex_array1 = 1;
      }
    }

    draw_call->index_count += batch.index_count;

    batch_counter++;
  }

  // opengl resources
  _vertex_array.upload();
  _buffers.upload();
  gl.genTextures(1, &_render_batch_tex);

  {
    OpenGL::Scoped::vao_binder const _(_vao);

    gl.bufferData<GL_ARRAY_BUFFER>(_vertices_buffer,
                                   geometry.vertices.size() *
                                       sizeof(*geometry.vertices.data()),
                                   geometry.vertices.data(), GL_STATIC_DRAW);

    gl.bufferData<GL_ARRAY_BUFFER>(_normals_buffer,
                                   geometry.normals.size() *
                                       sizeof(*geometry.normals.data()),
                                   geometry.normals.data(), GL_STATIC_DRAW);

    gl.bufferData<GL_ARRAY_BUFFER>(_texcoords_buffer,
                                   geometry.texcoords.size() *
                                       sizeof(*geometry.texcoords.data()),
                                   geometry.texcoords.data(), GL_STATIC_DRAW);

    gl.bufferData<GL_ARRAY_BUFFER>(
        _render_batch_mapping_buffer,
        _render_batch_mapping.size() * sizeof(unsigned),
        _render_batch_mapping.data(), GL_STATIC_DRAW);

    gl.bindBuffer(GL_TEXTURE_BUFFER, _render_batch_tex_buffer);
    gl.bufferData(GL_TEXTURE_BUFFER,
                  _render_batches.size() * sizeof(WMORenderBatch),
                  _render_batches.data(), GL_STATIC_DRAW);
    gl.bindTexture(GL_TEXTURE_BUFFER, _render_batch_tex);
    gl.texBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, _render_batch_tex_buffer);

    gl.bufferData<GL_ELEMENT_ARRAY_BUFFER, std::uint16_t>(
        _indices_buffer, geometry.indices, GL_STATIC_DRAW);

    if (_has_two_motv) {
      gl.bufferData<GL_ARRAY_BUFFER, glm::vec2>(
          _texcoords_buffer_2, geometry.texcoords_2, GL_STATIC_DRAW);
    }

    gl.bufferData<GL_ARRAY_BUFFER>(
        _vertex_colors_buffer,
        sizeof(*geometry.vertex_colors.data()) * geometry.vertex_colors.size(),
        geometry.vertex_colors.data(), GL_STATIC_DRAW);
  }

  _uploaded = true;
}

void WMOGroupRender::unload() {
  _vertex_array.unload();
  _buffers.unload();

  gl.deleteTextures(1, &_render_batch_tex);

  _uploaded = false;
  _vao_is_setup = false;
  _draw_calls.clear();
}

void WMOGroupRender::setupVao(OpenGL::Scoped::use_program &wmo_shader) {
  OpenGL::Scoped::index_buffer_manual_binder indices(_indices_buffer);
  {
    OpenGL::Scoped::vao_binder const _(_vao);

    wmo_shader.attrib("position", _vertices_buffer, 3, GL_FLOAT, GL_FALSE, 0,
                      0);
    wmo_shader.attrib("normal", _normals_buffer, 3, GL_FLOAT, GL_FALSE, 0, 0);
    wmo_shader.attrib("texcoord", _texcoords_buffer, 2, GL_FLOAT, GL_FALSE, 0,
                      0);
    wmo_shader.attribi("batch_mapping", _render_batch_mapping_buffer, 1,
                       GL_UNSIGNED_INT, 0, 0);

    if (_has_two_motv) {
      wmo_shader.attrib("texcoord_2", _texcoords_buffer_2, 2, GL_FLOAT,
                        GL_FALSE, 0, 0);
    }

    // even if the 2 flags are set there's only one vertex color vector, the 2nd
    // chunk is used for alpha only
    if (_has_vertex_color || _use_mocv2_blending) {
      wmo_shader.attrib("vertex_color", _vertex_colors_buffer, 4, GL_FLOAT,
                        GL_FALSE, 0, 0);
    }

    indices.bind();
  }

  _vao_is_setup = true;
}

void WMOGroupRender::draw(OpenGL::Scoped::use_program &wmo_shader,
                          math::frustum const & // frustum
                          ,
                          const float & // cull_distance
                          ,
                          const glm::vec3 & // camera
                          ,
                          bool // draw_fog
                          ,
                          bool // world_has_skies
) {
  if (!_vao_is_setup) [[unlikely]] {
    setupVao(wmo_shader);
  }

  OpenGL::Scoped::vao_binder const _(_vao);

  gl.activeTexture(GL_TEXTURE0);
  gl.bindTexture(GL_TEXTURE_BUFFER, _render_batch_tex);

  bool backface_cull = true;
  gl.enable(GL_CULL_FACE);

  for (auto &draw_call : _draw_calls) {
    if (backface_cull != draw_call.backface_cull) {
      if (draw_call.backface_cull) {
        gl.enable(GL_CULL_FACE);
      } else {
        gl.disable(GL_CULL_FACE);
      }

      backface_cull = draw_call.backface_cull;
    }

    for (std::size_t i = 0; i < draw_call.samplers.size(); ++i) {
      if (draw_call.samplers[i] < 0)
        break;

      gl.activeTexture(static_cast<GLenum>(GL_TEXTURE0 + 1 + i));
      gl.bindTexture(GL_TEXTURE_2D_ARRAY, draw_call.samplers[i]);
    }

    gl.drawElements(GL_TRIANGLES, draw_call.index_count, GL_UNSIGNED_SHORT,
                    reinterpret_cast<void *>(sizeof(std::uint16_t) *
                                             draw_call.index_start));
  }
}

void WMOGroupRender::initRenderBatches(
    WMOData::GroupGeometry const &geometry, WMOData::GroupFlags const &flags,
    std::vector<WMOMaterial> const &materials) {
  _has_two_motv = flags.has_two_motv;
  _has_vertex_color = flags.has_vertex_color;
  _use_mocv2_blending = flags.use_mocv2_for_texture_blending;

  _render_batch_mapping.resize(geometry.vertices.size());
  std::fill(_render_batch_mapping.begin(), _render_batch_mapping.end(), 0);

  _render_batches.resize(geometry.batches.size());

  std::size_t batch_counter = 0;
  for (auto &batch : geometry.batches) {
    for (std::size_t i = 0; i < (batch.vertex_end - batch.vertex_start + 1);
         ++i) {
      _render_batch_mapping[batch.vertex_start + i] =
          static_cast<unsigned>(batch_counter + 1);
    }

    std::uint32_t flag_store = 0;

    if (flags.exterior_lit || flags.exterior) {
      flag_store |= WMORenderBatchFlags::eWMOBatch_ExteriorLit;
    }
    if (flags.has_vertex_color || flags.use_mocv2_for_texture_blending) {
      flag_store |= WMORenderBatchFlags::eWMOBatch_HasMOCV;
    }

    WMOMaterial const &mat(materials.at(batch.texture));

    if (mat.flags.unlit) {
      flag_store |= WMORenderBatchFlags::eWMOBatch_Unlit;
    }

    if (mat.flags.unfogged) {
      flag_store |= WMORenderBatchFlags::eWMOBatch_Unfogged;
    }

    std::uint32_t alpha_test;

    switch (mat.blend_mode) {
    case 1:
      alpha_test = 1; // 224/255
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      alpha_test = 2;
      break;
    case 0:
    default:
      alpha_test = 0;
      break;
    }

    _render_batches[batch_counter] =
        WMORenderBatch{flag_store, mat.shader, 0, 0, 0, 0, alpha_test, 0};

    batch_counter++;
  }
}
