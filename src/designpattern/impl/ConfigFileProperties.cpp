/** \file ConfigFileProperties.cpp
 *  \date 27/10/2014
 *  \author ipetrov
 *  \brief Implementation of the IProperties interface tuned for
 *  reading properties from a file.
 */

#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/ConfigFileProperties.hpp>

#include<string>

/********************************************************************************/
namespace dp { namespace impl {
/********************************************************************************/

ConfigFileProperties::ConfigFileProperties (std::set<std::string> allowedKeys, const char* initfile)
        : _allowedKeys(allowedKeys)
{
    _initfile = initfile;
    if (_initfile != 0)
    {
        readFile(_initfile);
    }
}

IProperty* ConfigFileProperties::add (size_t depth, const std::string& aKey, const std::string& aValue)
{
    std::string actualKey = aKey;
    // we expect that the name of the keys start with a '-', but for better usability we allow
    // keys without '-' to be used as well.
    if (actualKey.size() > 0 &&  actualKey[0] != '-')
    {
        actualKey = "-" + actualKey;
    }

    // The key is not allowed
    if ( _allowedKeys.find(actualKey) == _allowedKeys.end() )
    {
        std::cerr << "Unknown key met in file " << _initfile << std::endl;

        return NULL;
    }

    return Properties::add(depth, actualKey, aValue);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
