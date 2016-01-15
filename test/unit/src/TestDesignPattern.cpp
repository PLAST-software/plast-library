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

#include "cppunit.h"

#include <designpattern/api/ICommand.hpp>

#include <designpattern/impl/ListIterator.hpp>
#include <designpattern/impl/ProductIterator.hpp>
#include <designpattern/impl/Observer.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/WrapperIterator.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>
#include <designpattern/impl/SystemCommand.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/XmlReader.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/IteratorGet.hpp>
#include <designpattern/impl/DirectoryIterator.hpp>

#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/TimeTools.hpp>

#include <string>
#include <sstream>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace dp;
using namespace dp::impl;

using namespace os;
using namespace os::impl;

extern const char* getPath (const char* file);

/********************************************************************************/

struct Info
{
    Info () : nbLines(0), checksum(0), cumulatedLength(0) {}

    friend std::ostream& operator<< (std::ostream& s, const Info& i)
    {
        s << "[INFO nbLines=" << i.nbLines << "  checksum=" << i.checksum  << "  cumulatedLength=" << i.cumulatedLength << "]";
        return s;
    }

    bool operator== (const Info& i) const
    {
        return  nbLines == i.nbLines  &&   checksum == i.checksum  && cumulatedLength == i.cumulatedLength;
    }

    size_t    nbLines;
    u_int64_t checksum;
    u_int64_t cumulatedLength;

};

/********************************************************************************/

class TestDesignPattern : public TestFixture, public IObserver
{
private:

public:

    /********************************************************************************/
    void setUp ()
    {
    }

    void tearDown ()
    {
    }

