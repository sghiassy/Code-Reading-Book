<%@ page contentType="text/plain" %><jsp:useBean id="simpleBean"
    scope="session" class="org.apache.tester.SessionBean"/><%
  if ("From Session07a".equals(simpleBean.getStringProperty())) {
    out.println("Session07 PASSED");
  } else {
    out.println("Session07 FAILED - Property = '" +
                simpleBean.getStringProperty() + "'");
  }
%>
