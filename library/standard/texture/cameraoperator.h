#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include <QCamera>
#include <QVideoFrame>
#include <QCameraImageCapture>

#include "model/baseoperator.h"

#include "texturedata.h"
#include "textureview.h"



class CameraOperator : public BaseOperator,
                       public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:

    CameraOperator();

    void run() override;

    void parameter_changed(BaseParameter* parameter) override;


    static BaseOperator * create() { return new CameraOperator(); }

    static const inline OperatorTypeInfo Type { "Camera", "Texture", {  },
                                              { &TextureData::Type }, &TextureView::Type, &create };

public slots:

    void on_image_captured(int id, const QVideoFrame& buffer);

private:

    TextureData output_texture{ this, "Camera Image", false };

    QCamera camera;
    QCameraImageCapture capture{ &camera };

    QVideoFrame last_frame;
    bool has_frame;

};