    /********************************************************************************/
    static Test* suite()
    {
         TestSuite* result = new TestSuite ("DesignPatternTest");

         result->addTest (new TestCaller<TestDesignPattern> ("testIterator1",           &TestDesignPattern::testIterator1 ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorProduct",     &TestDesignPattern::testIteratorProduct ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorProduct2",    &TestDesignPattern::testIteratorProduct2 ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorIterate",     &TestDesignPattern::testIteratorIterate ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testObserver1",           &TestDesignPattern::testObserver1 ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorObserver",    &TestDesignPattern::testIteratorObserver ) );
         result->addTest (new TestCaller<TestDesignPattern> ("testTokenizer",           &TestDesignPattern::testTokenizer) );
         result->addTest (new TestCaller<TestDesignPattern> ("testWrapper",             &TestDesignPattern::testWrapper) );
         result->addTest (new TestCaller<TestDesignPattern> ("testCommandFactories",    &TestDesignPattern::testCommandFactories) );
         result->addTest (new TestCaller<TestDesignPattern> ("testSystemCommand",       &TestDesignPattern::testSystemCommand) );
         result->addTest (new TestCaller<TestDesignPattern> ("testProperties",          &TestDesignPattern::testProperties) );
         result->addTest (new TestCaller<TestDesignPattern> ("testXmlReader",           &TestDesignPattern::testXmlReader) );
         result->addTest (new TestCaller<TestDesignPattern> ("testXmlFilterReader",     &TestDesignPattern::testXmlFilterReader) );
         result->addTest (new TestCaller<TestDesignPattern> ("testFileLineIterator1",   &TestDesignPattern::testFileLineIterator1) );
         result->addTest (new TestCaller<TestDesignPattern> ("testFileLineIterator2",   &TestDesignPattern::testFileLineIterator2) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorGet1",         &TestDesignPattern::testIteratorGet1) );
         result->addTest (new TestCaller<TestDesignPattern> ("testIteratorGet1",         &TestDesignPattern::testIteratorGet2) );
         result->addTest (new TestCaller<TestDesignPattern> ("testFileLineIterator3",   &TestDesignPattern::testFileLineIterator3) );
         result->addTest (new TestCaller<TestDesignPattern> ("testDirectoryIterator",   &TestDesignPattern::testDirectoryIterator) );
         result->addTest (new TestCaller<TestDesignPattern> ("testDirectoryIteratorRec",&TestDesignPattern::testDirectoryIteratorRecursive) );

         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIterator1 ()
    {
        list<int> l1;
        l1.push_back (1);
        l1.push_back (2);
        l1.push_back (4);

        ListIterator<int> it (l1);
        it.first();
        CPPUNIT_ASSERT (it.isDone() == false);

        CPPUNIT_ASSERT (it.currentItem() == 1);
        it.next();
        CPPUNIT_ASSERT (it.isDone() == false);

        CPPUNIT_ASSERT (it.currentItem() == 2);
        it.next();
        CPPUNIT_ASSERT (it.isDone() == false);

        CPPUNIT_ASSERT (it.currentItem() == 4);
        it.next();
        CPPUNIT_ASSERT (it.isDone() == true);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIteratorProduct ()
    {
        list<int> l1;
        l1.push_back (1);
        l1.push_back (2);
        l1.push_back (4);

        ListIterator<int> it1 (l1);
        it1.use ();  // needed because of used as smart pointer by the product iterator

        list<string> l2;
        l2.push_back ("abc");
        l2.push_back ("def");

        ListIterator<string> it2 (l2);
        it2.use (); // needed because of used as smart pointer by the product iterator

        size_t nbFound = 0;

        ProductIterator<int,string> it (&it1, &it2);

        for (it.first(); ! it.isDone(); it.next())
        {
            pair<int,string>& p = it.currentItem();

            if (nbFound==0)    {  CPPUNIT_ASSERT (p.first==1  &&  p.second=="abc");   }
            if (nbFound==1)    {  CPPUNIT_ASSERT (p.first==1  &&  p.second=="def");   }
            if (nbFound==2)    {  CPPUNIT_ASSERT (p.first==2  &&  p.second=="abc");   }
            if (nbFound==3)    {  CPPUNIT_ASSERT (p.first==2  &&  p.second=="def");   }
            if (nbFound==4)    {  CPPUNIT_ASSERT (p.first==4  &&  p.second=="abc");   }
            if (nbFound==5)    {  CPPUNIT_ASSERT (p.first==4  &&  p.second=="def");   }

            nbFound++;
        }

        CPPUNIT_ASSERT (nbFound == 6);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/

    /** */
    void testIteratorProduct2 ()
    {
        const char* sep = ",";

        TokenizerIterator it1 ("a,b,c,d", sep);
        TokenizerIterator it2 ("1,2,3",   sep);
        TokenizerIterator it3 ("X,Y",     sep);

        list <Iterator<char*>* > itList;
        itList.push_back (&it1);
        itList.push_back (&it2);
        itList.push_back (&it3);

        CartesianIterator <char*> p (itList);

        for (p.first(); ! p.isDone(); p.next())
        {
            list<char*>& current = p.currentItem();

            for (list<char*>::iterator itStr = current.begin(); itStr != current.end() ; itStr++)
            {
                printf ("%s ", *itStr);
            }
            printf ("\n");
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    class TestIterate
    {
    public:
        TestIterate (int p)  : sum(0), mult(p)  {}
        void foo (int n)  {  sum += mult*n;  }
        int getSum () { return sum; }
    private:
        int sum;
        int mult;
    };

    void testIteratorIterate ()
    {
        list<int> l1;
        l1.push_back (1);
        l1.push_back (2);
        l1.push_back (4);

        ListIterator<int> it (l1);

        TestIterate c1 (1);
        it.iterate (&c1, (Iterator<int>::Method) &TestIterate::foo);
        CPPUNIT_ASSERT (c1.getSum() == (1*1 + 1*2 + 1*4));

        TestIterate c2 (2);
        it.iterate (&c2, (Iterator<int>::Method) &TestIterate::foo);
        CPPUNIT_ASSERT (c2.getSum() == (2*1 + 2*2 + 2*4));

        TestIterate c3 (-5);
        it.iterate (&c3, (Iterator<int>::Method) &TestIterate::foo);
        CPPUNIT_ASSERT (c3.getSum() == (-5*1 + -5*2 + -5*4));
    }

    /********************************************************************************/
    /********************************************************************************/
    class EventInfoDummy : public EventInfo
    {
    public:
        EventInfoDummy (int n) : EventInfo(0), _n(n) {}
        int getValue() { return _n; }
    private:
        int _n;
    };

    /** */
    void update (EventInfo* evt, ISubject* subject)
    {
        EventInfoDummy* evt1 = dynamic_cast<EventInfoDummy*> (evt);
        if (evt1 != 0)
        {
            int n = evt1->getValue();
            CPPUNIT_ASSERT (n==1 || n==2 || n==3);
            return;
        }

        IterationStatusEvent* evt2 = dynamic_cast<IterationStatusEvent*> (evt);
        if (evt2 != 0)
        {
            //printf ("[%d] '%s'\n", evt2->getCurrentIndex(), evt2->getMessage());
        }

    }

    /** */
    void testObserver1 ()
    {
        static InterfaceId id1 = 0;

        /** We create some subject with some interface id. */
        Subject subject1 (id1);

        /** We attach some observer. */
        subject1.addObserver (this);

        /** We notify some events. */
        subject1.notify (new EventInfoDummy (1));
        subject1.notify (new EventInfoDummy (2));
        subject1.notify (new EventInfoDummy (3));

        /** We detach the observer. */
        subject1.removeObserver (this);

        /** We notify some event. */
        subject1.notify (new EventInfoDummy (4));
    }

    /** */
    void testIteratorObserver ()
    {
        /** We create some subject with some interface id. */
        Subject subject (ITERATION_NOTIF_ID);

        /** We attach some observer. */
        subject.addObserver (this);

        /** We notify some events. */
        list<int> l;
        for (int i=0; i<100; i++)  { l.push_back(i); }

        ListIterator<int> it (l);
        it.use ();  // needed because of used as smart pointer by the product iterator

        size_t nb = 0;

        subject.notify (new IterationStatusEvent (ITER_STARTING, nb, l.size(), "starting"));

        for (it.first(); ! it.isDone(); it.next(), nb++)
        {
            if (nb%10 == 0)
            {
                subject.notify (new IterationStatusEvent (ITER_ON_GOING, nb, l.size(), "iterating..."));
            }
        }
        subject.notify (new IterationStatusEvent (ITER_DONE, nb, l.size(), "finished"));
    }

    /** */
    void testTokenizer ()
    {
        size_t i=0;
        TokenizerIterator it ("1,3,4,5,6", ",");
        for (it.first(); ! it.isDone(); it.next(), i++)
        {
            char* current = it.currentItem();

            if (i==0)  {  CPPUNIT_ASSERT (strcmp(current,"1")==0);  }
            if (i==1)  {  CPPUNIT_ASSERT (strcmp(current,"3")==0);  }
            if (i==2)  {  CPPUNIT_ASSERT (strcmp(current,"4")==0);  }
            if (i==3)  {  CPPUNIT_ASSERT (strcmp(current,"5")==0);  }
            if (i==4)  {  CPPUNIT_ASSERT (strcmp(current,"6")==0);  }
        }
    }

    /** */
    void testWrapper ()
    {
        size_t i=0;

        WrapperIterator it1 ("EFLMDIFJDFMLSIHDFOAIEJFEOIFHMSLDIFJODUFHSLDKFUGMAEIF", 10);
        for (it1.first(); ! it1.isDone(); it1.next(), i++)
        {
            const char* current = it1.currentItem();

            if (i==0)  {  CPPUNIT_ASSERT (strcmp(current,"EFLMDIFJDF")==0);  }
            if (i==1)  {  CPPUNIT_ASSERT (strcmp(current,"MLSIHDFOAI")==0);  }
            if (i==2)  {  CPPUNIT_ASSERT (strcmp(current,"EJFEOIFHMS")==0);  }
            if (i==3)  {  CPPUNIT_ASSERT (strcmp(current,"LDIFJODUFH")==0);  }
            if (i==4)  {  CPPUNIT_ASSERT (strcmp(current,"SLDKFUGMAE")==0);  }
            if (i==5)  {  CPPUNIT_ASSERT (strcmp(current,"IF")==0);  }
        }

        WrapperIterator it2 ("", 10);
        CPPUNIT_ASSERT (it2.isDone() == true);

        WrapperIterator it3 ("a", 10);
        CPPUNIT_ASSERT (it3.isDone() == false);
    }

    /********************************************************************************/
    class MyCommandFactory : public ICommandFactory
    {
    public:

        MyCommandFactory (const string& id) : _id(id)  {}

        const string& getId() { return _id; }

        ICommand* createCommand (const std::string& name)
        {
            ICommand* result = 0;

            if (name.compare("cmd")==0)
            {
                result = new MyCommand (this);
            }
            return result;
        }
    private:

        string _id;

        class MyCommand : public ICommand
        {
        public:
            MyCommand (MyCommandFactory* ref) : _ref(ref) {}

            void execute ()  {  printf ("id='%s'\n", _ref->getId().c_str());  }

        private:
            MyCommandFactory* _ref;
        };
    };

    /** */
    void testCommandFactories ()
    {
        list<MyCommandFactory*> factories;

        MyCommandFactory* f1 = new MyCommandFactory ("foo");
        LOCAL (f1);
        factories.push_back (f1);

        MyCommandFactory* f2 = new MyCommandFactory ("bar");
        LOCAL (f2);
        factories.push_back (f2);

        list<ICommand*> commands = ICommandFactory::createCommandsList ((const list<ICommandFactory*>&)factories, "cmd");

        ParallelCommandDispatcher dispatcher;
        dispatcher.dispatchCommands (commands, 0);
    }

    /** */
    void testSystemCommand ()
    {
        IFile* file = DefaultFactory::file().newFile ("/tmp/plast_testSystemCommand", "w");

        list<ICommand*> commands;

        commands.push_back (new SystemCommand (file, "ls"));
        commands.push_back (new SystemCommand (file, "ls"));
        commands.push_back (new SystemCommand (file, "ls"));
        commands.push_back (new SystemCommand (file, "ls"));

        ParallelCommandDispatcher dispatcher;
        dispatcher.dispatchCommands (commands, 0);

        delete file;
    }

    /** */
    void testProperties ()
    {
        Properties props;
        props.add (0, "root", "");

        props.add (1, "level_1_1", "%ld", 1);
        props.add (1, "level_1_2", "%ld", 2);

        props.add (2, "level_2_1", "%ld", 3);
        props.add (3, "level_3_1", "%ld", 4);
        props.add (3, "level_3_2", "%ld", 5);

        props.add (2, "level_2_2", "%ld", 6);

        props.add (1, "level_1_3", "%ld", 7);

        XmlDumpPropertiesVisitor v ("/tmp/plast_testProperties_progress.xml");
        props.accept (&v);
    }

    /** */
    void testXmlReader ()
    {
        stringstream is (stringstream::in | stringstream::out);
        is << "<properties><progression><exec_percentage>100</exec_percentage><nb_alignments>4257</nb_alignments></progression>properties>";

        class XmlListener : public IObserver
        {
        public:
            void update (EventInfo* evt, ISubject* subject)
            {
                XmlTagOpenEvent* open = dynamic_cast<XmlTagOpenEvent*> (evt);
                if (open)
                {
                    cout << "open '" << open->_name << "'" << endl;
                    return;
                }

                XmlTagCloseEvent* close = dynamic_cast<XmlTagCloseEvent*> (evt);
                if (close)
                {
                    cout << "close '" << close->_name << "'" << endl;
                    return;
                }

                XmlTagTextEvent* text = dynamic_cast<XmlTagTextEvent*> (evt);
                if (text)
                {
                    cout << "text '" << text->_txt << "'" << endl;
                    return;
                }

                XmlTagAttributeEvent* attribute = dynamic_cast<XmlTagAttributeEvent*> (evt);
                if (attribute)
                {
                    cout << "attribute: name='" << attribute->_name << "'  value='" << attribute->_value << "'" << endl;
                    return;
                }
            }
        };

        /** We create a reader. */
        XmlReader reader (is);

        /** We create an observer. */
        IObserver* listener = new XmlListener();

        /** We attach the listener to the reader. */
        reader.addObserver (listener);

        /** We read the XML file. */
        reader.read();

        /** We detach the listener from the reader. */
        reader.addObserver (listener);
    }


    struct Rule
    {
        string _accessor;
        string _operator;
        string _type;
        string _value;

        friend ostream& operator<< (ostream& out, const Rule& rule)
        {
            out << "RULE: "
                 << "accessor='" << rule._accessor << "'  "
                 << "op='"       << rule._operator << "'  "
                 << "type='"     << rule._type     << "'  "
                 << "value='"    << rule._value    << "'";
            return out;
        }
    };

    class XmlFilterListener : public IObserver
    {
    public:

        /** */
        bool isExclusive () { return _exclusive; }

        /** */
        const list<Rule>& getRules ()  { return _rules; }

        /** */
        void update (EventInfo* evt, ISubject* subject)
        {
            XmlTagOpenEvent* open = dynamic_cast<XmlTagOpenEvent*> (evt);
            if (open)
            {
                return;
            }

            XmlTagCloseEvent* close = dynamic_cast<XmlTagCloseEvent*> (evt);
            if (close)
            {
                if (close->_name.compare ("BRule") == 0)
                {
                    /** We add the rule into the list. */
                    _rules.push_back (_currentRule);
                }
                else if (close->_name.compare ("accessor")  == 0)  {  _currentRule._accessor = _lastText;  }
                else if (close->_name.compare ("operator")  == 0)  {  _currentRule._operator = _lastText;  }
                else if (close->_name.compare ("value")     == 0)  {  _currentRule._value    = _lastText;  }
                else if (close->_name.compare ("exclusive") == 0)
                {
                    _exclusive = _lastText.compare ("true") == 0;
                }

                return;
            }

            XmlTagTextEvent* text = dynamic_cast<XmlTagTextEvent*> (evt);
            if (text)
            {
                _lastText = text->_txt;
                return;
            }

            XmlTagAttributeEvent* attribute = dynamic_cast<XmlTagAttributeEvent*> (evt);
            if (attribute)
            {
                if (attribute->_name.compare("class") == 0)
                {
                    _currentRule._type = attribute->_value;
                }
                return;
            }
        }

    private:

        bool _exclusive;
        Rule _currentRule;
        list<Rule> _rules;
        string _lastText;
        string _lastAttributeName;
        string _lastAttributeValue;
    };

    /** */
    void testXmlFilterReader ()
    {
        ifstream is(getPath("filter1.xml"), ios::binary);
        if ( ! is.good() ) {
            is.close ();
            throw std::logic_error( "Filter file not found" );
        }

        /** We create a reader. */
        XmlReader reader (is);

        /** We create an observer. */
        XmlFilterListener listener;

        /** We attach the listener to the reader. */
        reader.addObserver (&listener);

        /** We read the XML file. */
        reader.read();

        /** We detach the listener from the reader. */
        reader.addObserver (&listener);

        cout << "exclusive=" << listener.isExclusive() << endl;
        for (list<Rule>::const_iterator it = listener.getRules().begin(); it != listener.getRules().end(); it++)
        {
            cout << *it << endl;
        }

        /** We close the file. */
        is.close ();

        printf ("FINISHED\n");
    }

    /** */
    void testFileLineIterator1_aux (u_int64_t offset0, u_int64_t offset1)
    {
        string dir      = "/local/users/edrezen/databases/";

        size_t nb1 = 0;
        string filename1 = dir + "panda.fa";

        size_t nb2 = 0;
        stringstream ss;
        ss << dir << "panda_part1.fa" << ","
           << dir << "panda_part2.fa";
        string filename2 = ss.str();

        FileLineIterator it1 (filename1.c_str(), 256, offset0, offset1);
        for (it1.first(); !it1.isDone(); it1.next())    {  nb1 ++; }

        FileLineIterator it2 (filename2.c_str(), 256, offset0, offset1);
        for (it2.first(); !it2.isDone(); it2.next())    {  nb2 ++; }

        size_t nbCommon   = 0;
        size_t nbDistinct = 0;
        for (it1.first(), it2.first(); !it1.isDone() || !it2.isDone(); it1.next(), it2.next())
        {
            if (strcmp (it1.currentItem(), it2.currentItem()) != 0)
            {
                nbDistinct ++;
            }
            else
            {
                nbCommon ++;
            }
        }

        // printf ("nb1=%ld  nb2=%ld  nbCommon=%ld  nbDistinct=%ld\n", nb1, nb2, nbCommon, nbDistinct);

        CPPUNIT_ASSERT (nb1 == nb2);
        CPPUNIT_ASSERT (nbCommon   == nb1);
        CPPUNIT_ASSERT (nbDistinct == 0);
    }

    /** */
    void testFileLineIterator1 ()
    {
#if 0
        //tests files (panda*.fa) have been lost...
        testFileLineIterator1_aux (0,0);
        testFileLineIterator1_aux (1000,5000);
        testFileLineIterator1_aux (1000,10000);
        testFileLineIterator1_aux (2000,100000);
        testFileLineIterator1_aux (200000,500000);
        testFileLineIterator1_aux (1000000,2000000);
        testFileLineIterator1_aux (0,2000000);
        testFileLineIterator1_aux (0,        20000363);
        testFileLineIterator1_aux (20000364, 25499088);
        testFileLineIterator1_aux (20000000, 25499088);
#endif
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFileLineIterator2 ()
    {
        const char* filename = "uniprot_sprot.fa";
        const char* tag      = "line";

        u_int64_t nbLines   = 0;
        u_int64_t dataSize = 0;

        TimeInfo info;
        info.addEntry (tag);

        FileLineIterator it (getPath (filename));

        /** We loop over the sequences. */
        for (it.first(); ! it.isDone(); it.next ())
        {
            // const char* line = it.currentItem();

            dataSize += it.getLineSize ();

            nbLines++;
        }

        info.stopEntry (tag);

        /** Shortcut. */
        u_int64_t execTime = info.getEntryByKey(tag);

        /** Note we add totalSize+nbLines because the FileLineIterator::getLineSize doesn't count the ending \n character. */

        cout << "File '" << getPath (filename) << "' iterated in " << execTime << " msec => found "
        	 << nbLines << " lines,  data size " << dataSize << " ,  total size " << (nbLines + dataSize)
        	 <<  "=> rate " << (float) (125  * (dataSize+nbLines)) / (float) (execTime*128*1024)  << " MBytes/sec" << endl;
    }

    /** */
    void testIteratorGet1 ()
    {
        list<const char*> l1;
        l1.push_back ("testIteratorGet1_a");
        l1.push_back ("testIteratorGet1_b");
        l1.push_back ("testIteratorGet1_c");

        const char* current = 0;
        size_t nbGot;
        for (IteratorGet<const char*> it (new ListIterator<const char*> (l1));  it.get (current, nbGot); )
        {
            printf ("-> %s\n", current);
        }
    }

    class MyCmd : public IteratorCommand<string>
    {
    public:

        MyCmd (IteratorGet<string>* it) : IteratorCommand<string> (it)  {}

        void execute (string& current, size_t& nbGot)
        {
            printf ("=> %s\n", current.c_str());
            sleep (1);
        }
    };

    /** */
    void testIteratorGet2 ()
    {
        /** We build some list containing items. */
        list<string> l1;
        for (size_t i=1; i<=40; i++)  {  char buf[32];  snprintf (buf, sizeof(buf), "str%ld", i);  l1.push_back (buf);  }

        /** We build an iterator on that list. */
        IteratorGet<string>* it = new IteratorGet<string> (new ListIterator<string> (l1));

        /** We build a list of commands that will iterate our list, through the created iterator. */
        list<ICommand*> commands;
        for (size_t i=1; i<= DefaultFactory::thread().getNbCores(); i++)  {  commands.push_back (new MyCmd (it)); }

        /** We dispatch the commands to a dispatcher. */
        ParallelCommandDispatcher().dispatchCommands (commands, 0);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFileLineIterator_aux (std::vector<string>& files)
    {
        Info info1, info2;

        /** We iterate each file and get some information. */
        for (size_t i=0; i<files.size(); i++)
        {
            FileLineIterator it (getPath (files[i].c_str()));

            for (it.first(); ! it.isDone(); it.next ())
            {
                char* line = it.currentItem();

                info1.nbLines ++;

                size_t lenComment = strlen(line);
                for (size_t k=0; k<lenComment; k++)  {  info1.checksum += line[k];  }
            }

            info1.cumulatedLength += it.tell();
        }

        /** Now we iterated the concatenation file. */
        string concatFile;
        for (size_t i=0; i<files.size(); i++)
        {
            if (i>0) { concatFile += ','; }
            concatFile += getPath (files[i].c_str());
        }

        FileLineIterator it (concatFile.c_str());

        for (it.first(); ! it.isDone(); it.next ())
        {
            char* line = it.currentItem();

            info2.nbLines ++;

            size_t lenComment = strlen(line);
            for (size_t k=0; k<lenComment; k++)  {  info2.checksum += line[k];  }
        }
        info2.cumulatedLength = it.tell();

        printf ("CONCAT %s\n", concatFile.c_str());
        cout << "info1 " << info1 << endl;
        cout << "info2 " << info2 << endl;

        CPPUNIT_ASSERT (info1 == info2);
    }

    /********************************************************************************/
    void testFileLineIterator3 ()
    {
        vector<string> files;

        files.push_back ("tursiops.fa");            testFileLineIterator_aux (files);
        files.push_back ("query.fa");               testFileLineIterator_aux (files);
        files.push_back ("panda.fa");               testFileLineIterator_aux (files);
        files.push_back ("sapiens_1Mo.fa");         testFileLineIterator_aux (files);
        files.push_back ("yeast.fa");               testFileLineIterator_aux (files);
        files.push_back ("uniprot_sprot.fa");       testFileLineIterator_aux (files);
        files.push_back ("chr10.fa");               testFileLineIterator_aux (files);
        files.push_back ("CionaIntestinalis.fa");   testFileLineIterator_aux (files);
        files.push_back ("panda.fa");               testFileLineIterator_aux (files);
    }

    /********************************************************************************/
    void testDirectoryIterator ()
    {
        DirectoryIterator it ("/usr/lib", "lib");

        size_t nbFiles = 0;

        for (it.first(); !it.isDone(); it.next(), nbFiles++)
        {
            printf ("current = '%s'\n", it.currentItem());
        }

        printf ("FOUND %ld files...\n", nbFiles);
    }

    /********************************************************************************/
    void testDirectoryIteratorRecursive ()
    {
        DirectoryIterator it (getPath(".."), "", false, true);
        //DirectoryIterator it ("/media/USB_Disk/users/edrezen/irisa/symbiose/gat/results/other/blast", "gat");

        size_t nbFiles = 0;

        for (it.first(); !it.isDone(); it.next(), nbFiles++)
        {
            printf ("current = '%s'\n", it.currentItem());
        }

        printf ("FOUND %ld files...\n", nbFiles);
    }
};

/********************************************************************************/

extern "C" Test* TestDesignPattern_suite()  { return TestDesignPattern::suite (); }

