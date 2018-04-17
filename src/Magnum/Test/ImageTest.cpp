/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"

namespace Magnum { namespace Test {

struct ImageTest: TestSuite::Tester {
    explicit ImageTest();

    void constructGeneric();
    void constructGenericPlaceholder();
    void constructImplementationSpecific();
    void constructImplementationSpecificPlaceholder();
    void constructCompressedGeneric();
    void constructCompressedGenericPlaceholder();
    void constructCompressedImplementationSpecific();
    void constructCompressedImplementationSpecificPlaceholder();
    void constructCopy();
    void constructCopyCompressed();
    void constructMove();
    void constructMoveCompressed();

    void toView();
    void toViewCompressed();
    void release();
    void releaseCompressed();
};

ImageTest::ImageTest() {
    addTests({&ImageTest::constructGeneric,
              &ImageTest::constructGenericPlaceholder,
              &ImageTest::constructImplementationSpecific,
              &ImageTest::constructImplementationSpecificPlaceholder,
              &ImageTest::constructCompressedGeneric,
              &ImageTest::constructCompressedGenericPlaceholder,
              &ImageTest::constructCompressedImplementationSpecific,
              &ImageTest::constructCompressedImplementationSpecificPlaceholder,
              &ImageTest::constructCopy,
              &ImageTest::constructCopyCompressed,
              &ImageTest::constructMove,
              &ImageTest::constructMoveCompressed,

              &ImageTest::toView,
              &ImageTest::toViewCompressed,
              &ImageTest::release,
              &ImageTest::releaseCompressed});
}

namespace {

namespace GL {
    enum class PixelFormat { RGB = 666 };
    enum class PixelType { UnsignedShort = 1337 };
    UnsignedInt pixelSize(PixelFormat format, PixelType type) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::RGB);
        CORRADE_INTERNAL_ASSERT(type == PixelType::UnsignedShort);
        return 6;
    }

    enum class CompressedPixelFormat { RGBS3tcDxt1 = 21 };
}

namespace Vk {
    enum class PixelFormat { R32G32B32F = 42 };
    UnsignedInt pixelSize(PixelFormat format) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::R32G32B32F);
        return 12;
    }

    enum class CompressedPixelFormat { Bc1SRGBAlpha = 42 };
}

}

void ImageTest::constructGeneric() {
    {
        auto data = new char[3*4];
        Image2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 3*4}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    } {
        auto data = new char[1*2];
        Image2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, Containers::Array<char>{data, 1*2}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    }
}

void ImageTest::constructGenericPlaceholder() {
    {
        Image2D a{PixelFormat::RG32F};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RG32F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 16);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        Image2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::RGB16F};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::RGB16F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageTest::constructImplementationSpecific() {
    /* Single format */
    {
        auto data = new char[3*12];
        Image2D a{Vk::PixelFormat::R32G32B32F, {1, 3}, Containers::Array<char>{data, 3*12}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    } {
        auto data = new char[3*6];
        Image2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, Containers::Array<char>{data, 3*12}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    }

    /* Format + extra */
    {
        auto data = new char[3*6];
        Image2D a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    } {
        auto data = new char[3*6];
        Image2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    }

    /* Manual pixel size */
    {
        auto data = new char[3*6];
        Image2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, Containers::Array<char>{data, 3*6}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
    }
}

void ImageTest::constructImplementationSpecificPlaceholder() {
    /* Single format */
    {
        Image2D a{Vk::PixelFormat::R32G32B32F};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        Image2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Format + extra */
    {
        Image2D a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        Image2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual pixel size */
    {
        Image2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageTest::constructCompressedGeneric() {
    auto data = new char[8];
    CompressedImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data);
    CORRADE_COMPARE(a.data().size(), 8);
}

void ImageTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Image2D, const Image2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Image2D, const Image2D&>{}));
}

void ImageTest::constructCopyCompressed() {
    CORRADE_VERIFY(!(std::is_constructible<CompressedImage2D, const CompressedImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CompressedImage2D, const CompressedImage2D&>{}));
}

void ImageTest::constructMove() {
    auto data = new char[3*3];
    Image2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, Containers::Array<char>{data, 3*3}};
    Image2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGB);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    auto data2 = new char[12*4*2];
    Image2D c{PixelFormat::RGBA, PixelType::UnsignedShort, {2, 6}, Containers::Array<char>{data2, 12*4*2}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), PixelFormat::RGB);
    CORRADE_COMPARE(c.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.data(), data);
}

void ImageTest::constructMoveCompressed() {
    auto data = new char[8];
    CompressedImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImage2D b{std::move(a)};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);

    auto data2 = new char[16];
    CompressedImage2D c{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {8, 4}, Containers::Array<char>{data2, 16}};
    c = std::move(b);

    CORRADE_COMPARE_AS(b.data(), data2, char*);
    CORRADE_COMPARE(b.data().size(), 16);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(c.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(c.size(), Vector2i(4, 4));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 8);
}

void ImageTest::toView() {
    auto data = new char[3*3];
    const Image2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, Containers::Array<char>{data, 3*3}};
    ImageView2D b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGB);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);
}

void ImageTest::toViewCompressed() {
    auto data = new char[8];
    const CompressedImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView2D b = a;

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageTest::release() {
    char data[] = {'c', 'a', 'f', 'e'};
    Image2D a(PixelFormat::RGBA, PixelType::UnsignedByte, {1, 1}, Containers::Array<char>{data, 4});
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageTest::releaseCompressed() {
    char data[8];
    CompressedImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageTest)
