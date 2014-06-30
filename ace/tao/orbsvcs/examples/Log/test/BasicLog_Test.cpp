// BasicLog_Test.cpp,v 1.10 2001/06/25 19:54:32 oci Exp

#include "BasicLog_Test.h"

#include "orbsvcs/Time_Utilities.h"

#include "tao/debug.h"

#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/SString.h"

#include <assert.h>

ACE_RCSID(Logger, BasicLog_Test, "BasicLog_Test.cpp,v 1.10 2001/06/25 19:54:32 oci Exp")

const char* BasicLog_Test::basic_log_factory_name_="BasicLogFactory";
const char* BasicLog_Test::naming_sevice_name_="NameService";

// Constructor
BasicLog_Test::BasicLog_Test(void)
: logServiceIor_(NULL)
{
// do nothing
}

BasicLog_Test::~BasicLog_Test()
{
  this->destroy_log();
}

int
BasicLog_Test::init (int argc, char *argv[])
{
  this->argc_ = argc;
  this->argv_ = argv;

  ACE_DECLARE_NEW_CORBA_ENV;
  ACE_TRY
    {
      // Initialize the ORB
      orb_ = CORBA::ORB_init (argc,
                              argv,
                              "internet",
                              ACE_TRY_ENV);
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    "\nTrying to initialize orb\n"));

      ACE_TRY_CHECK;
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    "\nOrb initialized successfully\n"));

      // Parse command line and verify parameters.
      if (this->parse_args () == -1)
        return -1;

      // Initialize the factory
      int init_result = this->init_factory (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (init_result != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           " (%P|%t) Unable to initialize naming"
                           "services.\n"),
                          -1);
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "init");
      return -1;
    }
  ACE_ENDTRY;

  ACE_RETURN(0);

}

int
BasicLog_Test::init_factory (CORBA::Environment &ACE_TRY_ENV)
{

  // Assumpting INS for finding LogServie

  //CORBA::Object_var logging_obj = orb_->resolve_initial_references ("BasicLogFactory",
  //                                                         ACE_TRY_ENV);

  this->resolve_naming_service(ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  this->resolve_basic_factory(ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  /*
  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "\nLoggingService resolved\n"));

  if (CORBA::is_nil (logging_obj.in ()))
    ACE_ERROR_RETURN ((LM_ERROR,
                       "LogService resolved to nil object\n"),
                      -1);
  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "\nLogService resolved\n"));

  // Narrow the factory and check the success
  factory_ =
    DsLogAdmin::BasicLogFactory::_narrow (logging_obj.in (),
                             ACE_TRY_ENV);

  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "\nFactory narrowed\n"));
  if (CORBA::is_nil (factory_.in ()))
    ACE_ERROR_RETURN ((LM_ERROR,
                       "narrow returned nil"n"),
                      -1);
  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "\nLogger_Factory narrowed\n"));
  */
  // If debugging, get the factory's IOR
  CORBA::String_var str =
    orb_->object_to_string (basicLog_.in(),
                            ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "The logger IOR is <%s>\n",
                str.in ()));
  ACE_RETURN(0);
}

int
BasicLog_Test::test_CreateLog(CORBA::ULongLong maxSize)
{

  ACE_TRY_NEW_ENV
    {
      DsLogAdmin::LogId id;
      basicLog_ = factory_->create(DsLogAdmin::wrap, maxSize, id);
      // @@ Krish, never use != 0 to compare a NIL object reference!!!
      assert(!CORBA::is_nil (basicLog_.in ()));
      ACE_TRY_CHECK;
      ACE_DEBUG ((LM_DEBUG,
                  "The logger id is %d\n",
                  id));
    }
  ACE_CATCH(DsLogAdmin::NoResources, resourceX)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "no resources creating basic log.\n"),
                        -1);
    }
  ACE_CATCH(DsLogAdmin::InvalidThreshold, threshold)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "invalid threshold creating basic log.\n"),
                        -1);
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION(ACE_ANY_EXCEPTION,"test_CreateLog");
      ACE_ERROR_RETURN ((LM_ERROR,
                         "Caught exception creating log.\n"),
                        -1);
    }
  ACE_ENDTRY;

  ACE_RETURN(0);
}

