// AnswerEvent.java,v 1.1 1998/03/21 23:58:01 mk1 Exp
// 
// = FILENAME
//    AnswerEvent.java
//
// = AUTHOR
//    Michael Kircher (mk1@cs.wustl.edu)
//
// = DESCRIPTION
//   Event definition for the Dialog for selecting Observables.
//
// ============================================================================

public class AnswerEvent extends java.util.EventObject {

  protected String selected_;

  public AnswerEvent (Object source, String selected) {
    super (source);
    this.selected_ = selected;
  }
}
    
