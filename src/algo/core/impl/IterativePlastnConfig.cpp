#include <algo/core/impl/IterativePlastnConfig.hpp>
#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <alignment/visitors/impl/FoundQuerySequencesGeneratingVisitor.hpp>

#include <set>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {

/********************************************************************************/

IterativePlastnConfig::IterativePlastnConfig(IEnvironment* environment,
        dp::IProperties* properties,
        std::set<u_int64_t>& blacklist)
    : PlastnConfiguration(environment, properties), _blacklist(blacklist)
{
}

IterativePlastnConfig::~IterativePlastnConfig() { }

IParameters* IterativePlastnConfig::createDefaultParameters (const std::string& algoName)
{
    IParameters* params = PlastnConfiguration::createDefaultParameters(algoName);

    params->querySequencesBlacklist = &_blacklist;

    return params;
}

} // namespace impl
} // namespace core
} // namespace algo

