#ifndef _RESULT_VISITORS_FACTORY_HPP_
#define _RESULT_VISITORS_FACTORY_HPP_

/********************************************************************************/

#include <algo/core/api/IDatabasesProvider.hpp>
#include <algo/core/api/IResultVisitorsFactory.hpp>

#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/**
 * Factory for IAlignmentContainerVisitor.
 */
class ResultVisitorsFactory : public IResultVisitorsFactory {
public:
    /** Get an alignment visitor (likely for the gap alignments and likely a
     * visitor that dump the alignments into a file).
     * \return a new or an existing IAlignmentContainerVisitor instance.
     */
    alignment::core::IAlignmentContainerVisitor* getInstance(dp::IProperties* properties, algo::core::IDatabasesProvider* databaseProvider);

private:
    /** Create a visitor for the gap alignments (likely a visitor that dump the alignments into a file).
     * \return a new AlignmentResultVisitor instance
     */
    alignment::core::IAlignmentContainerVisitor* createResultVisitor (dp::IProperties* properties, algo::core::IDatabasesProvider* databaseProvider);

    alignment::core::IAlignmentContainerVisitor* createSimpleResultVisitor (const std::string& uri, int outfmt);

    alignment::core::IAlignmentContainerVisitor* createAlgorithmResultVisitor (dp::IProperties* properties, const std::string& uri, int outfmt);
};


} // namespace impl
} // namespace core
} // namespace algo

#endif // _RESULT_VISITORS_FACTORY_HPP_
