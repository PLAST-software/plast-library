/** \file ConfigFileProperties.hpp
 *  \date 24/10/2014
 *  \author ipetrov
 *  \brief Implementation of the IProperties interface tuned for
 *  reading properties from a file.
 */

#ifndef _CONFIG_FILE_PROPERTIES_H_
#define _CONFIG_FILE_PROPERTIES_H_

#include <designpattern/api/IProperty.hpp>
#include <designpattern/impl/Property.hpp>

#include <set>
#include <string>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/**
 * Implementation of IProperties interface that can read its properties from a
 * file.  It tries to be slightly more intuitive than the Properties class when
 * considering the names of the properties.
 */
class ConfigFileProperties : public Properties
{
public:

    /** Constructor. If a file path is provided, it tries to read [key,value]
     * entries from this file.
     * \param initfile : the file (if any) to be read
     */
    ConfigFileProperties (std::set<std::string> allowedKeys, const char* initfile = 0);

    /** \copydoc IProperties::add(size_t,const std::string&,const std::string&)  */
    virtual IProperty* add (size_t depth, const std::string& aKey, const std::string& aValue);

private:

    // A set of strings that are allowed to be used as keys.
    std::set<std::string> _allowedKeys;

    // The name of the init file
    const char* _initfile;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif
