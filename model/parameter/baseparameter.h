﻿#pragma once


class ParameterOwner;


enum class ParameterType
{
    Int,
    Int2,
    Int3,
    Int4,
    Float,
    Float2,
    Float3,
    Float4,
    Enum,
    Transform2D,
    Transform3D,
    ParameterRow
};



class BaseParameter
{
public:

    BaseParameter(ParameterOwner* owner, ParameterType type, const char * name);

    virtual ~BaseParameter();


    const char * name() const;

    ParameterType type() const;

    ParameterOwner * owner() const;


    // Also flags the parameter owner as changed!
    void flag_changed();
    void reset_changed_flag();
    bool has_changed() const;


private:

    bool m_changed = false;

    ParameterOwner * const m_owner;

    const ParameterType m_type;

    const char * const m_name;


};



class ArithmeticParameter : public BaseParameter
{
public:

    using BaseParameter::BaseParameter;


    bool minimal_updates() const
    {
        return m_minimal_updates;
    }
    void set_minimal_updates(bool value)
    {
        m_minimal_updates = value;
    }

    bool rollover() const
    {
        return m_rollover;
    }
    void set_rollover(bool value)
    {
        m_rollover = value;
    }

private:

    bool m_rollover = false;

    bool m_minimal_updates = false;
};

