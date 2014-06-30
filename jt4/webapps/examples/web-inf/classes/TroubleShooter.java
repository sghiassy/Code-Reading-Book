/* $Id: TroubleShooter.java,v 1.2 2001/07/20 02:36:10 pier Exp $
 *
 */

import java.io.IOException;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import javax.servlet.*;
import javax.servlet.http.*;

public class TroubleShooter extends HttpServlet {

    public void printHeader(PrintWriter out, String header) {
        out.println("   <tr>");
        out.println("    <td bgcolor=\"#999999\" colspan=\"2\">");
        out.println("     <b>"+header+"</b>");
        out.println("    </td>");
        out.println("   </tr>");
    }

    public void printValue(PrintWriter out, String key, String val) {
        if (val!=null) {
            if (val.length()>255) val=val.substring(0,128)+" <i>(... more)</i>";
        }
        out.println("   <tr>");
        out.println("    <td bgcolor=\"#cccccc\">"+key+"</td>");
        out.println("    <td bgcolor=\"#ffffff\">"+val+"</td>");
        out.println("   </tr>");
    }

    public void printVoid(PrintWriter out) {
        out.println("   <tr><td bgcolor=\"#ffffff\" colspan=\"2\">&nbsp;</td></tr>");
    }

    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        doGet(request,response);
    }

    public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {

        PrintWriter out = response.getWriter();
        response.setContentType("text/html");

        out.println("<html>");
        out.println(" <head>");
        out.println("  <title>TroubleShooter Servlet Output</title>");
        out.println(" </head>");
        out.println(" <body>");
        out.println("  <center>");
        out.println("   <h2>TroubleShooter Servlet Output</h2>");

        String url=request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+request.getRequestURI();

        out.println("   <table border=\"0\" cellspacing=\"2\" cellpadding=\"2\">");
        out.println("    <tr>");
        out.println("     <td>");

        out.println("      <form action=\""+url+"\" method=\"GET\">");
        out.println("       <input type=\"hidden\" name=\"hiddenName\" value=\"hiddenValue\">");
        out.println("       <input type=\"submit\" name=\"submitName\" value=\"Submit using GET\">");
        out.println("      </form>");

        out.println("     </td>");
        out.println("     <td>");

        out.println("      <form action=\""+url+"\" method=\"POST\">");
        out.println("       <input type=\"hidden\" name=\"hiddenName\" value=\"hiddenValue\">");
        out.println("       <input type=\"submit\" name=\"submitName\" value=\"Submit using POST\">");
        out.println("      </form>");

        out.println("     </td>");
        out.println("     <td>");

        out.println("      <form enctype=\"multipart/form-data\" action=\""+url+"\" method=\"POST\">");
        out.println("       <input type=\"hidden\" name=\"hiddenName\" value=\"hiddenValue\">");
        out.println("       <input type=\"submit\" name=\"submitName\" value=\"Submit using POST with &quot;multipart/form-data&quot; encoding\">");
        out.println("      </form>");

        out.println("     </td>");
        out.println("    </tr>");
        out.println("   </table>");

        out.println("  </center>");
        out.println("  <br>");

        out.println("  <table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"1\">");

        printHeader(out,"Servlet init parameters:");
        Enumeration enum = getInitParameterNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            String val = getInitParameter(key);
            printValue(out,key,val); 
        }
        printVoid(out);

        printHeader(out,"Context init parameters:");
        ServletContext context = getServletContext();
        enum = context.getInitParameterNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            Object val = context.getInitParameter(key);
            printValue(out,key,val.toString());
        }
        printVoid(out);

        printHeader(out,"Context attributes:");
        enum = context.getAttributeNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            Object val = context.getAttribute(key);
            printValue(out,key,val.toString());
        }
        printVoid(out);

        printHeader(out,"Request attributes:");
        enum = request.getAttributeNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            Object val = request.getAttribute(key);
            printValue(out,key,val.toString());
        }
        printVoid(out);

        printHeader(out,"Request Data:");
        printValue(out,"Servlet Name:", getServletName());
        printValue(out,"Protocol:", request.getProtocol());
        printValue(out,"Scheme:", request.getScheme());
        printValue(out,"Server Name:", request.getServerName());
        printValue(out,"Server Port:", Integer.toString(request.getServerPort()));
        printValue(out,"Server Info:", context.getServerInfo());
        printValue(out,"Remote Addr:", request.getRemoteAddr());
        printValue(out,"Remote Host:", request.getRemoteHost());
        printValue(out,"Character Encoding:", request.getCharacterEncoding());
        printValue(out,"Content Length:", Integer.toString(request.getContentLength()));
        printValue(out,"Content Type:", request.getContentType());
        printValue(out,"Locale:", request.getLocale().toString());
        printValue(out,"Default Response Buffer:", Integer.toString(response.getBufferSize()));
        printVoid(out);

        printHeader(out,"Parameter names in this request:");
        enum = request.getParameterNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            String[] val = request.getParameterValues(key);
            for(int i = 0; i < val.length; i++)
                printValue(out,key,val[i]);
        }
        printVoid(out);

        printHeader(out,"Headers in this request:");
        enum = request.getHeaderNames();
        while (enum.hasMoreElements()) {
            String key = (String)enum.nextElement();
            String val = request.getHeader(key);
            printValue(out,key,val);
        }
        printVoid(out);  

        printHeader(out,"Cookies in this request:");
        Cookie[] cookies = request.getCookies();
        if (cookies!=null) for (int i = 0; i < cookies.length; i++) {
            Cookie cookie = cookies[i];
            printValue(out,cookie.getName(),cookie.getValue());
        }
        printVoid(out);

        printHeader(out,"Extra Request Data:");
        printValue(out,"Request Is Secure:", new Boolean(request.isSecure()).toString());
        printValue(out,"Auth Type:", request.getAuthType());
        printValue(out,"HTTP Method:", request.getMethod());
        printValue(out,"Remote User:", request.getRemoteUser());
        printValue(out,"Request URI:", request.getRequestURI());
        printValue(out,"Context Path:", request.getContextPath());
        printValue(out,"Servlet Path:", request.getServletPath());
        printValue(out,"Path Info:", request.getPathInfo());
        printValue(out,"Path Trans:", request.getPathTranslated());
        printValue(out,"Query String:", request.getQueryString());
        printVoid(out);

        printHeader(out,"Session information:");
        SimpleDateFormat format = new SimpleDateFormat("yyyy/MM/dd hh:mm:ss.SSS z");
        HttpSession session = request.getSession(false);
        if (session!=null) {
        printValue(out,"Requested Session Id:", request.getRequestedSessionId());
            printValue(out,"Current Session Id:", session.getId());
            printValue(out,"Current Time:", format.format(new Date()));
            printValue(out,"Session Created Time:", format.format(new Date(session.getCreationTime())));
            printValue(out,"Session Last Accessed Time:", format.format(new Date(session.getLastAccessedTime())));
            printValue(out,"Session Max Inactive Interval Seconds:", Integer.toString(session.getMaxInactiveInterval()));
            printVoid(out);

            printHeader(out,"Session values:");
            enum = session.getAttributeNames();
            while (enum.hasMoreElements()) {
                String key = (String) enum.nextElement();
                Object val = session.getAttribute(key);
                printValue(out,key,val.toString());
            }
        }
        printVoid(out);

        printHeader(out,"Request body content:");
        out.println("   <tr>");
        out.println("    <td bgcolor=\"#ffffff\" colspan=\"2\">");

        out.print("<pre>");
        ServletInputStream in=request.getInputStream();
        for (int i = 0; i < request.getContentLength(); i++) {
            int c=-1;
            try {
                c=in.read();
            } catch (IOException e) {
                out.println();
                out.println("Exception reading data");
                e.printStackTrace(out);
                break;
            }
            out.write(c);
        }
        out.println("</pre>");
        out.println("    </td>");
        out.println("   </tr>");

        out.println("  </table>");
        out.println(" </body>");
        out.println("</html>");
        out.flush();
    }
}

