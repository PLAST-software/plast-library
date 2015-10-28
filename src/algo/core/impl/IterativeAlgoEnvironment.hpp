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
 */
class IterativeAlgoEnvironment : public IEnvironment
{
public:


    /** Constructor. */
    IterativeAlgoEnvironment (dp::IProperties* properties, bool& isRunning);

    /** Destructor. */
    virtual ~IterativeAlgoEnvironment ();

    /** */
    virtual void configure ();

    /** */
    virtual void run ();

    /** */
    virtual database::IDatabaseQuickReader* getQuickSubjectDbReader ();
    virtual database::IDatabaseQuickReader* getQuickQueryDbReader   ();

    /** \copydoc IEnvironment::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties);

    /** */
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
    SingleIterationAlgoEnvironment* _currentStepEnvironment;

    dp::IProperties* _properties;

    bool& _isRunning;

};

} // namespace impl
} // namespace core
} // namespace algo

#endif
