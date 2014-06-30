<%@ page contentType="text/plain" %><jsp:useBean id="simpleBean"
    scope="session" class="org.apache.tester.SessionBean"/><%
  simpleBean.setStringProperty("From Session07a");
  response.sendRedirect("Session07b.jsp");
%>
