#ifndef _ITERATIVE_ALGO_ENVIRONMENT_HPP_
#define _ITERATIVE_ALGO_ENVIRONMENT_HPP_

#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/impl/SingleIterationAlgoEnvironment.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/**
 * Algorithm environment (see IEnvironment) for the case when multiple iterations/steps
 * are required. Each step is trying to find at least one match of all so far
 * unresolved queries.
 */
class IterativeAlgoEnvironment : public IEnvironment
{
public:


    /** Constructor. */
    IterativeAlgoEnvironment (dp::IProperties* properties, bool& isRunning);

    /** Destructor. */
    virtual ~IterativeAlgoEnvironment ();

    /** No operation */
    virtual void configure ();

    /** Runs a number of iterations of the algorithm (see IEnvironment::run for detail)
     *    with some subset of the all possible seeds (see SeedMaskGenerator for details). */
    virtual void run ();

    /** \copydoc IEnvironment::getQuickSubjectDbReader */
    virtual database::IDatabaseQuickReader* getQuickSubjectDbReader ();

    /** \copydoc IEnvironment::getQuickQueryDbReader */
    virtual database::IDatabaseQuickReader* getQuickQueryDbReader   ();

    /** \copydoc IEnvironment::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** \copydoc IEnvironment::createConfiguration */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties);

    /** \copydoc IEnvironment::createAlgorithm */
    virtual std::list<IAlgorithm*> createAlgorithm (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        seed::ISeedModel*                               seedModel,
        algo::core::IDatabasesProvider*                 dbProvider,
        algo::core::IIndexator*                         indexator,
        statistics::IGlobalParameters*                  globalStats,
        os::impl::TimeInfo*                             timeInfo,
        bool&                                           isRunning
    );

    void setProperties(dp::IProperties* properties) { SP_SETATTR(properties); }

private:
    dp::IProperties* _properties;

    bool& _isRunning;

};

} // namespace impl
} // namespace core
} // namespace algo

#endif
