<%@ page contentType="text/plain" %><%
  StringBuffer results = new StringBuffer();
  String remoteUser = request.getRemoteUser();
  if (remoteUser == null) {
    results.append(" Not Authenticated/");
  } else if (!"tomcat".equals(remoteUser)) {
    results.append(" Authenticated as '");
    results.append(remoteUser);
    results.append("'/");
  }
  if (!request.isUserInRole("tomcat")) {
    results.append(" Not in role 'tomcat'/");
  }
  if (!request.isUserInRole("alias")) {
    results.append(" Not in role 'alias'/");
  }
  if (request.isUserInRole("unknown")) {
    results.append(" In role 'unknown'/");
  }
  if (results.length() < 1) {
    out.println("Authentication04 PASSED");
  } else {
    out.print("Authentication04 FAILED -");
    out.println(results.toString());
  }
%>
