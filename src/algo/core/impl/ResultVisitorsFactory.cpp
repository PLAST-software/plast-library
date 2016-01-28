#include <algo/core/impl/ResultVisitorsFactory.hpp>
#include <database/api/IDatabaseQuickReader.hpp>
#include <database/impl/FastaDatabaseQuickReader.hpp>

#include <alignment/visitors/impl/OstreamVisitor.hpp>
#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>
#include <alignment/visitors/impl/RawOutputVisitor.hpp>
#include <alignment/visitors/impl/XmlOutputVisitor.hpp>
#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>
#include <alignment/visitors/impl/FilterContainerVisitor.hpp>
#include <alignment/visitors/impl/ProxyVisitor.hpp>
#include <alignment/visitors/impl/NucleotidConversionVisitor.hpp>
#include <alignment/visitors/impl/QueryReorderVisitor.hpp>

#define DEBUG(a)  //a

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

alignment::core::IAlignmentContainerVisitor* ResultVisitorsFactory::getInstance(dp::IProperties* properties, algo::core::IDatabasesProvider* databaseProvider)
{
    return createResultVisitor(properties, databaseProvider);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
alignment::core::IAlignmentContainerVisitor* ResultVisitorsFactory::createResultVisitor(dp::IProperties* properties, algo::core::IDatabasesProvider* databaseProvider)
{
    alignment::core::IAlignmentContainerVisitor* result = 0;
    dp::IProperty* prop = 0;

    /** We need an uri. We take the one provided by the properties. */
    std::string uri ("stdout");
    if ( (prop = properties->getProperty(STR_OPTION_OUTPUT_FILE)) != 0) {
        uri = prop->value;
    }

    /** We need an output format. */
    int outfmt = 1;
    if ( (prop = properties->getProperty(STR_OPTION_OUTPUT_FORMAT)) != 0) {
        outfmt = prop->getInt();
    }

    DEBUG (std::cout << "DefaultConfiguration::createResultVisitor  outfmt=" << outfmt << "  uri=" << uri << std::endl);

    /** We may have to modify the query order. So we may encapsulate the "normal" result by another visitor
     *  that will reorder the alignments. */
    dp::IProperty* forceQryOrdering = properties->getProperty(STR_OPTION_FORCE_QUERY_ORDERING);

    int64_t nbAlignPerNotif = -1;

    if (forceQryOrdering != 0) {
        nbAlignPerNotif = forceQryOrdering->getInt();
    }

    if (nbAlignPerNotif >= 0) {
        if (nbAlignPerNotif == 0) { nbAlignPerNotif = 10*1000; }

        size_t nbHitPerQuery =
            (prop = properties->getProperty(STR_OPTION_MAX_HIT_PER_QUERY)) != 0 ? prop->getInt() : 500;
        size_t nbAlignPerHit =
            (prop = properties->getProperty(STR_OPTION_MAX_HSP_PER_HIT))   != 0 ? prop->getInt() : 0;

        dp::IProperty* queryProp = properties->getProperty (STR_OPTION_QUERY_URI);

        u_int64_t  maxblocksize = 20*1000*1000;
        dp::IProperty* maxBlockProp = properties->getProperty(STR_OPTION_MAX_DATABASE_SIZE);
        if (maxBlockProp != 0) {
            maxblocksize = maxBlockProp->getInt();
        }

        database::IDatabaseQuickReader* queryQuickReader =
            new database::impl::FastaDatabaseQuickReader(queryProp->value, true);
        queryQuickReader->read (maxblocksize);

        result = new alignment::visitors::impl::QueryReorderVisitor(
            databaseProvider,
            uri,
            createAlgorithmResultVisitor(properties, uri + ".tmp", 3), // visitor wanted by user with a forced outfmt
            createSimpleResultVisitor(uri, outfmt),    // visitor for final dump with the user outfmt
            queryQuickReader,
            (u_int32_t)nbAlignPerNotif,
            nbHitPerQuery, nbAlignPerHit);
    } else {
        /** We create the result. */
        result = createAlgorithmResultVisitor (properties, uri, outfmt);
    }

    /** We return the result. */
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
alignment::core::IAlignmentContainerVisitor* ResultVisitorsFactory::createSimpleResultVisitor (const std::string& uri, int outfmt)
{
    alignment::core::IAlignmentContainerVisitor* result = 0;

    switch (outfmt) {
        case 2:     result = new  alignment::visitors::impl::TabulatedOutputExtendedVisitor (uri);     break;
        case 3:     result = new  alignment::visitors::impl::RawOutputVisitor               (uri);     break;
        case 4:     result = new  alignment::visitors::impl::XmlOutputVisitor               (uri);     break;
        case 1:
        default:    result = new  alignment::visitors::impl::TabulatedOutputVisitor         (uri);     break;
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
alignment::core::IAlignmentContainerVisitor* ResultVisitorsFactory::createAlgorithmResultVisitor (dp::IProperties* properties, const std::string& uri, int outfmt)
{
    alignment::core::IAlignmentContainerVisitor* result = createSimpleResultVisitor (uri, outfmt);

    /** Now, we have to take into account the kind of the algorithm (plastp, plastx...)
     *  since we may have to convert back to nucleotid alphabet. */
    dp::IProperty* prop = properties->getProperty (STR_OPTION_ALGO_TYPE);
    if (prop != 0) {
        std::string algoName = prop->getValue();

        if (algoName.compare("tplastn")==0) {
            result = new alignment::visitors::impl::NucleotidConversionVisitor (result, alignment::core::Alignment::SUBJECT);
        }
        else if (algoName.compare("plastx")==0) {
            result = new alignment::visitors::impl::NucleotidConversionVisitor (result, alignment::core::Alignment::QUERY);
        }
        else if (algoName.compare("tplastx")==0) {
            result = new alignment::visitors::impl::NucleotidConversionVisitor(
                new alignment::visitors::impl::NucleotidConversionVisitor (result, alignment::core::Alignment::SUBJECT),
                alignment::core::Alignment::QUERY);
        }
    }

    /** We return the result. */
    return result;
}

} // namespace impl
} // namespace core
} // namespace algo
