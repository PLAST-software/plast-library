#ifndef _ENVIRONMENT_FACTORY_HPP
#define _ENVIRONMENT_FACTORY_HPP

#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/IterativeAlgoEnvironment.hpp>

#include <set>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

class EnviromentFactory {
public:
    static IEnvironment* createEnvironment(dp::IProperties* properties,
            bool& isRunning) {
        dp::IProperty* stepsProperty = properties->getProperty(STR_OPTION_ITERATIONS_STEPS);
        if (stepsProperty != NULL) {
            return new IterativeAlgoEnvironment(properties, isRunning);
        }

        return new DefaultEnvironment(properties, isRunning);
    }
};

} // namespace impl
} // namespace core
} // namespace algo

#endif
