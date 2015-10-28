#ifndef _IRESULT_VISITORS_FACTORY_HPP_
#define _IRESULT_VISITORS_FACTORY_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <algo/core/api/IDatabasesProvider.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

class IResultVisitorsFactory : public dp::SmartPointer {
public:
    virtual alignment::core::IAlignmentContainerVisitor* getInstance(dp::IProperties* properties,
            algo::core::IDatabasesProvider* databaseProvider) = 0;
};


} // namespace impl
} // namespace core
} // namespace algo

#endif // _IRESULT_VISITORS_FACTORY_HPP_
