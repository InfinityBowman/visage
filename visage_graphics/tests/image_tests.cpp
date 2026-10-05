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

#include "visage_graphics/image.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <random>

using namespace visage;
using namespace Catch;

namespace {
  long long nextPowerOfTwoArea(long long area) {
    long long result = 1;
    while (result < area)
      result *= 2;
    return result;
  }
}

TEST_CASE("An atlas grows to hold its images and no more than twice their room", "[graphics]") {
  // One wide image and many small: doubling both sides at once would make this 4096 square.
  PackedAtlasMap<int> atlas;
  atlas.addRect(0, 2080, 300);
  for (int i = 1; i < 60; ++i)
    atlas.addRect(i, 120, 120);
  atlas.pack();

  long long used = 2081LL * 301;
  used += 59LL * 121 * 121;
  const long long area = static_cast<long long>(atlas.width()) * atlas.height();
  REQUIRE(atlas.width() >= 2081);
  REQUIRE(area <= 2 * nextPowerOfTwoArea(used));
}

TEST_CASE("An image atlas gives back the room of images no longer drawn", "[graphics]") {
  static std::vector<unsigned char> texels(256 * 256 * 4, 0x80);
  ImageAtlas atlas(ImageAtlas::DataType::RGBA8);
  std::vector<ImageAtlas::PackedImage> drawn;
  for (int i = 0; i < 80; ++i) {
    Image image(texels.data(), static_cast<int>(texels.size()), 256, 256);
    image.raw = true;
    image.revision = i;
    drawn.push_back(atlas.addImage(image));
  }
  const long long grown = static_cast<long long>(atlas.width()) * atlas.height();
  REQUIRE(grown >= 80LL * 256 * 256);

  drawn.erase(drawn.begin() + 2, drawn.end());
  atlas.clearStaleImages();
  const long long kept = static_cast<long long>(atlas.width()) * atlas.height();
  REQUIRE(kept * 4 <= grown);
  REQUIRE(kept >= 2LL * 256 * 256);
}
