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

/**
 * Interface for factories for IAlignmentContainerVisitor.
 */
class IResultVisitorsFactory : public dp::SmartPointer {
public:
    /**
     * Obtain an instance of IAlignmentContainerVisitor.
     *
     * Note that it might be an old one or a new one.
     */
    virtual alignment::core::IAlignmentContainerVisitor* getInstance(dp::IProperties* properties,
            algo::core::IDatabasesProvider* databaseProvider) = 0;
};


} // namespace impl
} // namespace core
} // namespace algo

#endif // _IRESULT_VISITORS_FACTORY_HPP_
