#ifndef _SINGLE_RESULT_VISITORS_FACTORY_HPP_
#define _SINGLE_RESULT_VISITORS_FACTORY_HPP_

/********************************************************************************/

#include <algo/core/api/IDatabasesProvider.hpp>
#include <algo/core/api/IResultVisitorsFactory.hpp>
#include <algo/core/impl/ResultVisitorsFactory.hpp>

#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

#include <alignment/visitors/impl/FoundQuerySequencesGeneratingVisitor.hpp>

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

// TODO ipetrov: rename to something like IterativeAlgorithmResultVisitorFactory
// Otherwise the addition of FoundQuerySequencesGeneratingVisitor is inappropraite.
class SingleResultVisitorFactory : public IResultVisitorsFactory {
public:
    SingleResultVisitorFactory()
        : _singleton(0)
    {
    }

    ~SingleResultVisitorFactory()
    {
        setSingleton(0);
    }

    /**
     * NOTE ipetrov: Can not use the same factory to create two different instances.
     */
    alignment::core::IAlignmentContainerVisitor* getInstance(dp::IProperties* properties, algo::core::IDatabasesProvider* databaseProvider)
    {
        if (_singleton == NULL) {
            IResultVisitorsFactory* factory = new ResultVisitorsFactory();
            LOCAL(factory);

            alignment::core::IAlignmentContainerVisitor* result = factory->getInstance(properties, databaseProvider);
            result = new alignment::visitors::impl::FoundQuerySequencesGeneratingVisitor(&_foundQueryIndexes, result);
            setSingleton(result);
        }

        return _singleton;
    }

    std::set<u_int64_t>* getFoundQueryIndexes() {
        return &_foundQueryIndexes;
    }

private:
    // TODO ipetrov: Check where consts are good idea
    std::set<u_int64_t> _foundQueryIndexes;

    alignment::core::IAlignmentContainerVisitor* _singleton;

    void setSingleton(alignment::core::IAlignmentContainerVisitor* singleton) { SP_SETATTR(singleton); }
};


} // namespace impl
} // namespace core
} // namespace algo

#endif // _SINGLE_RESULT_VISITORS_FACTORY_HPP_
