<%@ page contentType="text/plain" isErrorPage="true" %><%

        // Accumulate all the reasons this request might fail
        StringBuffer sb = new StringBuffer();
        Object value = null;

        if (exception == null) {
            sb.append(" exception is missing/");
        } else {
            if (!(exception instanceof java.lang.ArrayIndexOutOfBoundsException)) {
                sb.append(" exception class is ");
                sb.append(exception.getClass().getName());
                sb.append("/");
            }
            if (!"ErrorPage09 Threw ArrayIndexOutOfBoundsException".equals(exception.getMessage())) {
                sb.append(" exception message is ");
                sb.append(exception.getMessage());
                sb.append("/");
            }
        }

        // Report ultimate success or failure
        if (sb.length() < 1)
            out.println("ErrorPage10 PASSED");
        else
            out.println("ErrorPage10 FAILED -" + sb.toString());

%>
<%
  out.println("EXCEPTION:  " + exception);
%>
