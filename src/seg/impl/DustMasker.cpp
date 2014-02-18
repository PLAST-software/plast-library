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

#include "DustMasker.hpp"
#include <stdio.h>

#define MAX(a,b)  ((a)>(b) ? (a) : (b))

using namespace std;
using namespace misc;

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
extern "C" void DustMasker_filterSequence (char* s, int len)
{
	DustMasker dust (DustMasker::DEFAULT_LEVEL, DustMasker::DEFAULT_WINDOW, DustMasker::DEFAULT_LINKER);
	std::vector<misc::RangeU32> regions;

    dust.compute (s, len, regions);

    if (regions.empty() == false)
    {
    	for (std::vector<misc::RangeU32>::iterator it = regions.begin();  it != regions.end(); it++)
    	{
    		for (size_t i=it->begin; i<=it->end; i++)  { s[i] = 'N'; }
    	}
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
DustMasker::DustMasker (
    u_int32_t level,
    size_t window,
    size_t linker
)
: level_  ( (level  >= 2 && level  <= 64) ? level  : DEFAULT_LEVEL),
  window_ ( (window >= 8 && window <= 64) ? window : DEFAULT_WINDOW),
  linker_ ( (linker >= 1 && linker <= 32) ? linker : DEFAULT_LINKER),
  low_k_  ( level_/5 )
{
    thresholds_.reserve   (window_ - 2);
    thresholds_.push_back (1);

    for (size_t i = 1; i < window_ - 2; ++i )  {  thresholds_.push_back( i*level_ );  }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void DustMasker::save_masked_regions (std::vector<RangeU32>& res, size_t wstart, size_t start )
{
    if( !P.empty() )
    {
        RangeU32 b = P.back().bounds_;

        if( b.begin < wstart )
        {
            RangeU32 b1 (b.begin + start, b.end + start);

            if( !res.empty() )
            {
                size_t s = res.back().end;

                if (s + linker_ >= b1.begin)    {   res.back().end = MAX ( s, b1.end );   }
                else                            {   res.push_back( b1 );                  }
            }
            else
            {
                res.push_back( b1 );
            }

            while( !P.empty() && P.back().bounds_.begin < wstart )   {  P.pop_back();  }
        }
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void  DustMasker::compute (const char* seq, size_t len, vector<RangeU32>& res)
{
    res.clear ();

    size_t start = 0;
    size_t stop  = len - 1;

    vector<u_int8_t> counts (64);

    while (stop > 2 + start)    // there must be at least one triplet
    {
        // initializations
        P.clear();
        triplets w (window_, low_k_, P, thresholds_);

        size_t it = start;

        char c1 = seq[it];
        char c2 = seq[++it];

        u_int8_t t = (converter_( c1 )<<2) + converter_( c2 );

        it = start + w.stop() + 2;

        bool done = false;
        while (!done && it <= stop)
        {
            save_masked_regions (res, w.start(), start);

            // shift the window
            t = ((t<<2)&TRIPLET_MASK) + (converter_( seq[it] )&0x3);
            ++it;

            if (w.shift_window (t))
            {
                if (w.needs_processing())   {  w.find_perfect (counts);  }
            }
            else
            {
                while (it <= stop)
                {
                    save_masked_regions (res, w.start(), start);
                    t = ((t<<2)&TRIPLET_MASK) + (converter_( seq[it] )&0x3);

                    if (w.shift_window (t))
                    {
                        done = true;
                        break;
                    }

                    ++it;
                }
            }
        }

        // append the rest of the perfect intervals to the result
        size_t wstart = w.start();

        while (!P.empty())
        {
            save_masked_regions (res, wstart, start);
            ++wstart;
        }

        if (w.start() > 0)  {  start += w.start(); }
        else                { break;               }
    }
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DustMasker::triplets::triplets (
    size_t              window,
    u_int8_t            low_k,
    list<perfect>&      perfect_list,
    vector<u_int32_t>&  thresholds
)
    : start_( 0 ), stop_( 0 ), max_size_( window - 2 ), low_k_( low_k ),
      L( 0 ), P( perfect_list ), thresholds_( thresholds ),
      c_w( 64, 0 ), c_v( 64, 0 ),
      r_w( 0 ), r_v( 0 ), num_diff( 0 )
{}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool DustMasker::triplets::shift_high (u_int8_t t)
{
    u_int8_t s = triplet_list_.back();
    triplet_list_.pop_back();
    rem_triplet_info (r_w, c_w, s);
    if( c_w[s] == 0 ) --num_diff;
    ++start_;

    triplet_list_.push_front( t );
    if( c_w[t] == 0 ) ++num_diff;
    add_triplet_info( r_w, c_w, t );
    ++stop_;

    if (num_diff <= 1)
    {
        P.insert( P.begin(), perfect( start_, stop_ + 1, 0, 0 ) );
        return false;
    }
    else
    {
        return true;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool DustMasker::triplets::shift_window (u_int8_t t)
{
    // shift the left end of the window, if necessary
    if( triplet_list_.size() >= max_size_ )
    {
        if( num_diff <= 1 ) {  return shift_high( t );  }

        u_int8_t s = triplet_list_.back();
        triplet_list_.pop_back();
        rem_triplet_info( r_w, c_w, s );
        if( c_w[s] == 0 ) --num_diff;

        if( L == start_ )
        {
            ++L;
            rem_triplet_info( r_v, c_v, s );
        }

        ++start_;
    }

    triplet_list_.push_front( t );
    if( c_w[t] == 0 ) ++num_diff;
    add_triplet_info( r_w, c_w, t );
    add_triplet_info( r_v, c_v, t );

    if( c_v[t] > low_k_ )
    {
        u_int32_t off = triplet_list_.size() - (L - start_) - 1;

        do {
            rem_triplet_info( r_v, c_v, triplet_list_[off] );
            ++L;
        }  while( triplet_list_[off--] != t );
    }

    ++stop_;

    if( triplet_list_.size() >= max_size_ && num_diff <= 1 )
    {
        P.clear();
        P.insert( P.begin(), perfect( start_, stop_ + 1, 0, 0 ) );
        return false;
    }
    else
    {
        return true;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DustMasker::triplets::find_perfect (vector<u_int8_t>& counts)
{
    u_int32_t count = stop_ - L; // count is the suffix length

    // we need a local copy of triplet counts
    copy( c_v.begin(), c_v.end(), counts.begin() );

    u_int32_t score = r_v; // and of the partial sum
    list<perfect>::iterator perfect_iter = P.begin();
    u_int32_t max_perfect_score = 0;

    size_t max_len = 0;
    size_t pos = L - 1; // skipping the suffix

    deque<u_int8_t>::const_iterator it   = triplet_list_.begin() + count; // skipping the suffix
    deque<u_int8_t>::const_iterator iend = triplet_list_.end();

    for ( ; it != iend; ++it, ++count, --pos)
    {
        u_int8_t cnt = counts[*it];
        add_triplet_info ( score, counts, *it );

        if( cnt > 0 && score*10 > thresholds_[count] )
        {
            // found the candidate for the perfect interval
            // get the max score for the existing perfect intervals within
            //      current suffix
            while (perfect_iter != P.end() && pos <= perfect_iter->bounds_.begin)
            {
                if (max_perfect_score == 0 || max_len*perfect_iter->score_ > max_perfect_score*perfect_iter->len_)
                {
                    max_perfect_score = perfect_iter->score_;
                    max_len = perfect_iter->len_;
                }

                ++perfect_iter;
            }

            // check if the current suffix score is at least as large
            if (max_perfect_score == 0 || score*max_len >= max_perfect_score*count )
            {
                max_perfect_score = score;
                max_len = count;
                perfect_iter = P.insert (perfect_iter, perfect( pos, stop_ + 1, max_perfect_score, count ) );
            }
        }
    }
}
