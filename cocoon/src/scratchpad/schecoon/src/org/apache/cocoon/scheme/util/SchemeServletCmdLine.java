package org.apache.cocoon.scheme.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.net.Authenticator;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.PasswordAuthentication;
import java.net.URL;
import java.net.URLConnection;

/**
 * REPL (Read Eval Print Loop) program that sends commands to a Scheme
 * interpreter running in a servlet. Assumes the URL uses basic
 * authentication.
 *
 * The best use of this REPL program is from within Emacs, as it will
 * have nice command history facilities.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since December  9, 2001
 */
public class SchemeServletCmdLine
{
  BufferedReader in;
  String url;
  
  public static void main(String[] args)
  {
    String url = "http://localhost:8080/schecoon/eval";

    if (args.length != 0) {
      if (args[0].indexOf("-h") != -1) {
        System.out.println("usage: SchemeServletCmdLine URL");
        System.exit(1);
      }
      else
        url = args[0];
    }

    System.out.println("Connecting to " + url);

    new SchemeServletCmdLine(url).repl();
  }

  public SchemeServletCmdLine(String url)
  {
    Authenticator.setDefault(new CmdLineAuthenticator());
    in = new BufferedReader(new InputStreamReader(System.in));
    this.url = url;
  }

  public void repl()
  {
    while (true) {
      System.out.print("scheme> ");

      String evalString;
      try {
        evalString = in.readLine();
        if (evalString == null) {
          System.out.println("\nQuit.");
          return;
        }
      }
      catch (IOException ex) {
        System.out.println("Cannot read input line");
        continue;
      }

      try {
        URL schemeURL = new URL(url);
        HttpURLConnection conn = (HttpURLConnection)schemeURL.openConnection();
        conn.setDoOutput(true);
        conn.setDoInput(true);
        conn.setRequestMethod("POST");

        OutputStream out = conn.getOutputStream();
        Writer wout = new OutputStreamWriter(out, "UTF-8");
        wout.write(evalString);
        wout.flush();
        wout.close();
        
        InputStream is = conn.getInputStream();
        BufferedReader bin = new BufferedReader(new InputStreamReader(is));
        String line;
        while ((line = bin.readLine()) != null) {
          System.out.println(line);
        }
        is.close();

        conn.disconnect();
        
      } catch (MalformedURLException e) {
        System.out.println ("Invalid URL");
      } catch (IOException e) {
        System.out.println ("Error reading URL");
      }
    }
  }

  class CmdLineAuthenticator extends Authenticator
  {
    public CmdLineAuthenticator() {}

    protected PasswordAuthentication getPasswordAuthentication()
    {
      String username = null;
      String password = null;

      try {
        System.out.print("Username: ");
        username = in.readLine();
        System.out.print("Password: ");
        // FIXME: how to read the password securely? Java doesn't seem
        // to have a nice way to do it:
        // http://www.javaworld.com/javaworld/javaqa/2000-04/01-qa-0407-command.html
        password = in.readLine();
      }
      catch (IOException ex) {
        System.out.println("Caught exception " + ex);
        System.exit(1);
      }

      return new PasswordAuthentication(username, password.toCharArray());
    }
  }
}
