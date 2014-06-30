// Sample javascript script for use with ScriptGenerator

// Step 1 -- Retrieve helper "beans" from the BSF framework

out      = bsf.lookupBean( "output" )
logger   = bsf.lookupBean( "logger" )
resolver = bsf.lookupBean( "resolver" )
source   = bsf.lookupBean( "source" )
objModel = bsf.lookupBean( "objectModel" )
params   = bsf.lookupBean( "parameters" )

// Step 2 -- Generate xml using whatever means you desire

xml = " \
<page> \
  <title>Hello</title> \
  <content> \
    <para>This is my first Cocoon2 page!</para> \
    <para>With help from JavaScript!</para> \
  </content> \
</page>"

// note that you have access to the Cocoon logger

logger.debug( "Debug message from JavaScript" )

// Step 3 -- Append the generated xml to the output StringBuffer

out.append( xml )
