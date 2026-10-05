/* Copyright Vital Audio, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "texture.h"

#include "renderer.h"

#include <bgfx/bgfx.h>
#include <cstring>

namespace visage {
  struct Texture::Handle {
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
    int generation = -1;

    ~Handle() {
      if (bgfx::isValid(handle) && generation == Renderer::generation() &&
          Renderer::instance().initialized())
        bgfx::destroy(handle);
    }
  };

  static bgfx::TextureFormat::Enum bgfxFormat(Texture::Format format) {
    switch (format) {
    case Texture::Format::RGBA8: return bgfx::TextureFormat::RGBA8;
    case Texture::Format::RGBA16F: return bgfx::TextureFormat::RGBA16F;
    case Texture::Format::RGBA32F: return bgfx::TextureFormat::RGBA32F;
    }
    return bgfx::TextureFormat::RGBA8;
  }

  Texture::Texture(int width, int height, Format format) :
      width_(width), height_(height), format_(format), handle_(std::make_unique<Handle>()) {
    data_.assign(static_cast<size_t>(width_) * height_ * bytesPerTexel(), 0);
  }

  Texture::~Texture() = default;

  int Texture::bytesPerTexel() const {
    switch (format_) {
    case Format::RGBA8: return 4;
    case Format::RGBA16F: return 8;
    case Format::RGBA32F: return 16;
    }
    return 4;
  }

  void Texture::setData(const void* data) {
    std::memcpy(data_.data(), data, data_.size());
    stale_ = true;
  }

  uint16_t Texture::handleIndex() {
    auto& h = *handle_;
    if (h.generation != Renderer::generation()) {
      // Made before the device was last shut down: gone with it.
      h.handle = BGFX_INVALID_HANDLE;
      h.generation = Renderer::generation();
      stale_ = true;
    }
    if (!bgfx::isValid(h.handle)) {
      h.handle = bgfx::createTexture2D(width_, height_, false, 1, bgfxFormat(format_),
                                       BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
    }
    if (stale_ && bgfx::isValid(h.handle)) {
      bgfx::updateTexture2D(h.handle, 0, 0, 0, 0, width_, height_,
                            bgfx::copy(data_.data(), static_cast<uint32_t>(data_.size())));
      stale_ = false;
    }
    return h.handle.idx;
  }
}
