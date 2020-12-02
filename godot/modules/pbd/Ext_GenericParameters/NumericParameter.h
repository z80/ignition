#ifndef __NumericParameter_h__
#define __NumericParameter_h__

#include <functional>
#include <memory>
#include "Parameter.h"
#include <iostream>
#include <float.h>
#include <algorithm>

namespace GenParam
{
	/** Class of a numeric parameter. The user can define min/max values for this parameter type */
	template<typename T>
	class NumericParameter : public Parameter<T>
	{
	protected:
		T m_minValue;
		T m_maxValue;

	public:
		NumericParameter(const std::string& name, const std::string& label, T* valuePtr)
			: Parameter<T>(name, label, ParameterBase::INT32, valuePtr)
		{
			setType(m_minValue);
			m_maxValue = std::numeric_limits<T>::max();
			m_minValue = std::numeric_limits<T>::lowest();
			Parameter<T>::m_setValue = [&, valuePtr](T value) { *valuePtr = std::max(std::min(value, m_maxValue), m_minValue); };
		}

		NumericParameter(const std::string& name, const std::string& label, ParameterBase::GetFunc<T> getValue, ParameterBase::SetFunc<T> setValue)
			: Parameter<T>(name, label, ParameterBase::INT32, getValue, setValue)
		{
			setType(m_minValue);
			m_maxValue = std::numeric_limits<T>::max(); 
			m_minValue = std::numeric_limits<T>::lowest();
		}

		virtual ~NumericParameter() {}
		
		T getMinValue() const { return m_minValue; }
		void setMinValue(const T val) { m_minValue = val; }
		
		T getMaxValue() const { return m_maxValue; }
		void setMaxValue(const T val) { m_maxValue = val; }

		template<typename TN>
		void setType(TN v) {}

		void setType(char v) { ParameterBase::m_type = ParameterBase::DataTypes::INT8; }
		void setType(short v) { ParameterBase::m_type = ParameterBase::DataTypes::INT16; }
		void setType(int v) { ParameterBase::m_type = ParameterBase::DataTypes::INT32; }
		void setType(unsigned char v) { ParameterBase::m_type = ParameterBase::DataTypes::UINT8; }
		void setType(unsigned short v) { ParameterBase::m_type = ParameterBase::DataTypes::UINT16; }
		void setType(unsigned int v) { ParameterBase::m_type = ParameterBase::DataTypes::UINT32; }
		void setType(float v) { ParameterBase::m_type = ParameterBase::DataTypes::FLOAT; }
		void setType(double v) { ParameterBase::m_type = ParameterBase::DataTypes::DOUBLE; }
	};

	using FloatParameter = NumericParameter<float>;
	using DoubleParameter = NumericParameter<double>;
	using LongDoubleParameter = NumericParameter<long double>;
	using CharParameter = NumericParameter<unsigned char>;
	using UnsignedShortParameter = NumericParameter<unsigned short>;
	using UnsignedIntParameter = NumericParameter<unsigned int>;
	using UnsignedLongParameter = NumericParameter<unsigned long>;
	using SignedCharParameter = NumericParameter<signed char>;
	using ShortParameter = NumericParameter<signed short>;
	using IntParameter = NumericParameter<signed int>;
	using LongParameter = NumericParameter<signed long>;
}

#endif
