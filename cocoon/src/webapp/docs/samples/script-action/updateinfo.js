// Step 1 -- Retrieve helper "beans" from the BSF framework

scriptaction = bsf.lookupBean( "scriptaction" )
manager      = bsf.lookupBean( "manager" )            
request      = bsf.lookupBean( "request" )
logger       = bsf.lookupBean( "logger" )
actionMap    = bsf.lookupBean( "actionMap" )

// Step 2 -- Perform the action

logger.debug( "START updateinfo.js" )

// Retrieve things from the session and request
// NOTE: they are all of type java.lang.String

session       = request.getSession( false )
uwid 	      = session.getAttribute( "uwid" )
studentname   = request.getParameter( "studentname" )
studentnumber = request.getParameter( "studentnumber" )
current_term  = request.getParameter( "current_term" )
note          = request.getParameter( "note" )

logger.debug( "Raw" )
logger.debug( "  uwid           [" + uwid + "]" )
logger.debug( "  studentname    [" + studentname + "]" )
logger.debug( "  studentnumber  [" + studentnumber + "]" )
logger.debug( "  current_term   [" + current_term + "]" )
logger.debug( "  note           [" + note + "]" )

// Cook things to make them more palatable to the database

note = ( note + "" ) // convert to a JS string
if (note == " ") { note="" }

if ( note.length > 255 )
{
   note = note.substring( 0, 255 ); // take the leading 255 characters
}

logger.debug( "Cooked" )
logger.debug( "  uwid           [" + uwid + "]" )
logger.debug( "  studentname    [" + studentname + "]" )
logger.debug( "  studentnumber  [" + studentnumber + "]" )
logger.debug( "  current_term   [" + current_term + "]" )
logger.debug( "  note           [" + note + "]" )

// We have the choice of declaring things out here and making them explicitly
// null, or we have to use a different comparison in the "finally" block (defined?)

dbselector = null
datasource = null
conn = null
updateStatement = null

try
{
    dbselector = manager.lookup( scriptaction.DB_CONNECTION )
    datasource = dbselector.select( "ceabplanner" )
    conn = datasource.getConnection()    
    updateStatement = conn.prepareStatement(
        "UPDATE students SET name = ?, current_term = ?, uw_id = ? WHERE uw_userid = ? "
    )
              
    updateStatement.setString( 1, studentname ); 
    updateStatement.setString( 2, current_term ); 
    updateStatement.setString( 3, studentnumber ); 
    updateStatement.setString( 4, uwid ); 
      
    result = updateStatement.executeUpdate()
    logger.debug( "Result #1 [" + result + "]" )

    updateStatement = conn.prepareStatement(
        "UPDATE studentnotes SET note = ? WHERE student = ( SELECT id FROM students WHERE uw_userid = ? )"
    )

    updateStatement.setString( 1, note ); 
    updateStatement.setString( 2, uwid ); 

    result = updateStatement.executeUpdate()
    logger.debug( "Result #2 [" + result + "]" )

    conn.commit()
    
    actionMap.put( "scriptaction-continue", "" )
    session.setAttribute( "results", "<SUCCESS>Information Saved at " + Date() + "</SUCCESS>" )

}
catch( ex )
{
    logger.debug( "Caught Exception" )
    logger.debug( "  " + ex )
}
finally
{
    if ( null != updateStatement ) { updateStatement.close() }
    if ( null != conn ) { conn.close() }
    if ( null != datasource ) { dbselector.release( datasource ) }
    if ( null != dbselector ) { manager.release( dbselector ) }
}

logger.debug( "END updateinfo.js" )
