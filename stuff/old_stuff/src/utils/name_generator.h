
#ifndef __NAME_GENERATOR_H_
#define __NAME_GENERATOR_H_

#include <Urho3D/Urho3DAll.h>
#include <map>

using namespace Urho3D;

/**
 * \brief	A utility class to generate unique names.
**/
class NameGenerator
{
public:

    /**
     * \brief	Gets the next unique name.
    **/
    static String Next();

    /**
     * \brief	Gets the next unique name for a given prefix.
    **/
    static String Next( const String & prefix );

    /**
     * \brief	Counts the number of unique auto-generated names.
    **/
    static size_t Count();

    /**
     * \brief	Counts the number of names generated for a given prefix.
     * \param	prefix	The prefix of the generated names.
    **/
    static size_t Count(const String & prefix);
private:
    NameGenerator(void);
    ~NameGenerator(void);

    typedef std::map<String, size_t> NameCountMap;
    static NameCountMap s_nameCount;
};




#endif


