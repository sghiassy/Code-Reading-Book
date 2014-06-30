// Step 1 -- Retrieve helper "beans" from the BSF framework

scriptaction = bsf.lookupBean( "scriptaction" )
manager      = bsf.lookupBean( "manager" )            
request      = bsf.lookupBean( "request" )
logger       = bsf.lookupBean( "logger" )
actionMap    = bsf.lookupBean( "actionMap" )

// Step 2 -- Perform the action

logger.debug( "START deleteunknown.js" )

// Retrieve things from the session and request
// NOTE: they are all of type java.lang.String

id      = request.getParameter( "id" )

logger.debug( "Raw" )
logger.debug( "  id            [" + id + "]" )

// Actually do the database work

// We have the choice of declaring things out here and making them explicitly
// null, or we have to use a different comparison in the "finally" block (defined?)

dbselector = null
datasource = null
conn = null
addStatement = null

try
{
    dbselector = manager.lookup( scriptaction.DB_CONNECTION )
    datasource = dbselector.select( "ceabplanner" )
    conn = datasource.getConnection()    
    deleteStatement = conn.prepareStatement(
              "DELETE FROM otherCourses WHERE id = ?"
    )

    deleteStatement.setString( 1, id ); 

    result = deleteStatement.executeUpdate()
    logger.debug( "Result #1 [" + result + "]" )

    conn.commit()

    actionMap.put( "scriptaction-continue", "" )
}
catch( ex )
{
    logger.debug( "Caught Exception" )
    logger.debug( "  " + ex )
}
finally
{
    if ( null != addStatement ) { addStatement.close() }
    if ( null != conn ) { conn.close() }
    if ( null != datasource ) { dbselector.release( datasource ) }
    if ( null != dbselector ) { manager.release( dbselector ) }
}

logger.debug( "END deleteunknown.js" )
