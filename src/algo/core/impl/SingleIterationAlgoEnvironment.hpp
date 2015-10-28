#ifndef _SINGLE_ITERATION_ALGO_ENVIRONMENT_HPP_
#define _SINGLE_ITERATION_ALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoEnvironment.hpp>

#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/PlastnAlgoConfig.hpp>
#include <algo/core/impl/IterativePlastnConfig.hpp>
#include <algo/core/api/IResultVisitorsFactory.hpp>

#include <designpattern/api/IProperty.hpp>
#include <designpattern/api/SmartPointer.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/**
 */
class SingleIterationAlgoEnvironment : public DefaultEnvironment
{
public:
    SingleIterationAlgoEnvironment (dp::IProperties* properties,
            bool& isRunning,
            std::set<u_int64_t>& blacklist,
            IResultVisitorsFactory* resultVisitorsFactory);

    virtual ~SingleIterationAlgoEnvironment ();

    virtual IConfiguration* createConfiguration (dp::IProperties* properties);

protected:
    virtual void flushResults();

    virtual IResultVisitorsFactory* getResultsVisitorFactory();

private:
    std::set<u_int64_t>& _blacklist;

    IResultVisitorsFactory* _resultVisitorsFactory;

    void setResultVisitorsFactory(IResultVisitorsFactory* resultVisitorsFactory) { SP_SETATTR(resultVisitorsFactory); }
};

} // namespace impl
} // namespace core
} // namespace algo

#endif // _SINGLE_ITERATION_ALGO_ENVIRONMENT_HPP_
