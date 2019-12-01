#pragma once

#include "model/baseoperator.h"
#include "library/standard/control/parameterpreview.h"





class NoiseWaveControlOperator : public BaseOperator
{

public:

    NoiseWaveControlOperator();

    void run() override;

    static BaseOperator * create();

    static const OperatorDescription description;

    static const EnumDefinition NoiseType;

private:

    EnumDefinition UseTime { "Use Time as Position", { "True", "False" } };

    void recalculate();


    FloatPar output   { this, "Output",    0 };
    EnumPar noise_type { this, "Noise Type", NoiseType };
    EnumPar use_time   { this, "Use time as Input", UseTime, 0 };
    FloatPar amplitude{ this, "Amplitude", 1 };
    FloatPar offset   { this, "Offset",    0 };
    Float4Par position{ this, "Position",  {} };
    FloatPar slow_down{ this, "Slow Down X", 3 };

};
