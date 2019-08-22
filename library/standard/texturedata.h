#pragma once

#include "model/basedatatype.h"
#include "model/enumdefinition.h"
#include "model/parameter/enumparameter.h"
#include "model/parameter/integerparameter.h"

#include <QObject>
#include <QOpenGLFunctions_3_2_Core>




const inline EnumDefinition PixelType
{
    "PixelType",
    {
        "uint8_1_channel",
        "uint8_2_channel",
        "uint8_3_channel",
        "uint8_4_channel",
        "float_1_channel",
        "float_2_channel",
        "float_3_channel",
        "float_4_channel"
    }
};


const inline EnumDefinition PixelDataFormat
{
    "PixelDataType",
    {
        "8 Bit Unsigned",
        "16 Bit Unsigned",
        "32 Bit Unsigned", // Cannot be normalized
        "8 Bit Signed",
        "16 Bit Signed",
        "32 Bit Signed", // Cannot be normalized
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

const inline EnumDefinition PixelIsNormalized
{
    "PixelNumChannels",
    {
        "False",
        "True"
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

    void bind_as_texture(int texture_index);


    static const inline DataTypeInfo Type { "Texture", QColor(255, 20, 147) };


public slots:

    void reallocate_texture();


private:

    EnumParameter pixel_format{ this, PixelDataFormat, "Pixel Data Format", 0 };
    EnumParameter pixel_channels{ this, PixelNumChannels, "Pixel Num Channels", 3 };
    EnumParameter pixel_normalized{ this, PixelIsNormalized, "Pixel is Normalized", 1 };
    IntegerParameter resolution_x{ this, "Resolution X", 256 };
    IntegerParameter resolution_y{ this, "Resolution Y", 256 };

    GLuint fbo_handle;
    GLuint texture_handle;

    bool currently_allocated = false;


};