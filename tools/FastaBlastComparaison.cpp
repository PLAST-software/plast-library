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

/********************************************************************************/

#include <misc/api/PlastStrings.hpp>
#include <misc/impl/OptionsParser.hpp>

#include <designpattern/impl/Property.hpp>

#include <database/api/IDatabaseQuickReader.hpp>
#include <database/impl/DatabaseUtility.hpp>
#include <database/impl/BlastdbDatabaseQuickReader.hpp>
#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <database/impl/BlastdbSequenceIterator.hpp>
#include <database/impl/FastaSequenceIterator.hpp>

#include <designpattern/impl/TokenizerIterator.hpp>

#include <os/impl/TimeTools.hpp>
#include <os/api/IMemoryFile.hpp>

#define DEBUG(a)  printf a
#define INFO(a)   printf a

/********************************************************************************/

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace misc;
using namespace misc::impl;
using namespace database;
using namespace database::impl;
using namespace os;
using namespace os::impl;

#define STR_OPTION_CACHE_REMOVE             "-c"
#define STR_OPTION_FILE_PERFORMANCE_TEST    "-test"

#define BLASTDB_COMMENT_DETECTION "#&COMMENT;!"

list<os::IMemoryFile*> headerFileList;

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
char *getComment(const char* commentSrc, u_int64_t *sizeDest)
{
	char *commentDest;
	u_int32_t startOffset = 0;
	u_int32_t maxCommentSize = 0;
	unsigned char byteLengthDef = 0;
	unsigned char byteLengthDefIfupper128 = 0;
	u_int32_t size = 0;
	u_int32_t indexRead = 0;
	os::IMemoryFile* currentHeaderFile;
	list<os::IMemoryFile*>::iterator fileIt;
    TimeInfo t;

    char fileNamePath[1024] = "";
    char offset[20]= "";;
    char maxOffset[20]= "";;
    char *maskDetection;
    char *filename;
    char *startOff;

	/*** read the filename ***/
    maskDetection=strchr((char*)commentSrc,',');
	if (strncmp(commentSrc,BLASTDB_COMMENT_DETECTION,(maskDetection-commentSrc))==0)
	{
		filename=strchr((maskDetection+1),',');
		strncpy(fileNamePath,maskDetection+1,(filename - maskDetection-1));
		//fileNamePath[(filename - maskDetection-1)]="\0";
		/*** read the filename ***/
		for (fileIt= headerFileList.begin(); fileIt != headerFileList.end(); fileIt++)
		{
			if (strcmp(fileNamePath,(*fileIt)->getPath().c_str()) == 0)
			{
				currentHeaderFile = *fileIt;
				break;
			}
		}
		if (fileIt==headerFileList.end())
		{
			currentHeaderFile = DefaultFactory::fileMem().newFile (fileNamePath);
			headerFileList.push_back(currentHeaderFile);
		}

		//currentHeaderFile = DefaultFactory::fileMem().newFile (comment[1].c_str());
		/*** startOffset ***/
		startOff=strchr((filename+1),',');
		strncpy(offset,filename+1,(startOff - filename-1));
		startOffset = misc::atoi(offset);
		/*** Max comment size ***/
		strcpy(maxOffset,startOff+1);
		maxCommentSize = misc::atoi(maxOffset);

		/** set the pointer at the begin of the header + 7 in order to remove
		 * - 2 bytes for the beginning of the sequenceOf which starts with a 0x3080
		 * - 2 bytes for the beginning of the sequence which starts with a 0x3080
		 * - 2 bytes for the beginning of the item which starts with a 0xA080 (next item A1, ....)
		 * - 1 Byte  which is 0x1A for the beginning of the title
		 */
		indexRead = startOffset+7;

		/** read the first byte to know the string length
		 * if the first bit is on, then the next seven bits will encode the string length on 128 bytes
		 * if the first bit is off, then the next seven bits will encode the interger which indicate the string length
		 */
		byteLengthDef = (unsigned char)currentHeaderFile->getData()[indexRead];
		indexRead++;
		if (byteLengthDef&0x80)
		{
			byteLengthDefIfupper128=byteLengthDef&0x7F;
			/** Read the header length*/
			//sequenceHdr->read(&byteLengthDefUpper128[0], sizeof(unsigned char), byteLengthDefIfupper128);
			for (int32_t i = 0; i < (int32_t)byteLengthDefIfupper128; i++)
			{
				u_int32_t value= (unsigned char)currentHeaderFile->getData()[indexRead]&0x000000FF;
				size = (size << 8) + value;
				indexRead++;
			}
		}
		else
		{
			size=byteLengthDef&0x7F;
		}

		if (size>maxCommentSize)
			size=maxCommentSize;
		commentDest = (char*)&currentHeaderFile->getData()[indexRead];
		*sizeDest = size;
		/*commentDest.append(&currentHeaderFile->getData()[indexRead], size);
		commentDest[size]='\0';*/
		//delete currentHeaderFile;
	}
	else
	{
		commentDest = (char*)commentSrc;
		*sizeDest = strlen(commentSrc);
	}
	return commentDest;
}

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
    TimeInfo t;

	u_int64_t datasizeBlast    = 0;
    u_int32_t nbsequenceBlast = 0;
    size_t checksumBlast = 0;
    size_t checksumComBlast = 0;

	u_int64_t datasizeFasta    = 0;
	u_int32_t nbsequenceFasta = 0;
	size_t checksumFasta = 0;
	size_t checksumComFasta = 0;

    /** We need a command line options parser. */
    OptionsParser parser;
    parser.add (new OptionOneParam (STR_OPTION_SUBJECT_URI,         	"name of the fasta bank (.fa)", false));
    parser.add (new OptionOneParam (STR_OPTION_QUERY_URI,         		"name of the blast bank (.pin or nin) or (.pal and .nal)", false));
    parser.add (new OptionNoParam  (STR_OPTION_FILE_PERFORMANCE_TEST,   "File performance test",  false));
    parser.add (new OptionOneParam (STR_OPTION_MAX_DATABASE_SIZE,   	"block size (default 0)", false));
    parser.add (new OptionNoParam  (STR_OPTION_INFO_VERBOSE,        	"verbose",  false));
    parser.add (new OptionNoParam  (STR_OPTION_CACHE_REMOVE,        	"Remove the cache",  false));

    try {
        /** We parse the provided options. */
        parser.parse (argc, argv);

        /** We create a Properties instance for collecting user command line options. */
        Properties props;
        props.add (0, parser.getProperties());

        /** We may want to display some help. */
        if (parser.saw(STR_OPTION_HELP))   {   parser.displayHelp   (stdout);  }

        /** We retrieve the URI of the bank. */
        string uriFasta = props.getProperty (STR_OPTION_SUBJECT_URI) ? props.getProperty (STR_OPTION_SUBJECT_URI)->getValue() : "";

        /** We retrieve the URI of the bank. */
        string uriBlastdb = props.getProperty (STR_OPTION_QUERY_URI) ? props.getProperty (STR_OPTION_QUERY_URI)->getValue() : "";

        /** We retrieve the max database size. */
        u_int64_t maxdatabasesize = props.getProperty (STR_OPTION_MAX_DATABASE_SIZE) ?
            atoll (props.getProperty (STR_OPTION_MAX_DATABASE_SIZE)->getString())
            : 0;

        if (props.getProperty (STR_OPTION_CACHE_REMOVE))
        {
        	u_int64_t	sizeCache = 1024*1024;
        	u_int64_t	totalSize = 0;
        	u_int64_t	totalMem = DefaultFactory::memory().getMemUsage();
    		char *buffer = NULL;

    		printf("--- Start clear cache Usage : %ld---\n", totalMem);

        	for (u_int32_t ni=0;ni<(64*1024);ni++)
        	{
        		buffer = (char*)calloc(sizeCache,sizeof(char));
        		buffer[0]=1;
        		buffer[(sizeCache-1)]=2;
        		totalSize+=sizeCache;
        	}
        	totalMem = DefaultFactory::memory().getMemUsage();
    		printf("--- Stop clear cache total : %ld Usage : %ld ---\n",totalSize, totalMem);
        }

        if (props.getProperty (STR_OPTION_FILE_PERFORMANCE_TEST))
        {
        	os::IFile* testFile;
        	os::IMemoryFile* testMemFileSeq;
        	os::IMemoryFile* testMemFileHdr;
        	os::IMemoryFile* testMemFileIn;
        	u_int64_t	fileSize = 0;
        	u_int64_t	size = 0;
        	u_int64_t	sizeRead = 1024*2;
			char *buffer = (char*)DefaultFactory::memory().malloc(sizeRead);
		    size_t checksumBlastSeq = 0;
		    size_t checksumBlastHdr = 0;
		    size_t checksumBlastIn = 0;


			checksumFasta = 0;
			printf("--- FASTA file ---\n");
        	t.addEntry ("FileSizeFastA");
        	TokenizerIterator tokenizer (uriFasta.c_str(), ",");
	        for (tokenizer.first (); !tokenizer.isDone(); tokenizer.next())
	        {
	        	testFile = DefaultFactory::file().newFile (tokenizer.currentItem(),"rb");
	        	size += testFile->getSize();
	        	testFile->seeko (0, SEEK_SET);
	        	do
	        	{
	        		fileSize +=	testFile->gets(buffer, sizeRead);
		        	for (u_int32_t ni=0;ni<sizeRead;ni++)
		        	{
		        		checksumFasta = (checksumFasta + buffer[ni]) % (1<<16);
		        	}

	        	}while(!testFile->isEOF());
	        }
	        t.stopEntry("FileSizeFastA");
			printf("   FastA read (fread) time *** total=%ld total2 =%ld time=%d\n", size, fileSize, t.getEntryByKey("FileSizeFastA"));

	        size = 0;
        	printf("--- BLAST file ---\n");
        	t.addEntry ("FileSizeBlast");
        	string filenameWithExtSeq;
        	string filenameWithExtHdr;
        	string filenameWithExtIn;
        	string filenameWithoutExt;
        	u_int32_t foundPoint;

        	fileSize = 0;
        	size=0;
        	TokenizerIterator tokenizer2 (uriBlastdb.c_str(), ",");
	        for (tokenizer2.first (); !tokenizer2.isDone(); tokenizer2.next())
	        {
	        	foundPoint = std::string(tokenizer2.currentItem()).find_last_of(".");
	        	filenameWithoutExt = std::string(tokenizer2.currentItem()).substr(0,foundPoint);
	        	DatabaseLookupType::QuickReaderType_e databaseType;
	            databaseType = DatabaseLookupType::quickReaderType(tokenizer2.currentItem());
	        	if (databaseType == DatabaseLookupType::ENUM_BLAST_PIN)
	        	{
	        		filenameWithExtSeq = filenameWithoutExt + ".psq";
	        		filenameWithExtHdr = filenameWithoutExt + ".phr";
	        		filenameWithExtIn = filenameWithoutExt + ".pin";
	        	}
	        	else
	        	{
	        		filenameWithExtSeq = filenameWithoutExt + ".nsq";
	        		filenameWithExtHdr = filenameWithoutExt + ".nhr";
	        		filenameWithExtIn = filenameWithoutExt + ".nin";
	        	}
	        	testMemFileSeq = DefaultFactory::fileMem().newFile (filenameWithExtSeq.c_str());
	        	testMemFileHdr = DefaultFactory::fileMem().newFile (filenameWithExtHdr.c_str());
	        	testMemFileIn = DefaultFactory::fileMem().newFile (filenameWithExtIn.c_str());
	        	size += testMemFileSeq->getSize();
	        	size += testMemFileHdr->getSize();
	        	size += testMemFileIn->getSize();
	        	for (u_int32_t ni=0;ni<testMemFileSeq->getSize();ni++)
	        	{
	        		checksumBlastSeq = (checksumBlastSeq + testMemFileSeq->getData()[ni]) % (1<<16);
	        		fileSize ++;
	        	}
	        	for (u_int32_t ni=0;ni<testMemFileHdr->getSize();ni++)
	        	{
	        		checksumBlastHdr = (checksumBlastHdr + testMemFileHdr->getData()[ni]) % (1<<16);
	        	}
	        	for (u_int32_t ni=0;ni<testMemFileIn->getSize();ni++)
	        	{
	        		checksumBlastIn = (checksumBlastIn + testMemFileIn->getData()[ni]) % (1<<16);
	        	}
	        }
	        t.stopEntry("FileSizeBlast");
			printf("   Blast size read time *** total=%ld total2 =%ld time=%d\n", size, fileSize, t.getEntryByKey("FileSizeBlast"));
			DefaultFactory::memory().free(buffer);
        }
        else
        {

			if(uriFasta!="")
			{
				printf("--- FASTA file '%s' ---\n", uriFasta.c_str());

				/** Read the Fasta File
				 * */
				if (maxdatabasesize!=0)
		        {
		        	string filenameExt;
		        	u_int32_t foundPoint;

		        	t.addEntry ("FastaQuick");
		        	foundPoint = std::string(uriFasta).find_last_of(".");
		        	filenameExt = std::string(uriFasta).substr(foundPoint,uriFasta.size());

					FastaDatabaseQuickReader readerFasta (uriFasta, false);
					if (filenameExt==".info")
						readerFasta.load(uriFasta);
					else
						readerFasta.read (maxdatabasesize);
					t.stopEntry("FastaQuick");

					if (readerFasta.getKind()==IDatabaseQuickReader::ENUM_AMINO_ACID)
						EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
					else
						EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);

					printf("   FASTA QuickReader data *** MaxSize=%ld nbOffset=%ld dataSize=%ld nbSequences=%d\n",
							readerFasta.getMaxBlockSize(),readerFasta.getOffsets().size(),readerFasta.getDataSize(),readerFasta.getNbSequences());

					std::vector<u_int64_t>& offsetsFasta = readerFasta.getOffsets ();
					u_int64_t totalFastaSize = 0;

					t.addEntry ("Fasta");
					for (size_t i=0; i<offsetsFasta.size()-1; i++)
					{
						u_int64_t a = offsetsFasta[i];
						u_int64_t b = offsetsFasta[i+1] - 1;
						TimeInfo tInter;

						tInter.addEntry ("FastaCreate");
						ISequenceIterator* it = new FastaSequenceIterator (readerFasta.getUri().c_str(), 2*1024, a, b);
						//ISequenceIterator* it = new FastaSequenceIterator (uriFasta.c_str(), 1024, a, b);
						tInter.stopEntry("FastaCreate");
						LOCAL (it);
						tInter.addEntry ("FastaIter");
						for (it->first(); ! it->isDone(); it->next())
						{
							const ISequence* seq = it->currentItem();

							datasizeFasta += seq->data.letters.size;
							totalFastaSize += seq->data.letters.size + strlen(seq->comment);
							nbsequenceFasta ++;

							size_t n = seq->data.letters.size;
							for (size_t i=0; i<n; i++)   {  checksumFasta = (checksumFasta + seq->data.letters.data[i]) % (1<<16);  }
							for (size_t i=0; i<strlen(seq->comment); i++)   {  checksumComFasta = (checksumComFasta + seq->comment[i]) % (1<<16); }
						}
						tInter.stopEntry("FastaIter");
						/*printf("   FASTA inter time *** nbSeq=%d DataSize=%ld TotalSize=%ld Read%ld=%d Create=%d\n",
								nbsequenceFasta,datasizeFasta, totalFastaSize, i,
								tInter.getEntryByKey("FastaIter"),tInter.getEntryByKey("FastaCreate"));*/

					}
		        }
		        else
		        {
					t.addEntry ("Fasta");
					ISequenceIterator* it = new FastaSequenceIterator (uriFasta.c_str());
					LOCAL (it);
					for (it->first(); ! it->isDone(); it->next())
					{
						const ISequence* seq = it->currentItem();

						datasizeFasta += seq->data.letters.size;
						nbsequenceFasta ++;

						size_t n = seq->data.letters.size;
						for (size_t i=0; i<n; i++)   {  checksumFasta = (checksumFasta + seq->data.letters.data[i]) % (1<<16);  }
						for (size_t i=0; i<strlen(seq->comment); i++)   {  checksumComFasta = (checksumComFasta + seq->comment[i]) % (1<<16); }
					}
		        }
				t.stopEntry("Fasta");
				printf("   FASTA data *** dataSize=%ld nbSequences=%d checksum=%ld checksumComment=%ld\n",
						datasizeFasta,nbsequenceFasta,checksumFasta,checksumComFasta);

				printf("   FASTA time *** QuickRead=%d IterRead=%d\n", t.getEntryByKey("FastaQuick"), t.getEntryByKey("Fasta"));
			}


			if(uriBlastdb!="")
			{
			    /** List of index files to be read. */
			    std::list<string> commentList;
				printf("--- BLAST file '%s' ---\n", uriBlastdb.c_str());
				/** Read the Blast File
				 * */
	/*	        AliasDatabaseQuickReader<BlastdbDatabaseQuickReader> readerTemp(uriBlastdb);
				readerTemp.read(0);*/
				//uriBlastdb = uriBlastFile;

				if (maxdatabasesize!=0)
		        {
					t.addEntry ("BlastQuick");
					BlastdbDatabaseQuickReader readerBlast (uriBlastdb, false);
					readerBlast.read (maxdatabasesize);
					t.stopEntry("BlastQuick");

					if (readerBlast.getKind()==IDatabaseQuickReader::ENUM_AMINO_ACID)
						EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
					else
						EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);

					printf("   BLAST QuickReader data *** MaxSize=%ld nbOffset=%ld dataSize=%ld nbSequences=%d time=%d\n",
							readerBlast.getMaxBlockSize(),readerBlast.getOffsets().size(),readerBlast.getDataSize(),
							readerBlast.getNbSequences(), t.getEntryByKey("BlastQuick"));

					std::vector<u_int64_t>& offsetsBlast  = readerBlast.getOffsets ();

					headerFileList.clear();
					commentList.clear();
					t.addEntry ("Blast");
					for (size_t i=0; i<offsetsBlast.size()-1; i++)
					{
						u_int64_t a = offsetsBlast[i];
						u_int64_t b = offsetsBlast[i+1] - 1;
					    TimeInfo tInter;


					    tInter.addEntry ("BlastCreate");
					    BlastdbSequenceIterator* it = new BlastdbSequenceIterator (readerBlast.getUri().c_str(), 2*1024, a, b);
						tInter.stopEntry("BlastCreate");
						LOCAL (it);
						tInter.addEntry ("BlastIter");
						for (it->first(); ! it->isDone(); it->next())
						{
							const ISequence* seq = it->currentItem();

							datasizeBlast += seq->data.letters.size;
							nbsequenceBlast ++;

							size_t n = seq->data.letters.size;
							for (size_t i=0; i<n; i++)   {  checksumBlast = (checksumBlast + seq->data.letters.data[i]) % (1<<16);  }
							commentList.push_back(seq->comment);
							//for (size_t i=0; i<strlen(seq->comment); i++)   {  checksumComBlast = (checksumComBlast + seq->comment[i]) % (1<<16); }
						}
						tInter.stopEntry("BlastIter");

						printf("   BLAST inter time *** nbSeq=%d DataSize=%ld Read%ld=%d Create=%d\n",nbsequenceBlast,datasizeBlast,i,tInter.getEntryByKey("BlastIter"),tInter.getEntryByKey("BlastCreate"));
					}
					t.stopEntry("Blast");
					/** We delete each IFile instance of the list of files. */
					t.addEntry ("BlastComment");
					char *comment;
					u_int64_t size;
					u_int32_t nbComment=0;
					for (list<string>::iterator com= commentList.begin(); com != commentList.end(); com++)
					{
						comment = getComment((*com).c_str(),&size);
						for (size_t i=0; i<size; i++)   {  checksumComBlast = (checksumComBlast + comment[i]) % (1<<16); }
						nbComment++;
					}
					t.stopEntry("BlastComment");
					for (list<os::IMemoryFile*>::iterator file= headerFileList.begin(); file != headerFileList.end(); file++)
					{
						delete (*file);
					}
				}
				else
				{
					t.addEntry ("Blast");
					BlastdbDatabaseQuickReader readerBlast (uriBlastdb, false);
					readerBlast.read (maxdatabasesize);

					BlastdbSequenceIterator* it = new BlastdbSequenceIterator (readerBlast.getUri().c_str());
					LOCAL (it);
					for (it->first(); ! it->isDone(); it->next())
					{
						const ISequence* seq = it->currentItem();

						datasizeBlast += seq->data.letters.size;
						nbsequenceBlast ++;

						size_t n = seq->data.letters.size;
						for (size_t i=0; i<n; i++)   {  checksumBlast = (checksumBlast + seq->data.letters.data[i]) % (1<<16);  }
						commentList.push_back(seq->comment);
						//for (size_t i=0; i<strlen(seq->comment); i++)   {  checksumComBlast = (checksumComBlast + seq->comment[i]) % (1<<16); }
					}
					t.stopEntry("Blast");
					t.addEntry ("BlastComment");
					char *comment;
					u_int64_t size;
					for (list<string>::iterator com= commentList.begin(); com != commentList.end(); com++)
					{
						comment = getComment((*com).c_str(),&size);
						for (size_t i=0; i<size; i++)   {  checksumComBlast = (checksumComBlast + comment[i]) % (1<<16); }
					}
					t.stopEntry("BlastComment");
					for (list<os::IMemoryFile*>::iterator file= headerFileList.begin(); file != headerFileList.end(); file++)
					{
						delete (*file);
					}
				}



				printf("   BLAST data *** dataSize=%ld nbSequences=%d checksum=%ld checksumComment=%ld\n",
						datasizeBlast,nbsequenceBlast,checksumBlast,checksumComBlast);

				printf("   BLAST time *** QuickRead=%d IterRead=%d Comment=%d TOTAL=%d\n", t.getEntryByKey("BlastQuick"), t.getEntryByKey("Blast"), t.getEntryByKey("BlastComment"),
						t.getEntryByKey("BlastQuick")+t.getEntryByKey("Blast")+t.getEntryByKey("BlastComment"));
			}

			printf("- DIFF : time:%d data:%ld sequence:%d checksum:%ld checksumCom:%ld\n",
					(t.getEntryByKey("BlastQuick") + t.getEntryByKey("Blast") - t.getEntryByKey("FastaQuick") - t.getEntryByKey("Fasta")),
					(datasizeBlast - datasizeFasta),
					(nbsequenceBlast - nbsequenceFasta),
					(checksumBlast - checksumFasta),
					(checksumComBlast - checksumComFasta));
        }

        Properties info;

        info.add (0, "input");
        info.add (1, "filename FastA",  uriFasta);
        info.add (1, "filename Blast",  uriBlastdb);
//        info.add (1, "output",     output);
        info.add (1, "maxdbsize",  "%ld", maxdatabasesize);

        //info.add (0, reader.getProperties());

        if (props.getProperty (STR_OPTION_INFO_VERBOSE))
        {
            RawDumpPropertiesVisitor v;
            info.accept (&v);
        }
    }
    catch (OptionFailure& e)
    {
        parser.displayErrors (stdout);
        parser.displayHelp   (stdout);
    }
    catch (...)
    {
        fprintf (stderr, MSG_MAIN_MSG4);
    }

    return 0;
}


