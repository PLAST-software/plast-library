#ifndef _ITERATIVE_PLASTN_CONFIG_HPP_
#define _ITERATIVE_PLASTN_CONFIG_HPP_

#include <algo/core/impl/PlastnAlgoConfig.hpp>

#include <set>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {

/********************************************************************************/

/**
 * Configuration (see IConfiguration) for the iterative algorithm environment.
 */
class IterativePlastnConfig: public PlastnConfiguration
{
public:
    /** Constructor */
    IterativePlastnConfig(IEnvironment* environment,
            dp::IProperties* properties,
            std::set<u_int64_t>& blacklist);

    /** Destructor */
    virtual ~IterativePlastnConfig ();

    /** \copydoc PlastnConfiguration::createDefaultParameters */
    IParameters* createDefaultParameters (const std::string& algoName);

private:

    // Unique identifiers of query sequences that should not be used.
    std::set<u_int64_t>& _blacklist;

    /** Update the value of kmersPerSequence in params, based on the value
     *  in the properties. */
    void updateKmersPerSequence(IParameters* params);
};

} // namespace impl
} // namespace core
} // namespace algo

#endif // _ITERATIVE_PLASTN_CONFIG_HPP_
