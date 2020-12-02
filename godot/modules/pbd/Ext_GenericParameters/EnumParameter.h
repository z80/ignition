#ifndef __EnumParameter_h__
#define __EnumParameter_h__

#include <functional>
#include <memory>
#include "Parameter.h"
#include <vector>

namespace GenParam
{
	/** Class of an enum parameter. */
	class EnumParameter : public Parameter<int>
	{
	public: 
		using Ptr = std::unique_ptr<EnumParameter>;

		struct EnumValue
		{
			int id;
			std::string name;
		};

	protected:
		std::vector<EnumValue> m_enumValues;

	public:
		EnumParameter(const std::string& name, const std::string& label, int* valuePtr)
			: Parameter<int>(name, label, ParameterBase::ENUM, valuePtr)
		{
		}

		EnumParameter(const std::string& name, const std::string& label, GetFunc<int> getValue, SetFunc<int> setValue)
			: Parameter<int>(name, label, ParameterBase::ENUM, getValue, setValue)
		{
		}

		virtual ~EnumParameter() {}

		/** Add an enum value to this parameter type. */
		void addEnumValue(const std::string &name, int &id)
		{
			EnumValue val;
			val.name = name;
			id = static_cast<int>(m_enumValues.size());
			val.id = id;
			m_enumValues.push_back(val);
		}
		
		/** Clear all enum values.
		 */
		void clearEnumValues()
		{
			m_enumValues.clear();
		}

		const std::vector<EnumValue>& getEnumValues() { return m_enumValues;  }
	};
}

#endif
