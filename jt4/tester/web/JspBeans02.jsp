<%@ page contentType="text/plain" %><jsp:useBean id="bean" class="org.apache.tester.shared.SharedSessionBean"/>JspBeans02 PASSED
lifecycle = <%= bean.getLifecycle() %>
stringProperty= <%= bean.getStringProperty() %>
toString = <%= bean.toString() %>
