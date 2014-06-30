<%@ page contentType="text/plain" %><%

        // Accumulate all the reasons this request might fail
        StringBuffer sb = new StringBuffer();
        Object value = null;

        value = request.getAttribute("javax.servlet.error.exception");
        if (value == null) {
            sb.append(" exception is missing/");
        } else if (!(value instanceof java.lang.ArrayIndexOutOfBoundsException)) {
            sb.append(" exception class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        }

        value = request.getAttribute("javax.servlet.error.exception_type");
        if (value == null)
            sb.append(" exception_type is missing/");
        else if (!(value instanceof Class)) {
            sb.append(" exception_type class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            Class clazz = (Class) value;
            String name = clazz.getName();
            if (!"java.lang.ArrayIndexOutOfBoundsException".equals(name)) {
                sb.append(" exception_type is ");
                sb.append(name);
                sb.append("/");
            }
        }

        value = request.getAttribute("javax.servlet.error.message");
        if (value == null)
            sb.append(" message is missing/");
        else if (!(value instanceof String)) {
            sb.append(" message class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else if (!"ErrorPage05 Threw ArrayIndexOutOfBoundsException".equals(value) &&
                   !"ErrorPage08 Threw ArrayIndexOutOfBoundsException".equals(value)) {
            sb.append(" message is not correct");
        }

        value = request.getAttribute("javax.servlet.error.request_uri");
        if (value == null)
            sb.append(" request_uri is missing/");
        else if (!(value instanceof String)) {
            sb.append(" request_uri class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            String request_uri = (String) value;
            String test1 = request.getContextPath() + "/ErrorPage05";
            String test2 = request.getContextPath() + "/WrappedErrorPage05";
            String test3 = request.getContextPath() + "/ErrorPage08";
            String test4 = request.getContextPath() + "/WrappedErrorPage08";
            if (!request_uri.equals(test1) && !request_uri.equals(test2) &&
                !request_uri.equals(test3) && !request_uri.equals(test4)) {
                sb.append(" request_uri is ");
                sb.append(request_uri);
                sb.append("/");
            }
        }

        value = request.getAttribute("javax.servlet.error.servlet_name");
        if (value == null)
            sb.append(" servlet_name is missing/");
        else if (!(value instanceof String)) {
            sb.append(" servlet_name class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            String servlet_name = (String) value;
            if (!"ErrorPage05".equals(servlet_name) &&
                !"ErrorPage08".equals(servlet_name)) {
                sb.append(" servlet_name is ");
                sb.append(servlet_name);
                sb.append("/");
            }
        }

        // Report ultimate success or failure
        if (sb.length() < 1)
            out.println("ErrorPage06 PASSED - JSP");
        else
            out.println("ErrorPage06 FAILED -" + sb.toString());

%>
<%
  Exception e = (Exception)
   request.getAttribute("javax.servlet.error.exception");
  out.println("EXCEPTION:  " + e);
  Class et = (Class)
   request.getAttribute("javax.servlet.error.exception_type");
  out.println("EXCEPTION_TYPE:  " + et.getName());
  String m = (String)
   request.getAttribute("javax.servlet.error.message");
  out.println("MESSAGE:  " + m);
  String ru = (String)
   request.getAttribute("javax.servlet.error.request_uri");
  out.println("REQUEST_URI:  " + ru);
  String sn = (String)
   request.getAttribute("javax.servlet.error.servlet_name");
  out.println("SERVLET_NAME:  " + sn);
%>
