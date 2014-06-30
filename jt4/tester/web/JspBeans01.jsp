<%@ page contentType="text/plain" %><jsp:useBean id="bean" class="org.apache.tester.SessionBean"/>JspBeans01 PASSED
lifecycle = <%= bean.getLifecycle() %>
stringProperty= <%= bean.getStringProperty() %>
toString = <%= bean.toString() %>
