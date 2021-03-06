// server.cpp,v 1.4 2001/02/13 17:24:35 coryan Exp

#include "ace/Get_Opt.h"
#include "ace/Task.h"
#include "test_i.h"

#include "tao/Strategies/advanced_resource.h"

ACE_RCSID(Leader_Followers, server, "server.cpp,v 1.4 2001/02/13 17:24:35 coryan Exp")

const char *ior_output_file = "ior";

int number_of_event_loop_threads = 1;

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "e:o:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
        ior_output_file = get_opts.optarg;
        break;

      case 'e':
        number_of_event_loop_threads = ::atoi (get_opts.optarg);
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-o <iorfile> "
                           "-e <number of event loop threads> "
                           "\n",
                           argv [0]),
                          -1);
      }

  // Indicates sucessful parsing of the command line
  return 0;
}

class Event_Loop_Task : public ACE_Task_Base
{
public:
  Event_Loop_Task (CORBA::ORB_ptr orb)
    :  orb_ (CORBA::ORB::_duplicate (orb))
    {
    }

  int svc (void)
    {
      ACE_DECLARE_NEW_CORBA_ENV;

      ACE_TRY
        {
          this->orb_->run (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }
      ACE_CATCHANY
        {
          ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                               "Exception caught in thread:");
          return -1;
        }
      ACE_ENDTRY;

      ACE_CHECK_RETURN (-1);

      return 0;
    }

private:
  CORBA::ORB_var orb_;
  // ORB reference.
};

int
main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc,
                         argv,
                         "",
                         ACE_TRY_ENV);
      ACE_TRY_CHECK;

      CORBA::Object_var poa_object =
        orb->resolve_initial_references ("RootPOA",
                                         ACE_TRY_ENV);
      ACE_TRY_CHECK;

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in (),
                                      ACE_TRY_ENV);
      ACE_TRY_CHECK;

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      poa_manager->activate (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (parse_args (argc, argv) != 0)
        return -1;

      test_i servant (orb.in ());

      test_var server =
        servant._this (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      CORBA::String_var ior =
        orb->object_to_string (server.in (),
                               ACE_TRY_ENV);
      ACE_TRY_CHECK;

      ACE_DEBUG ((LM_DEBUG, "Activated as <%s>\n", ior.in ()));

      FILE *output_file = ACE_OS::fopen (ior_output_file, "w");
      if (output_file == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Cannot open output file for writing IOR: %s",
                           ior_output_file),
                          -1);
      ACE_OS::fprintf (output_file, "%s", ior.in ());
      ACE_OS::fclose (output_file);

      Event_Loop_Task event_loop_task (orb.in ());

      if (event_loop_task.activate (THR_NEW_LWP | THR_JOINABLE,
                                    number_of_event_loop_threads) != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Cannot activate event_loop threads\n"),
                          -1);

      event_loop_task.thr_mgr ()->wait ();

      ACE_DEBUG ((LM_DEBUG, "Server: Event loop finished\n"));

      root_poa->destroy (1,
                         1,
                         ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}
