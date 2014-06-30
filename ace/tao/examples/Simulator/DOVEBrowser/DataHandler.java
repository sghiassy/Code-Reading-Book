// DataHandler.java,v 1.2 1998/09/30 15:27:28 cdgill Exp
// 
// = FILENAME
//    DataHandler.java
//
// = AUTHOR
//    Michael Kircher (mk1@cs.wustl.edu)
//
// = DESCRIPTION
//   This is the interface for data handling in the simulation demo
//   using the Event Channel as transport media.
//
// ============================================================================

import org.omg.CORBA.*;

public interface DataHandler {
  
  // entry point for an consumer to put data event into the data handler
  public void update (RtecEventComm.Event event);
  
  // get a list of the Observables names to allow selection
  public java.util.Enumeration getObservablesList ();
 
  // get a specific Observable
  public DemoObservable getObservable(String name);

  // get the property of an observable
  public int getObservableProperty (String name);
}


    
    
