/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

#include <alignment/tools/impl/AlignmentContainerShrinkCmd.hpp>

using namespace std;
using namespace alignment;
using namespace alignment::core;

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool AlignmentContainerShrinkCmd::mysortfunction (const Alignment& i, const Alignment& j)
{
    return (i.getEvalue() <  j.getEvalue())  ||
           (i.getEvalue() == j.getEvalue()  &&  i.getBitScore() < j.getBitScore());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentContainerShrinkCmd::AlignmentContainerShrinkCmd (
    std::list<Alignment>& alignments,
    bool (*sort_cbk) (const Alignment& i, const Alignment& j)
)
    : _alignments (alignments), _sort_cbk(sort_cbk), _shiftDivisor(20), _nbRemoved(0)
{
    if (_sort_cbk == 0)  {  _sort_cbk = mysortfunction; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentContainerShrinkCmd::execute ()
{
    /** We keep the initial size. */
    size_t initSize = _alignments.size();

    vector<bool> removeTable (_alignments.size(), false);

    size_t k = 0;
    for (std::list<Alignment>::iterator itLevel3 = _alignments.begin(); itLevel3 != _alignments.end(); k++)
    {
        Alignment& a1 = *(itLevel3++);

        if (removeTable[k] == true)  { continue; }

        size_t l = k + 1;
        for (std::list<Alignment>::iterator itLevel3b = itLevel3; itLevel3b != _alignments.end(); l++)
        {
            Alignment& a2 = *(itLevel3b++);

            if (removeTable[l] == true)  { continue; }

            size_t shift = MIN (a1.getLength(), a2.getLength()) / _shiftDivisor;


            if ((a1.getSbjRange().end     + shift >= a2.getSbjRange().end     - shift) &&
                (a1.getSbjRange().begin   - shift <= a2.getSbjRange().begin   + shift) &&
                (a1.getQryRange().end     + shift >= a2.getQryRange().end     - shift) &&
                (a1.getQryRange().begin   - shift <= a2.getQryRange().begin   + shift) )
            {
                if (a1.getBitScore() > a2.getBitScore())
                {
                    removeTable [l] = true;
                }
                else
                {
                    removeTable [k] = true;
                }
            }

            else if ((a2.getSbjRange().end     + shift >= a1.getSbjRange().end     - shift) &&
                     (a2.getSbjRange().begin   - shift <= a1.getSbjRange().begin   + shift) &&
                     (a2.getQryRange().end     + shift >= a1.getQryRange().end     - shift) &&
                     (a2.getQryRange().begin   - shift <= a1.getQryRange().begin   + shift) )
            {

                if (a1.getBitScore() > a2.getBitScore())
                {
                    removeTable [l] = true;
                }
                else
                {
                    removeTable [k] = true;
                }
            }
        }

    }  /* end of for (InnerContainer::iterator itLevel3 = container.begin()... */

    k = 0;
    for (list<Alignment>::iterator itLevel3 = _alignments.begin(); itLevel3 != _alignments.end(); k++)
    {
        if (removeTable[k] == true)  {  itLevel3 = _alignments.erase (itLevel3);  }
        else                         {  itLevel3++;                               }
    }

    /** We may have to sort the list at the end of the process. */
    if (_sort_cbk != 0) {  _alignments.sort (_sort_cbk); }

    /** We memorize the number of removed items. */
    _nbRemoved = initSize - _alignments.size();

    DEBUG (("AlignmentContainerShrinkCmd::execute:  init=%ld  new=%ld  =>  diff=%ld \n",
        initSize, _alignments.size(), _nbRemoved
    ));
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
