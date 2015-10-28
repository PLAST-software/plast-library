#include <algo/core/impl/SingleIterationAlgoEnvironment.hpp>

#include <designpattern/api/IProperty.hpp>

namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {

SingleIterationAlgoEnvironment::SingleIterationAlgoEnvironment (dp::IProperties* properties,
        bool& isRunning,
        std::set<u_int64_t>& blacklist,
        IResultVisitorsFactory* resultVisitorsFactory)
    : DefaultEnvironment(properties, isRunning),
    _blacklist(blacklist),
    _resultVisitorsFactory(0)
{
    setResultVisitorsFactory(resultVisitorsFactory);
}

SingleIterationAlgoEnvironment::~SingleIterationAlgoEnvironment ()
{
    setConfig (0);
    setResultVisitorsFactory(0);
}

IConfiguration* SingleIterationAlgoEnvironment::createConfiguration (dp::IProperties* properties)
{
    return new IterativePlastnConfig (this, properties, _blacklist);
}

void SingleIterationAlgoEnvironment::flushResults()
{
}

IResultVisitorsFactory* SingleIterationAlgoEnvironment::getResultsVisitorFactory()
{
    return _resultVisitorsFactory;
}

} // namespace impl
} // namespace core
} // namespace algo