int
BasicLog_Test::test_LogAction ()
{
  ACE_TRY_NEW_ENV {

    DsLogAdmin::LogId id = basicLog_->id();

    ACE_DEBUG((LM_DEBUG,"The log's id is %d\n", id));

    basicLog_->set_log_full_action(DsLogAdmin::halt);

    DsLogAdmin::LogFullAction logFullAction = basicLog_->get_log_full_action();
    if (logFullAction != DsLogAdmin::halt){
      ACE_ERROR_RETURN((LM_ERROR,"Setting log full action to halt failed\n"),-1);
    }
    ACE_DEBUG((LM_DEBUG,"The log's full action successfully set to halt\n"));

#ifndef ACE_LACKS_LONGLONG_T
    ACE_DEBUG((LM_DEBUG,"The current size %o , max size %o",
               basicLog_->get_current_size(),basicLog_->get_max_size()));
#else
    ACE_DEBUG((LM_DEBUG,"The current size %u , max size %u",
               basicLog_->get_current_size().lo(),
               basicLog_->get_max_size().lo()));
#endif

    this->write_records(0, ACE_TRY_ENV);
    //make sure that it is full and when writing
    ACE_TRY_CHECK;
    ACE_DEBUG((LM_DEBUG,"Wrote records instead should have thrown exception"));

    basicLog_->set_log_full_action(DsLogAdmin::wrap);
    ACE_ERROR_RETURN((LM_ERROR,"Testing log action halt failed\n"),-1);

  }
  ACE_CATCH(DsLogAdmin::LogFull, xLogFull)
    {
      ACE_DEBUG((LM_DEBUG,"Correctly caught exception LogFull\n"));

      CORBA::ULongLong nrecords = basicLog_->get_n_records();

      ACE_DEBUG((LM_ERROR,"The number of records written was %d\n", ACE_U64_TO_U32(nrecords)));

      DsLogAdmin::AvailabilityStatus logStatus = basicLog_->get_availability_status();

      if (!logStatus.log_full)
        ACE_ERROR_RETURN((LM_ERROR,"Failed the log's get_availability_status.\n"),-1);

      ACE_DEBUG((LM_DEBUG,"Log's availability status correctly set to log_full\n"));

      basicLog_->set_log_full_action(DsLogAdmin::wrap);
      DsLogAdmin::LogFullAction logFullAction = basicLog_->get_log_full_action();
      if (logFullAction != DsLogAdmin::wrap)
        ACE_ERROR_RETURN((LM_ERROR,"Setting log full action to wrap failed\n"),-1);

      ACE_TRY_EX (SECOND)
        {
          int i = this->write_records(0, ACE_TRY_ENV);
          ACE_TRY_CHECK_EX (SECOND);

          ACE_DEBUG((LM_DEBUG,"Test to wrap worked. %d written.\n",i));
        }
      ACE_CATCH(DsLogAdmin::LogFull, xLogFull)
        {
          ACE_DEBUG((LM_ERROR,"Caught exception LogFull.  Fail testing wrapping of the log.\n"));
          return -1;
        }
      ACE_CATCHANY
        {
          ACE_PRINT_EXCEPTION(ACE_ANY_EXCEPTION,"test_LogAction");
          return -1;
        }
      ACE_ENDTRY;
    }
  ACE_ENDTRY;

  ACE_RETURN(0);
}

int BasicLog_Test::write_records(CORBA::ULongLong numberOfRecords, CORBA::Environment &/*ACE_TRY_ENV*/)
{

  CORBA::ULongLong size = numberOfRecords;
  if (size == 0)
    size = basicLog_->get_max_size();

#ifndef ACE_LACKS_LONGLONG_T
  ACE_DEBUG((LM_INFO,"B_test: Write_records: Max size in bytes %Q.\n", size));
#else
  ACE_DEBUG((LM_INFO,"B_test: Write_records: Max size in bytes %u.\n",
             size.lo()));
#endif

  int i = 0;
  CORBA::ULongLong l;
  for(l = 0 ; l < size ;)
    {
      DsLogAdmin::Anys record;
      record.length(1);
      ACE_CString str ("For the test of log "
                       "full action, writing test record, ");
      char number[32];
      ACE_OS::sprintf (number, "%d.", i);
      str += number;
      l += str.length ();
      // record owns internal os string
      ++i;
      CORBA::String_var t(str.c_str ());
      record[0] <<= t.in ();
      basicLog_->write_records(record);
    }

#ifndef ACE_LACKS_LONGLONG_T
  ACE_DEBUG((LM_INFO,"B_test: Write_records: currentsize in bytes %Q.\n", size));
#else
  ACE_DEBUG((LM_INFO,"B_test: Write_records: currentsize in bytes %u.\n", size.lo()));
#endif

  ACE_RETURN(i);
}


