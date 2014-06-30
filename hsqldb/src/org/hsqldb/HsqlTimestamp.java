package org.hsqldb;

// fredt@users.sourceforge.net 20010701
// patch 418019 by deforest@users.sourceforge.net
// This uses a deprecated method for jdk 1.x compatibility
// Can be rewritten using java.util.Calendar; for HSQLDB V.2.00

/**
 * Insert the type's description here.
 * Creation date: (2/2/01 4:09:38 PM)
 * @author: Darin DeForest
 */
public abstract class HsqlTimestamp {
/**
 * Timestamp constructor comment.
 */
public HsqlTimestamp() {
   super();
}

 /*
 public static void main(String[] args) {
   String tests[] = {
      "2000-1-1",
      "2000-1-1 12:13",
      "2000-1-1 12:13:14",
      "2000-1-1 12:13:14.15"
      };
   for(int i =0; i <tests.length;i++) {
      String test = tests[i];
      System.out.print("test "+test + " = ");
      try {
         System.out.println(HsqlTimestamp.valueOf(test));
      } catch(Exception e) {
         System.out.println(e);
      }

   }
}
*/

/**
 * Converts a string in JDBC timestamp escape format to a
 * <code>Timestamp</code> value.
 *
 * @param s timestamp in format <code>yyyy-mm-dd hh:mm:ss.fffffffff</code>
 * @return corresponding <code>Timestamp</code> value
 * @exception java.lang.IllegalArgumentException if the given argument
 * does not have the format <code>yyyy-mm-dd hh:mm:ss.fffffffff</code>
 */

public static java.sql.Timestamp valueOf(String s) {

   final String formatError = "Timestamp format must be yyyy-mm-dd hh:mm:ss.fffffffff";
   final String zeros = "000000000";

   if (s == null) {
      throw new java.lang.IllegalArgumentException("null string");
   }
   // Split the string into date and time components
   int dividingSpace = s.trim().indexOf(' ');
   String date_s = null;
   String time_s = null;
   ;
   if (dividingSpace != -1) {
      date_s = s.substring(0, dividingSpace);
      time_s = s.substring(dividingSpace + 1);
   } else {
      date_s = s;
   }
   if (date_s.length() == 0) {
      throw new java.lang.IllegalArgumentException(formatError);
   }

   // Parse the date

   // Convert the date

   final int firstDash = date_s.indexOf('-');
   final int secondDash = date_s.indexOf('-', firstDash + 1);
   if ((firstDash == -1) | (secondDash == -1) | (secondDash >= date_s.length() - 1)) {
      throw new java.lang.IllegalArgumentException(formatError);
   }
   final int year = Integer.parseInt(date_s.substring(0, firstDash)) - 1900;
   final int month = Integer.parseInt(date_s.substring(firstDash + 1, secondDash)) - 1;
   final int day = Integer.parseInt(date_s.substring(secondDash + 1));

   int hour = 0;
   int minute = 0;
   int second = 0;
   int a_nanos = 0;
   // Convert the time; default missing nanos
   // Parse the time
   if (time_s != null) {
      int firstColon = time_s.indexOf(':');
      int secondColon = time_s.indexOf(':', firstColon + 1);
      final int period = time_s.indexOf('.', secondColon + 1);
      if ((firstColon == -1) | (secondColon == -1) | (secondColon >= time_s.length() - 1)) {
         throw new java.lang.IllegalArgumentException();
      }
      hour = Integer.parseInt(time_s.substring(0, firstColon));
      minute = Integer.parseInt(time_s.substring(firstColon + 1, secondColon));
      if ((period > 0) & (period < time_s.length() - 1)) {
         second = Integer.parseInt(time_s.substring(secondColon + 1, period));
         String nanos_s = time_s.substring(period + 1);
         if (nanos_s.length() > 9)
            throw new java.lang.IllegalArgumentException(formatError);
         if (!Character.isDigit(nanos_s.charAt(0)))
            throw new java.lang.IllegalArgumentException(formatError);
         nanos_s = nanos_s + zeros.substring(0, 9 - nanos_s.length());
         a_nanos = Integer.parseInt(nanos_s);
      } else if (period > 0) {
         throw new java.lang.IllegalArgumentException(formatError);
      } else {
         second = Integer.parseInt(time_s.substring(secondColon + 1));
      }
   }

   return new java.sql.Timestamp(year, month, day, hour, minute, second, a_nanos);
}}