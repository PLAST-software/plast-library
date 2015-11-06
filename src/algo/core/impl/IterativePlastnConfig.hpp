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

class IterativePlastnConfig: public PlastnConfiguration
{
public:
    IterativePlastnConfig(IEnvironment* environment,
            dp::IProperties* properties,
            std::set<u_int64_t>& blacklist);

    /** Destructor. */
    virtual ~IterativePlastnConfig ();

    IParameters* createDefaultParameters (const std::string& algoName);

private:

    std::set<u_int64_t>& _blacklist;

    void updateKmersBitsetPath(IParameters* params);
};

} // namespace impl
} // namespace core
} // namespace algo

#endif // _ITERATIVE_PLASTN_CONFIG_HPP_
