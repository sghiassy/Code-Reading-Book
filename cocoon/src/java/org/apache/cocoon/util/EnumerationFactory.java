//----------------------------------------------------------------------
// This software is provided as is in the hope that it might be found
// useful.
// You may use and modify it freely provided you keep this copyright
// notice unchanged and mark modifications appropriately.
//
// Bug reports and proposals for improvements are welcome. Please send
// them to the eMail address below.
//
// Christoph Karl Walter Grein
// Hauptstr. 42
// D-86926 Greifenberg
// Germany
//
// eMail: Christ-Usch.Grein@T-Online.de
//
// Copyright (c) 1998 Christoph Karl Walter Grein
//----------------------------------------------------------------------

//====================================================================
// Author    Christoph Grein <Christ-Usch.Grein@T-Online.de>
// Version   1.1
// Date      7 February 1998
//====================================================================
// A factory for the creation of enumeration types (missing in Java).
// The same operations are provided as for Ada, only representation
// specification is missing (which does not seem to make much sense
// in Java).
//
// The idea behind this implementation model is the following:
//
// Each enumeration object [or literal as called in Ada] is internally
// represented by its position number [like in Ada] starting with 0
// for the first one. For external representation, an image string
// may also be defined.
// All operations on enumerations (order relations between objects,
// iterators [in the Ada sense for getting the successor and predeces-
// sor of an object]) are implemented on the internal position number.
//
// Implementing the Ada 'Pos attribute as a function getPos () is
// straight forward as is the implementation of 'Image as a function
// toString () [*], whereas their reverse operations 'Val and 'Value
// present a bit of a problem.
// In order to be able to access all objects created for the given
// class, we define a vector and let the constructors add each object
// upon creation to this vector. Thus each object's position number
// is also its vector index. So getVal [Ada's 'Val] simply returns
// the object stored at the given place; getObject [Ada's 'Value]
// loops thru the vector until it finds the object with the given
// string.
//
// [*] The name toString has deliberately been chosen because of
// Java's convention of calling an operation with this name whenever
// an object's name appears in string context.
//
// There is one last point to take care of. An enumeration class has
// only a fixed number of objects, so we must inhibit the creation of
// new objects (with the "new" operator) outside of the enumeration
// class. However, in order to make this EnumerationFactory class
// work, the constructors need to be public. Therefore we set up the
// requirement that upon derivation from class EnumerationFactory all
// objects be created in the derivation class (using all capital
// letters according to Java's convention) and the constructors be
// made private.
//
// For this class at work, see the example classes EnumerationExample
// and Test_EnumerationExample, which present a few objects and opera-
// tions on them.
//
// Comments and improvements are welcome, see e-mail address above.
//====================================================================
// History:
// Author Version   Date    Reason for Change
//  C.G.   1.0  03.02.1998
//  C.G.   1.1  07.02.1998  getObject returns null if nothing found
//====================================================================

package org.apache.cocoon.util;

/**
 * @author  Christoph Grein
 * @version 1.1
 * A factory for the creation of enumeration types (missing in Java).
 * The same operations are provided as for Ada, only representation
 * specification is missing (which does not seem to make much sense
 * in Java).
 * Enumeration classes are to be derived from this class thereby
 * making the constructors private to inhibit creation outside of
 * the derived class.
 */
public class EnumerationFactory {

  private static java.util.Vector allObjects =  // must be here JDK 1.1.3
    new java.util.Vector (0, 1);  // empty, increment by 1

  private int pos;
  private String image;

  /**
   * Constructors with and without a string representation (image).
   * Make constructors private upon derivation.
   * Be careful: No check is made that the image string is unique!
   * @param image
   */
  public EnumerationFactory (String image) {
    this.pos   = allObjects.size ();
    this.image = image;
    allObjects.addElement (this);
  }
  public EnumerationFactory () {
    this ("");
  }

  //--------------------------------------------------------------------------
  // Order relations:

  /**
   * Order relations Object.op (OtherObject) representing the relation
   * Object op OtherObject.
   * @param enum the right operand
   */
  public boolean lt (EnumerationFactory enum) {                   // "<"
    return this.getPos() < enum.getPos ();
  }
  public boolean le (EnumerationFactory enum) {                   // "<="
    return this.getPos() <= enum.getPos ();
  }
  public boolean gt (EnumerationFactory enum) {                   // ">"
    return this.getPos() > enum.getPos ();
  }
  public boolean ge (EnumerationFactory enum) {                   // ">="
    return this.getPos() >= enum.getPos ();
  }

  // "==" and "equals" are inherited.

  //--------------------------------------------------------------------------
  // Numeric representation:

  /**
   * Access to the numeric representation.
   * @param value the numeric value
   */
  public int getPos () {                                          // Ada'Pos
    return pos;
  }
  public static EnumerationFactory getVal (int value) {           // Ada'Val
    return (EnumerationFactory) allObjects.elementAt (value);
  }

  //--------------------------------------------------------------------------
  // Iterator:

  public static EnumerationFactory getFirst () {                  // Ada'First
    return getVal (0);
  }
  public static EnumerationFactory getLast () {                   // Ada'Last
    return getVal (allObjects.size () - 1);
  }

  public EnumerationFactory getNext () {                          // Ada'Succ
    return getVal (this.getPos () + 1);
  }
  public EnumerationFactory getPrev () {                          // Ada'Pred
    return getVal (this.getPos () - 1);
  }

  //--------------------------------------------------------------------------
  // String representation:

  public String toString () {                                     // Ada'Image
    return image;
  }
  public static EnumerationFactory getObject (String image) {     // Ada'Value
    EnumerationFactory found;
    // Linear search seems good enough because there presumably
    // will not be too many literals.
    for (int i = 0 ; i < allObjects.size () ; i++) {
      found = (EnumerationFactory) allObjects.elementAt (i);
      if (found.toString () == image) {
        return found;
      }
    }
    return null;
  }
}
