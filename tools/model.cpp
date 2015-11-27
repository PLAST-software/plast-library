/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/********************************************************************************/
#include <database/api/IAlphabet.hpp>

#include <misc/api/PlastStrings.hpp>
#include <misc/impl/OptionsParser.hpp>

#include <designpattern/impl/Property.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <map>

#define DEBUG(a)  printf a
#define INFO(a)   printf a

/********************************************************************************/

using namespace std;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int main (int argc, char* argv[])
{

    ISeedModel* subseedModel = new SubSeedModel (4,
        "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
        "HFYWIVLMC,RKQENDASTGP",
        "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
        "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
    );
    LOCAL (subseedModel);

    map<string,int>       mapSubseedString;
    ISeedIterator* itSubseedAll = subseedModel->createAllSeedsIterator();
    for (itSubseedAll->first(); !itSubseedAll->isDone(); itSubseedAll->next())
    {
        const ISeed* current = itSubseedAll->currentItem();
        if (mapSubseedString.find (current->kmer.toString()) == mapSubseedString.end())
        {
            mapSubseedString[current->kmer.toString()] = mapSubseedString.size();
        }
    }
    printf ("=> nbSubseeds=%d\n", mapSubseedString.size());

    /** We create a model. */
    ISeedModel* basicModel = new BasicSeedModel (SUBSEED,4);
    LOCAL (basicModel);

    ISeedIterator* itSeed = basicModel->createAllSeedsIterator();
    LOCAL (itSeed);

    size_t nbSeeds = 0;
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())  { nbSeeds++; }

    vector<SeedHashCode> subseedCodes;

    map<string,string>  basic2subseed;

    nbSeeds = 0;
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())
    {
        const ISeed* current = itSeed->currentItem();

        ISeedIterator* itSeed2 = subseedModel->createSeedsIterator (current->kmer);
        itSeed2->first();
        const ISeed* current2 = itSeed2->currentItem();

        basic2subseed [current->kmer.toString()] = current2->kmer.toString();

//        subseedCodes.push_back (mapSubseed[current2->code]);
    }
    printf ("==> nbSeeds=%d\n", subseedCodes.size());

    char table[] =  {'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y'};

    size_t nbTrans = 0;
    map<string,int> occurMap;
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())
    {
        const ISeed* current = itSeed->currentItem();

        //printf ("'%s' (%d)\n",  current->kmer.toString().c_str(), current->code);

        for (size_t j=0; j<sizeof(table)/sizeof(table[0]); j++)
        {
            string next = current->kmer.toString() +  table[j];
            next = next.substr (1, 4);
            //printf ("===> %s\n", next.c_str());

            string subFrom = basic2subseed[current->kmer.toString()];
            string subTo   = basic2subseed[next];

            occurMap[subTo] ++;
//            printf ("'%s'->'%s'   '%s'->'%s'  (%d->%d)\n",
//                current->kmer.toString().c_str(), next.c_str(),
//                subFrom.c_str(), subTo.c_str(),
//                mapSubseedString[subFrom], mapSubseedString[subTo]
//            );

            nbTrans++;
        }
    }

    printf ("occurMap=%d\n", occurMap.size());
    for (map<string,int>::iterator it = occurMap.begin(); it != occurMap.end(); it++)
    {
        cout << it->first << " " << it->second << endl;
    }

    return 0;
}
