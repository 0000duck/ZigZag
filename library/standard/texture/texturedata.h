#pragma once

#include "model/basedatatype.h"
#include "model/enumdefinition.h"
#include "model/parameter/enum.h"
#include "model/parameter/int.h"

#include <QObject>
#include <QOpenGLFunctions_3_2_Core>




const inline EnumDefinition PixelDataFormat
{
    "PixelDataType",
    {
        "8 Bit Unsigned Norm",
        "8 Bit Signed Norm",
        "8 Bit Unsigned Int",
        "8 Bit Signed Int",
        "16 Bit Unsigned Norm",
        "16 Bit Signed Norm",
        "16 Bit Unsigned Int",
        "16 Bit Signed Int",
        "32 Bit Unsigned Int",
        "32 Bit Signed Int",
        "16 Bit Float",
        "32 Bit Float"
    }
};


const inline EnumDefinition PixelNumChannels
{
    "PixelNumChannels",
    {
        "1 Channel",
        "2 Channels",
        "3 Channels",
        "4 Channels"
    }
};


class TextureData : public BaseDataType,
                    protected QOpenGLFunctions_3_2_Core
{
    Q_OBJECT

public:

    TextureData(BaseOperator* parent_operator);

    ~TextureData() override;


    void acquire_resources() override;

    void release_resources() override;


    void bind_as_framebuffer();

    void bind_as_texture(int texture_index) const;


    static const inline DataTypeInfo Type { "Texture", "Standard/Texture", QColor(255, 20, 147), true };


private:

    void reallocate();

    static GLenum gl_format_for(const EnumPar& format, const EnumPar& num_channels);

    Int2Par resolution{ this, "Resolution", { 256, 256 }, 1, 16384 };
    EnumPar pixel_format{ this, PixelDataFormat, "Format", 0 };
    EnumPar pixel_channels{ this, PixelNumChannels, "Num Channels", 3 };

    GLuint fbo_handle;
    GLuint texture_handle;

    bool currently_allocated = false;


};
