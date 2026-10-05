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

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace visage {
  // A texture of the caller's own for a Shader to sample: data rather than a picture in an
  // atlas, read texel by texel, made on the GPU when first bound and again after the device
  // is shut down and brought back up. Setting new data uploads it at the next bind.
  class Texture {
  public:
    enum class Format {
      RGBA8,
      RGBA16F,
      RGBA32F,
    };

    Texture(int width, int height, Format format);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    int width() const { return width_; }
    int height() const { return height_; }
    Format format() const { return format_; }
    int bytesPerTexel() const;

    // width * height texels, row by row, in the format's own layout.
    void setData(const void* data);
    const std::vector<uint8_t>& data() const { return data_; }

    // For the renderer: the texture as the GPU has it now.
    uint16_t handleIndex();

  private:
    struct Handle;

    int width_ = 0;
    int height_ = 0;
    Format format_ = Format::RGBA8;
    std::vector<uint8_t> data_;
    bool stale_ = true;
    std::unique_ptr<Handle> handle_;
  };
}
