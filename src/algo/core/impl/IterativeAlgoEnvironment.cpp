#include <algo/core/impl/IterativeAlgoEnvironment.hpp>
#include <algo/core/impl/SingleIterationAlgoEnvironment.hpp>

#include <algo/core/impl/SingleResultVisitorFactory.hpp>

#include <designpattern/impl/TokenizerIterator.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

IterativeAlgoEnvironment::IterativeAlgoEnvironment (dp::IProperties* properties, bool& isRunning)
    : _properties(0), _isRunning(isRunning)
{
    setProperties(properties);
}

IterativeAlgoEnvironment::~IterativeAlgoEnvironment ()
{
    setProperties(0);
}

void IterativeAlgoEnvironment::configure ()
{
    // NOTE ipetrov: All the configuration is executed upon run. This step is
    // unnecessary for this implementation and is therefore left empty.
}

/** */
void IterativeAlgoEnvironment::run ()
{
    algo::core::impl::SingleResultVisitorFactory* visitorFactory = new algo::core::impl::SingleResultVisitorFactory();
    LOCAL(visitorFactory);

    dp::IProperty* iterationSteps = _properties->getProperty(STR_OPTION_ITERATIONS_STEPS);

    if (iterationSteps == NULL) {
        throw "Internal error! Iterative algorithm started without required parameters";
    }

    std::set<u_int64_t> foundQueryIndexes;

    dp::impl::TokenizerIterator it (iterationSteps->getString(), ",");
    for (it.first(); !it.isDone(); it.next())
    {
        std::string kmersToSelect = it.currentItem();
        dp::IProperties* currentStepProps = _properties->clone();
        LOCAL(currentStepProps);

        currentStepProps->add(1, STR_OPTION_KMERS_TO_SELECT, kmersToSelect);
        bool tmpIsRunning = _isRunning;
        SingleIterationAlgoEnvironment firstStep(currentStepProps, tmpIsRunning, foundQueryIndexes, visitorFactory);
        _currentStepEnvironment = &firstStep;

        _currentStepEnvironment->configure ();
        _currentStepEnvironment->run ();
        std::set<u_int64_t>* found = visitorFactory->getFoundQueryIndexes();
        foundQueryIndexes.insert(found->begin(), found->end());

        std::cout << "End step " << kmersToSelect << " " << found->size() << "\n";
    }

    std::cout << "Size " << foundQueryIndexes.size() << std::endl;

    _isRunning = false;
}

/** */
database::IDatabaseQuickReader* IterativeAlgoEnvironment::getQuickSubjectDbReader ()
{
    // NOTE ipetrov: For me this method should not be in the interface. It is put
    // there due to bad design decisions, so that it can be used by classes where
    // IEnviroment derived class has passed itself as an argument.
    throw "Unimplemented method IterativeAlgoEnvironment::getQuickSubjectDbReader";
}

database::IDatabaseQuickReader* IterativeAlgoEnvironment::getQuickQueryDbReader   ()
{
    // NOTE ipetrov: For me this method should not be in the interface. It is put
    // there due to bad design decisions, so that it can be used by classes where
    // IEnviroment derived class has passed itself as an argument.
    throw "Unimplemented method IterativeAlgoEnvironment::getQuickQueryDbReader";
}

void IterativeAlgoEnvironment::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    //_currentStepEnvironment->update(evt, subject);
}

IConfiguration* IterativeAlgoEnvironment::createConfiguration (dp::IProperties* properties)
{
    // NOTE ipetrov: For me this method should not be in the interface. It is put
    // there due to bad design decisions. It is strange to say the least to have
    // a protected method in an interface...
    throw "Unimplemented method IterativeAlgoEnvironment::createConfiguration";
}

/** */
std::list<IAlgorithm*> IterativeAlgoEnvironment::createAlgorithm (
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
    bool&                                           isRunning)
{
    // NOTE ipetrov: For me this method should not be in the interface. It is put
    // there due to bad design decisions. It is strange to say the least to have
    // a protected method in an interface...
    throw "Unimplemented method IterativeAlgoEnvironment::createAlgorithm";
}


} // namespace impl
} // namespace core
} // namespace algo