int
BasicLog_Test::test_adminState()
{
  ACE_DECLARE_NEW_CORBA_ENV;

  basicLog_->set_administrative_state(DsLogAdmin::locked);
  ACE_TRY
    {
      this->write_records(0, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      ACE_ERROR_RETURN((LM_ERROR,"Setting administrative state to lock failed.  DsLogAdmin::LogLocked not thrown.\n"),-1);

    }
  ACE_CATCH(DsLogAdmin::LogLocked, xLocked)
    {
      ACE_DEBUG((LM_DEBUG,"Setting administrative state to lock succeeded.  DsLogAdmin::LogLocked was caught.\n"));
    }
  ACE_CATCHANY
    {
      basicLog_->set_administrative_state(DsLogAdmin::unlocked);
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "test_adminState");
      ACE_ERROR_RETURN((LM_ERROR,"Setting administrative state to lock failed. Exception throw.\n"),-1);
    }
  ACE_ENDTRY;

  basicLog_->set_administrative_state(DsLogAdmin::unlocked);
  ACE_TRY_EX(SECOND)
    {
      this->write_records(0, ACE_TRY_ENV);
      ACE_TRY_CHECK_EX (SECOND);
      ACE_DEBUG((LM_DEBUG,"Setting administrative state to succeeded.  DsLogAdmin::LogLocked not thrown.\n"));
      ACE_RETURN(0);

    }
  ACE_CATCH(DsLogAdmin::LogLocked, xLocked)
    {
      ACE_DEBUG((LM_DEBUG,"Setting administrative state to lock faild.  DsLogAdmin::LogLocked was caught.\n"));
    }
  ACE_ENDTRY;
  ACE_RETURN(0);
}


int
BasicLog_Test::test_logSize()
{
  ACE_DECLARE_NEW_CORBA_ENV;

  ACE_TRY
    {
      basicLog_->set_max_size(1);
      ACE_TRY_CHECK;
      ACE_ERROR_RETURN((LM_ERROR,"Setting max size less than current size failed.  DsLogAdmin::InvalidParam not thrown.\n"),
                       -1);
    }
  ACE_CATCH(DsLogAdmin::InvalidParam, xParam)
    {
      ACE_DEBUG((LM_DEBUG,"Setting max size less than current size succeeded.  DsLogAdmin::InvalidParam caught.\n"));
    }
  ACE_ENDTRY;

  ACE_TRY_EX(SECOND)
    {
      basicLog_->set_max_size(0);

      CORBA::ULongLong nrecords = basicLog_->get_n_records();
      int i = this->write_records(0, ACE_TRY_ENV);
      ACE_TRY_CHECK_EX (SECOND);
      ACE_DEBUG((LM_DEBUG,"Increasing max size succeeded. Old record count, %d. New record count, %d\n", ACE_U64_TO_U32(nrecords), i));
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "test_logSize");
      ACE_ERROR_RETURN((LM_ERROR,"Increasing max size failed. Exception thrown.\n"),-1);

    }
  ACE_ENDTRY;
  ACE_RETURN(0);
}

int
BasicLog_Test::test_logCompaction(CORBA::ULong lifeExpectancy)
{
  ACE_DECLARE_NEW_CORBA_ENV;

  CORBA::ULongLong old_n_records = basicLog_->get_n_records();
  if (old_n_records <= 0)
    {
      ACE_TRY
        {
          this->write_records(0, ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }
      ACE_CATCHANY
        {
          ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "test_logCompaction");
          ACE_ERROR_RETURN
            ((LM_ERROR,"Caught exception trying to write records for test_logCompaction.\n"), -1);
        }
      ACE_ENDTRY;
    }
  CORBA::ULong old_max_life = basicLog_->get_max_record_life();
  ACE_TRY_EX(SECOND)
    {
      basicLog_->set_max_record_life (lifeExpectancy);
      // @@: need to modify set_max_record_life
      ACE_TRY_CHECK_EX (SECOND);
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "test_logCompaction");
      ACE_ERROR_RETURN
        ((LM_ERROR,"Caught exception setting max record life test_logCompaction.\n"), -1);
    }
  ACE_ENDTRY;

  ACE_OS::sleep(lifeExpectancy); // make sure the records have aged.
  CORBA::ULongLong new_n_records = basicLog_->get_n_records();
  basicLog_->set_max_record_life(old_max_life);
  if (new_n_records != 0)
    {
      ACE_ERROR_RETURN((LM_ERROR,
                        "Log compaction failed, because %d records remain.\n"),
                       1);
    }
  ACE_RETURN(0);
}

