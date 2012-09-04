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

#ifndef DUSTMASKER_HPP_
#define DUSTMASKER_HPP_

#include <misc/api/types.hpp>

#include <deque>
#include <list>
#include <vector>

/********************************************************************************/

class DustMasker
{
public:

    /**< Default value of score threshold. */
    static const u_int32_t DEFAULT_LEVEL  = 20;

    /**< Default window size. */
    static const u_int32_t DEFAULT_WINDOW = 64;

    /**< Default value of the longest distance between consecutive masked intervals at which they should be merged. */
    static const u_int32_t DEFAULT_LINKER = 80;

    /** */
    DustMasker (
        u_int32_t level  = DEFAULT_LEVEL,
        size_t    window = static_cast<size_t>(DEFAULT_WINDOW),
        size_t    linker = static_cast<size_t>(DEFAULT_LINKER)
    );

    /** */
    void  compute (const char* seq, size_t len, std::vector<misc::RangeU32>& regions);

private:

    /**\brief Selects the significant bits in triplet_type. */
    static const u_int8_t TRIPLET_MASK = 0x3F;

    /** */
    void save_masked_regions (std::vector<misc::RangeU32>& res, size_t w, size_t start);

    /********************************************************************************/
    struct convert_t
    {
        u_int8_t operator() (u_int8_t r) const
        {
            switch (r)
            {
                case 67: return 1;
                case 71: return 2;
                case 84: return 3;
                default: return 0;
            }
        }
    } converter_;

    /********************************************************************************/
    struct perfect
    {
        misc::RangeU32 bounds_;
        u_int32_t      score_;
        size_t         len_;

        perfect (size_t start, size_t stop, u_int32_t score, size_t len )
            : bounds_( start, stop ), score_( score ), len_( len ) {}
    };

    /********************************************************************************/
    class triplets
    {
    public:

        /** */
        triplets (size_t window, u_int8_t  low_k, std::list<perfect>& perfect_list, std::vector<u_int32_t>& thresholds);

        /** */
        size_t start() const { return start_;               }
        size_t stop()  const { return stop_;                }
        size_t size()  const { return triplet_list_.size(); }

        /** */
        void find_perfect ();

        /** */
        bool shift_window (u_int8_t t);
        bool shift_high   (u_int8_t t);

        /** */
        bool needs_processing () const
        {
            u_int32_t count = stop_ - L;
            return count < triplet_list_.size() && 10*r_w > thresholds_[count];
        }

    private:

        /** */
        void add_triplet_info (u_int32_t & r, std::vector<u_int8_t>& c, u_int8_t t)  { r += c[t]; ++c[t]; }
        void rem_triplet_info (u_int32_t & r, std::vector<u_int8_t>& c, u_int8_t t)  { --c[t]; r -= c[t]; }

        std::deque<u_int8_t> triplet_list_;

        size_t    start_;
        size_t    stop_;
        size_t    max_size_;
        u_int8_t  low_k_;
        u_int32_t L;

        std::list <perfect>& P;
        std::vector <u_int32_t>& thresholds_;

        std::vector<u_int8_t> c_w;
        std::vector<u_int8_t> c_v;

        u_int32_t r_w;
        u_int32_t r_v;
        u_int32_t num_diff;
    };

    /********************************************************************************/
    u_int32_t level_;
    size_t    window_;
    size_t    linker_;
    u_int8_t  low_k_;

    std::list   <perfect>   P;
    std::vector <u_int32_t> thresholds_;
};

#endif /* DUSTMASKER_HPP_ */
