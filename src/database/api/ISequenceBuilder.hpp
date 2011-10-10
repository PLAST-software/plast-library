/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _ISEQUENCE_BUILDER_HPP_
#define _ISEQUENCE_BUILDER_HPP_

/********************************************************************************/

#include "ISequence.hpp"
#include "SmartPointer.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Forward references. */
class ISequence;

/********************************************************************************/
/** We define a class that can modify a ISequence instance internals.
 */
class ISequenceBuilder: public dp::SmartPointer
{
public:

    /** Accessor to the ISequence. */
    virtual ISequence* getSequence  () = 0;

    /** Encoding of the data to be built. */
    virtual Encoding getEncoding () = 0;

    /** Set the comment for the current sequence being built. */
    virtual void setComment (const char* buffer, size_t length) = 0;

    /** Reset the data for the current sequence being built. */
    virtual void resetData  () = 0;

    /** Add some information for the current sequence being built.
     *  Note that a potential encoding conversion will have to be done from
     *  the encoding scheme of the provided data to the encoding scheme of the
     *  builder. */
    virtual void addData (const LETTER* data, size_t size, Encoding encoding) = 0;

    /** Some post treatment capabilities. */
    virtual void postTreamtment () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_BUILDER_HPP_ */
