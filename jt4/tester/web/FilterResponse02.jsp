<%@ page contentType="text/plain" %>FilterResponse02 PASSED
<%
        while (true) {
            String message = org.apache.tester.StaticLogger.read();
            if (message == null)
                break;
            out.println(message);
        }
        org.apache.tester.StaticLogger.reset();
%>