//use ACE_U64_TO_U32 to convert ULongLong to ULong in call to this function
//Writes and retrieves numberOfrecordsToWrite records.
int
BasicLog_Test::test_retrieval(CORBA::ULong numberOfRecordsToWrite)
{
  int rc = 0;
  TimeBase::TimeT now;

  ORBSVCS_Time::Time_Value_to_TimeT(now,ACE_OS::gettimeofday());
#ifndef ACE_LACKS_LONGLONG_T
  ACE_DEBUG((LM_INFO,"Time : %Q\n", now));
#else
  ACE_DEBUG((LM_INFO,"Time : %u\n", now.lo()));
#endif
  DsLogAdmin::Anys records(numberOfRecordsToWrite);

  ACE_TRY_NEW_ENV
    {
      // @@ Kirsh: please use CORBA::is_nil()
      // assert(basicLog_ != 0);
      // @@ Krish: assert is the wrong thing to do!! You should log
      // the error (somehow) and try to continue!
      assert (!CORBA::is_nil (basicLog_.in ()));
      basicLog_->set_max_size(0);
      // @@: need to modify set_max_size
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "test_retrieval");
      ACE_ERROR_RETURN((LM_ERROR,"set_max_size failed in test_retrieval.\n"),-1);
    }
  ACE_ENDTRY;

  ACE_DEBUG((LM_INFO,"set max\n"));

  CORBA::ULong i;
  for(i = 0 ; i < numberOfRecordsToWrite; ++i)
    {
      records.length(i+1);
      ACE_CString str ("For the test of retrieving "
                       "records from the log. Record, " );
      char number[32];
      ACE_OS::sprintf (number, "%d.", i);
      str += number;
      CORBA::String_var t(str.c_str ());
      records[i] <<= t.in ();
    }
  basicLog_->write_records(records);

  ACE_DEBUG((LM_INFO,"---- Testing Forward retrieval with null iterator----\n"));

  DsLogAdmin::Iterator_ptr nullIterator = DsLogAdmin::Iterator::_nil();
  DsLogAdmin::RecordList_var list = basicLog_->retrieve(now,numberOfRecordsToWrite,nullIterator);
  if (list->length() != numberOfRecordsToWrite)
    {
      ACE_DEBUG((LM_DEBUG,"Forward retrieval wrong asked for %d records and received %d records.\n",
                 numberOfRecordsToWrite,
                 list->length()));
      rc = -1;
    }

  for (i = 0; i < list->length(); ++i)
    {
      const char *from;
      const char *to;

      records[i] >>= from;
      list[i].info >>= to;
      if (strcmp(from,to))
        {
          ACE_DEBUG((LM_DEBUG,"Test of retrieval: failed. String %s does not match %s\n",from,to));
          rc = -1;
        }
      else
        for (i = 0; i < list->length(); ++i)
          {
            const char* from;
            list[i].info >>= from;
            ACE_DEBUG((LM_DEBUG,"Test of retrieval: %d retrieved record with value %s\n",i,from));
          }
    }

  /** -------------<
  // backwards causes the server to core

#if 0

  ACE_DEBUG((LM_ERROR,"---- Testing backwards retrieval with null iterator----\n"));
  // test backwards with null iterator
  // get records prior from now
  ORBSVCS_Time::Time_Value_to_TimeT(now,ACE_OS::gettimeofday());
  DsLogAdmin::RecordList_var list2 = basicLog_->retrieve(now,-numberOfRecordsToWrite,nullIterator);
  ACE_DEBUG((LM_ERROR,"** 2.2"));
  if (list2->length() != numberOfRecordsToWrite) // **** list is null ptr
    {
      ACE_DEBUG((LM_DEBUG,"Backward retrieval wrong asked for %d records and received %d records.\n",
                 numberOfRecordsToWrite,
                 list2->length()));
      rc = -1;
    }
  ACE_DEBUG((LM_ERROR,"** 2.5"));
  for (i = ((list2->length() < numberOfRecordsToWrite) ? list2->length() : numberOfRecordsToWrite) -1 ; i > 0;  --i)
    {
      char *from, *to;
      records[i] >>= from;
      list2[i].info >>= to;
      if (strcmp(from,to))
        {
          ACE_DEBUG((LM_DEBUG,"Test of retrieval: failed. String %s does not match %s\n",from,to));
          rc = -1;
        }
      else
        for (i = 0; i < list2->length(); ++i)
          {
            char* stg;
            list2[i].info >>= stg;
            ACE_DEBUG((LM_DEBUG,"Test of retrieval: %d retrieved record with value %s\n",i,stg));
          }
    }
#endif

  ACE_DEBUG((LM_INFO,"---- Forward retrieval using iterator----\n"));

  DsLogAdmin::Iterator_var iterator;
  list = basicLog_->retrieve(now,numberOfRecordsToWrite-4,iterator.out());
  if ( CORBA::is_nil(iterator.in()))
    {
      ACE_DEBUG((LM_DEBUG,"Test of retrieval: failed by returning NULL iterator, probably because "
                 "not enough records were asked for.\n"));
    }
  else
    {

      // simple test of iterator
      for (i = 0; i < list->length(); ++i)
        {
          char *from, *to;
          records[i] >>= from;
          list[i].info >>= to;
          if (strcmp(from,to))
            {
              ACE_DEBUG((LM_DEBUG,"Test of retrieval: failed. String %s does not match %s\n",from,to));
              rc = -1;
            }
        }
    }

  // backwards causes the server to core
#if 0

  ACE_DEBUG((LM_INFO,"---- Backwards retrieval using iterator----"));

  if (!CORBA::is_nil(iterator))
    iterator->destroy();
  // simple test of iterator
  list = basicLog_->retrieve(now,-numberOfRecordsToWrite,iterator.out());
  if ((list = iterator->get(0,numberOfRecordsToWrite/2))->length() != numberOfRecordsToWrite)
    {
      ACE_DEBUG((LM_DEBUG,"Backward retrieval: wrong asked for %d records and received %d records.\n",
                 numberOfRecordsToWrite/2,
                 list->length()));
      rc = -1;
    }
  if (!list->length())
    {
      ACE_DEBUG((LM_ERROR,"Test of retrieval: with iterator failed because number of records specified too small.\n"));
    }
  iterator->destroy();
#endif

  **/
  if (!rc)
    {
      ACE_DEBUG((LM_ERROR,"Test of retrieval: succeeded.\n"));
    }
  ACE_RETURN(rc);
}



