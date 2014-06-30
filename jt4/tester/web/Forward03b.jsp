<%
  String specials[] =
    { "javax.servlet.include.request_uri",
      "javax.servlet.include.context_path",
      "javax.servlet.include.servlet_path",
      "javax.servlet.include.path_info",
      "javax.servlet.include.query_string" };

  StringBuffer sb = new StringBuffer();
  if (request.getAttribute("Forward03") == null)
    sb.append(" Cannot retrieve forwarded attribute/");
  request.setAttribute("Forward03b", "This is our very own attribute");
  if (request.getAttribute("Forward03b") == null)
    sb.append(" Cannot retrieve our own attribute/");

  for (int i = 0; i < specials.length; i++) {
    if (request.getAttribute(specials[i]) != null) {
      sb.append(" Exposed attribute ");
      sb.append(specials[i]);
      sb.append("/");
    }
  }

  if (sb.length() < 1) {
    out.println("Forward03 PASSED");
  } else {
    out.print("Forward03 FAILED - ");
    out.println(sb.toString());
  }
%>
