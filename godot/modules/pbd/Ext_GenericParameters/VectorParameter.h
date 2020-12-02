#ifndef __VectorParameter_h__
#define __VectorParameter_h__

#include <functional>
#include <memory>
#include <cstring>
#include "Parameter.h"
#include <iostream>

namespace GenParam
{
	/** Class of a vector parameter. 
	*/
	template<typename T>
	class VectorParameter : public ParameterBase
	{
	protected:
		GetVecFunc<T> m_getVecValue;
		SetVecFunc<T> m_setVecValue;
		int m_dim;

	public:
		VectorParameter(const std::string& name, const std::string& label, const unsigned int dim, T* valuePtr)
			: ParameterBase(name, label, ParameterBase::INT32),
			m_dim(dim)
		{
			setType(valuePtr[0]);
			m_getVecValue = [valuePtr]() { return valuePtr; };
			m_setVecValue = [valuePtr, dim](T* value)
			{
				memcpy(valuePtr, value, dim*sizeof(T));
			};
		}

		VectorParameter(const std::string& name, const std::string& label, const unsigned int dim, ParameterBase::GetVecFunc<T> getValue, ParameterBase::SetVecFunc<T> setValue)
			: ParameterBase(name, label, ParameterBase::INT32),
			m_dim(dim),
			m_getVecValue(getValue),
			m_setVecValue(setValue)
		{
			setType(getValue()[0]);
		}

		virtual ~VectorParameter() {}

		void setValue(T *v)
		{
			if (m_setVecValue != nullptr)
				m_setVecValue(v);
		}
		T* getValue() const { return m_getVecValue(); }

		virtual void setReadOnly(const bool val)
		{
			m_readOnly = val;
			if (m_setVecValue == nullptr)
				m_readOnly = true;
		}
		
		template<typename TN>
		void setType(TN v) {}

		void setType(char v) { m_type = DataTypes::VEC_INT8; }
		void setType(short v) { m_type = DataTypes::VEC_INT16; }
		void setType(int v) { m_type = DataTypes::VEC_INT32; }
		void setType(unsigned char v) { m_type = DataTypes::VEC_UINT8; }
		void setType(unsigned short v) { m_type = DataTypes::VEC_UINT16; }
		void setType(unsigned int v) { m_type = DataTypes::VEC_UINT32; }
		void setType(float v) { m_type = DataTypes::VEC_FLOAT; }
		void setType(double v) { m_type = DataTypes::VEC_DOUBLE; }

		unsigned int getDim() const { return m_dim; }
	};

	using FloatVectorParameter = VectorParameter<float>;
	using DoubleVectorParameter = VectorParameter<double>;
	using LongDoubleVectorParameter = VectorParameter<long double>;
	using CharVectorParameter = VectorParameter<unsigned char>;
	using UnsignedShortVectorParameter = VectorParameter<unsigned short>;
	using UnsignedIntVectorParameter = VectorParameter<unsigned int>;
	using UnsignedLongVectorParameter = VectorParameter<unsigned long>;
	using SignedCharVectorParameter = VectorParameter<signed char>;
	using ShortVectorParameter = VectorParameter<signed short>;
	using IntVectorParameter = VectorParameter<signed int>;
	using LongVectorParameter = VectorParameter<signed long>;
}

#endif
