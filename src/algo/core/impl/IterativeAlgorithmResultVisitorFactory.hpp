#ifndef _ITERATIVE_ALGORITHM_RESULT_VISITORS_FACTORY_HPP_
#define _ITERATIVE_ALGORITHM_RESULT_VISITORS_FACTORY_HPP_

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

/**
 * ResultVisitorsFactory that always returns the same instance of
 * FoundQuerySequencesGeneratingVisitor.
 */
class IterativeAlgorithmResultVisitorFactory : public IResultVisitorsFactory {
public:
    IterativeAlgorithmResultVisitorFactory()
        : _singleton(0)
    {
    }

    ~IterativeAlgorithmResultVisitorFactory()
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

    // Get a set of unique identifiers of each query sequence that has been
    // found so far.
    std::set<u_int64_t>* getFoundQueryIndexes() {
        return &_foundQueryIndexes;
    }

private:
    // The set of unique identifiers of each query sequence that has been found
    // so far.
    std::set<u_int64_t> _foundQueryIndexes;

    alignment::core::IAlignmentContainerVisitor* _singleton;

    void setSingleton(alignment::core::IAlignmentContainerVisitor* singleton) { SP_SETATTR(singleton); }
};


} // namespace impl
} // namespace core
} // namespace algo

#endif // _ITERATIVE_ALGORITHM_RESULT_VISITORS_FACTORY_HPP_
