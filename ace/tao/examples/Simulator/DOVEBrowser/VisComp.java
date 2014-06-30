// VisComp.java,v 1.2 1998/03/30 17:34:59 mk1 Exp
// 
// = FILENAME
//    VisComp.java
//
// = AUTHOR
//    Michael Kircher (mk1@cs.wustl.edu)
//
// = DESCRIPTION
//   This is the interface for Java Beans.
//
// ============================================================================


public interface VisComp extends java.util.Observer {

  public void setName (String title);
  public int getProperty ();
}
