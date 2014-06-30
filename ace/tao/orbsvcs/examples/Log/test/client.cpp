//client.cpp,v 1.3 2000/05/16 22:09:32 pradeep Exp

#include "BasicLog_Test.h"

ACE_RCSID(LogService, BasicLog_Test,"client.cpp,v 1.3 2000/05/16 22:09:32 pradeep Exp")


int
main(int argc, char *argv[])
{
  BasicLog_Test log_test;

  ACE_DEBUG((LM_DEBUG, "\n\t Basic Log test \n\n"));

  if (log_test.init(argc, argv) == -1)
    {
      ACE_ERROR_RETURN((LM_ERROR,
			    "Failed initializing BasicLog_Test.\n"),-1);
    }

  // We may want to replace this with a run on the BasicLog_Test.
  // If we cannot create a log, there is no use continuing.
  if (log_test.test_CreateLog() == -1)
    {
      ACE_ERROR_RETURN((LM_ERROR,
                        "Failed creating log and bailing out.\n"),-1);
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The creating log test succeeded.\n"));
    }

  // go easy until this works
  //@@ use ACE_U64_TO_U32 to convert ULongLong to ULong
  //in call to test_retrieval
  if (log_test.test_retrieval(10) == -1)
    {
      ACE_DEBUG((LM_ERROR,"The test of retrieval failed.\n"));
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The test of retrieval succeeded.\n"));
    }

  /**      if (log_test.test_log_destroy() == -1)
           {
           ACE_ERROR_RETURN((LM_ERROR,
           "Failed destroying  log.\n"),-1);
           }
           else
           {
           ACE_DEBUG((LM_ERROR,"The destroying log test succeeded.\n"));
           }

           if (log_test.test_CreateLog() == -1)
           {
           ACE_ERROR_RETURN((LM_ERROR,
           "Failed creating log and bailing out.\n"),-1);
           }
  **/

  if (log_test.test_LogAction() == -1)
    {
      ACE_DEBUG((LM_ERROR,"The log action test failed.\n"));
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The log action test succeeded.\n"));
    }

  if (log_test.test_adminState() == -1)
    {
      ACE_DEBUG((LM_ERROR,"The administration test failed.\n"));
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The administration test succeeded.\n"));
    }

  if (log_test.test_logSize() == -1)
    {
      ACE_DEBUG((LM_ERROR,"The log size test failed.\n"));
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The log size test succeeded.\n"));
    }

  if (log_test.test_logCompaction() == -1)
    {
      ACE_DEBUG((LM_ERROR,"The test of log compaction failed.\n"));
    }
  else
    {
      ACE_DEBUG((LM_ERROR,"The test of log compaction succeeded.\n"));
    }

  return 0;
}