int
BasicLog_Test::test_query(CORBA::ULong numberOfRecordsToWrite)
{
    // save for compares
    DsLogAdmin::Anys infos(numberOfRecordsToWrite);
    return -1;
}


int
BasicLog_Test::parse_args (void)
{
  ACE_Get_Opt get_opts (argc_, argv_, "di");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'd':  // debug flag
        TAO_debug_level++;
        break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s"
                           " [-d]"
                           "\n"
                           "    -d: increase debug level\n",
                           this->argv_ [0]),
                          -1);
      }

  // Indicates successful parsing of command line.
  return 0;
}

void
BasicLog_Test::resolve_naming_service (CORBA::Environment &ACE_TRY_ENV)
{
  CORBA::Object_var naming_obj =
    this->orb_->resolve_initial_references (naming_sevice_name_,
                                            ACE_TRY_ENV);
  ACE_CHECK;

  // Need to check return value for errors.
  if (CORBA::is_nil (naming_obj.in ()))
    ACE_THROW (CORBA::UNKNOWN ());

  this->naming_context_ =
    CosNaming::NamingContext::_narrow (naming_obj.in (), ACE_TRY_ENV);
  ACE_CHECK;
}

void
BasicLog_Test::resolve_basic_factory (CORBA::Environment &ACE_TRY_ENV)
{
  CosNaming::Name name (1);
  name.length (1);
  name[0].id = CORBA::string_dup (basic_log_factory_name_);

  CORBA::Object_var obj =
    this->naming_context_->resolve (name,
                                   ACE_TRY_ENV);
  ACE_CHECK;

  this->factory_ =
    DsLogAdmin::BasicLogFactory::_narrow (obj.in (),
                                          ACE_TRY_ENV);
  ACE_CHECK;
}

void
BasicLog_Test::destroy_log()
{
  ACE_TRY_NEW_ENV
    {
      if (!CORBA::is_nil(basicLog_.in()))
        {
          basicLog_->destroy(ACE_TRY_ENV);
          ACE_TRY_CHECK;

          basicLog_ = DsLogAdmin::BasicLog::_nil();
        }
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "Destroying log");
    }
  ACE_ENDTRY;
}

int BasicLog_Test::test_log_destroy()
{

  ACE_DEBUG((LM_ERROR,"Testing destroy log\n"));
  this->basicLog_->destroy();
  ACE_TRY_NEW_ENV
    {
      this->write_records(1, ACE_TRY_ENV);
      ACE_TRY_CHECK;
      ACE_DEBUG((LM_INFO,"Wrote to log\n"));
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION(ACE_ANY_EXCEPTION,"Destroying log");
      ACE_DEBUG((LM_ERROR,"Test of destroy log succeeded and caught exception.\n"));
      return 0;
    }
  ACE_ENDTRY;
  ACE_ERROR_RETURN((LM_ERROR,"Test of destroy log failed and no exception thrown.\n"), -1);
}
