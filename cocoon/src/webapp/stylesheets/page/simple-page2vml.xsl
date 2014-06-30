<?xml version="1.0"?>

<!-- Written by Theodore B. Achacoso, MD 991122 ted@groupserve.com -->

<!--
    The code is based on Motorola's VoxML (Version 1.2) implementation of
    the emerging VoiceXML standard (version 0.9 as of 17 August '99).
    Foremost is the absence of <form></form> tags that delineate input. That
    said, when this file is rendered in Motorola's Mobile ADK simulator
    (MADK, version 1 beta 4 this November) the interaction looks like this:

    Computer: Hello world! This is my first voice enabled cocoon page.

    If user says "repeat" then the computer repeats the prompt.
    If the user says "goodbye" then the computer says "Goodbye" and ends the
    session.
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="page">
    <DIALOG>
     <STEP NAME="init">
      <PROMPT><xsl:apply-templates/></PROMPT>
      <INPUT TYPE="OPTIONLIST">
       <OPTION NEXT="#init">repeat</OPTION>
       <OPTION NEXT="#goodbye">goodbye</OPTION>
      </INPUT>
     </STEP>
     <STEP NAME="goodbye">
       <PROMPT>Goodbye</PROMPT>
       <INPUT TYPE="NONE" NEXT="#end"/>
     </STEP>
    </DIALOG>
  </xsl:template>

  <xsl:template match="title">
   <!-- ignore -->
  </xsl:template>

  <xsl:template match="para">
   <xsl:apply-templates/>
  </xsl:template>

</xsl:stylesheet>
