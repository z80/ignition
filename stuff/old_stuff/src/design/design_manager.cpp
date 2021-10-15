
#include "design_manager.h"
#include "tinyxml2.h"
#include <sstream>
#include <iomanip>
#include <set>

namespace Osp
{

static bool designNameExists( DesignManager & dm, const String & name );
static bool loadDesigns( DesignManager & dm );
static bool saveDesigns( DesignManager & dm );
static bool loadDesign( DesignManager & dm, const DesignManager::DesignItem & di, Design & design );
static bool saveDesign( DesignManager & dm, const DesignManager::DesignItem & di, Design & design );
static int  uniqueId( DesignManager & dm );
static String designFileName( int index );

DesignManager::DesignManager( Context * c )
    : Object( c )
{
    Osp::loadDesigns( *this );
}

DesignManager::~DesignManager()
{
    Osp::saveDesigns( *this );
}

bool DesignManager::saveDesign( const String & name,
                                const String & description,
                                const Design & design )
{
    DesignItem di;
    di.name = name;
    di.desc = description;
    di.id   = uniqueId( *this );
    designs.push_back( di );
    // And save design file to disk.
    const String fname =  designFileName( di.id );
    const bool savedOk = design.save( fname );
    Osp::saveDesigns( *this );
    return savedOk;
}

Design DesignManager::loadDesign( int index )
{
    const DesignItem & di = designs[index];
    const String fname =  designFileName( di.id );
    Design design;
    design.load( fname );
    return design;
}

bool DesignManager::renameDesign( int index,
                                  const String & nameNew )
{
    designs[index].name = nameNew;
    Osp::saveDesigns( *this );
    return true;
}

void DesignManager::setDescription( int index,
                                    const String & description )
{
    designs[index].desc = description;
    Osp::saveDesigns( *this );
}

void DesignManager::destroyDesign( int index )
{
    const int qty = (int)designs.size();
    const int lastInd = qty-1;
    if ( index < lastInd )
        designs[index] = designs[lastInd];
    designs.resize( lastInd );
    Osp::saveDesigns( *this );
}

std::vector<String> DesignManager::designNames()
{
    const int qty = (int)designs.size();
    std::vector<String> names;
    names.reserve( qty );
    for ( int i=0; i<qty; i++ )
        names.push_back( designs[i].name );

    return names;
}

const DesignManager::DesignItem & DesignManager::designItem( int ind )
{
    return designs[ind];
}







static bool designNameExists( DesignManager & dm,
                              const String & name )
{
    const std::vector<DesignManager::DesignItem> & designs = dm.designs;
    const size_t qty = designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignManager::DesignItem & di = designs[i];
        if ( di.name == name )
            return true;
    }

    return false;
}

static bool loadDesigns( DesignManager & dm )
{
    tinyxml2::XMLDocument doc;
    if ( doc.LoadFile( "./designs.xml" ) != tinyxml2::XML_SUCCESS )
        return false;

    dm.designs.clear();

    tinyxml2::XMLElement * rootE = doc.FirstChildElement( "designs" );
    while ( true )
    {
        tinyxml2::XMLElement * designE;
        if ( dm.designs.empty() )
            designE = rootE->FirstChildElement();
        else
            designE = designE->NextSiblingElement();
        if ( !designE )
            break;

        DesignManager::DesignItem di;
        di.name = designE->Attribute( "name" );
        {
            std::istringstream in( designE->Attribute( "id" ) );
            in >> di.id;
        }
        di.desc = designE->GetText();
        dm.designs.push_back( di );
    }

    return true;
}

static bool saveDesigns( DesignManager & dm )
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * rootE = doc.NewElement( "designs" );
    doc.InsertFirstChild( rootE );
    const size_t qty = dm.designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignManager::DesignItem & di = dm.designs[i];
        tinyxml2::XMLElement * designE = doc.NewElement( "design" );
        designE->SetAttribute( "name", di.name.CString() );
        {
            std::ostringstream out;
            out << di.id;
            designE->SetAttribute( "id", out.str().c_str() );
        }
        designE->SetText( di.desc.CString() );

        if ( i==0 )
            rootE->InsertFirstChild( designE );
        else
            rootE->InsertEndChild( designE );
    }
    doc.SaveFile( "./designs.xml" );
    return true;
}

static bool loadDesign( DesignManager & dm, int index )
{

}

static bool saveDesign( DesignManager & dm, int index )
{
    return false;
}

static int  uniqueId( DesignManager & dm )
{
    std::set<int> ids;
    const size_t qty = dm.designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignManager::DesignItem & di = dm.designs[i];
        ids.insert( di.id );
    }

    int id = 0;
    while ( true )
    {
        std::set<int>::const_iterator it = ids.find( id );
        if ( it == ids.end() )
            break;
        id += 1;
    }

    return id;
}

static String designFileName( int index )
{
    std::ostringstream out;
    out << "./" << std::setw( 6 ) << std::setfill( '0' )
        << index << ".xml";
    return out.str().c_str();
}




}




