/* $XConsortium: prop.c,v 1.6 94/12/16 17:30:38 mor Exp $ */
/******************************************************************************

Copyright (c) 1993  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
******************************************************************************/

#include "xsm.h"

extern Widget clientListWidget;


void
FreePropValues (propValues)

List *propValues;

{
    List	*pv;
    PropValue	*pval;

    for (pv = ListFirst (propValues); pv; pv = ListNext (pv))
    {
	pval = (PropValue *) pv->thing;
	XtFree ((char *) pval->value);
	XtFree ((char *) pval);
    }

    ListFreeAll (propValues);
}



void
FreeProp (prop)

Prop *prop;

{
    FreePropValues (prop->values);
    XtFree (prop->name);
    XtFree (prop->type);
    XtFree ((char *) prop);
}



void
SetInitialProperties (client, props)

ClientRec	*client;
List		*props;

{
    List *pl;

    if (verbose)
	printf("Setting initial properties for %s\n", client->clientId);

    if (client->props)
    {
	/*
	 * The only way client->props could be non-NULL is if the list
	 * was initialized, but nothing was added yet.  So we just free
	 * the head of the list.
	 */

	XtFree ((char *) client->props);
    }

    client->props = props;

    for (pl = ListFirst (props); pl; pl = ListNext (pl))
    {
	Prop		*pprop;
	PropValue	*pval;
	List		*vl;

	pprop = (Prop *) pl->thing;

	if (strcmp (pprop->name, SmDiscardCommand) == 0)
	{
	    if (client->discardCommand)
		XtFree (client->discardCommand);

	    vl = ListFirst (pprop->values);
	    pval = (PropValue *) vl->thing;

	    client->discardCommand = (char *) XtNewString (
		(char *) pval->value);
	}
	else if (strcmp (pprop->name, SmRestartStyleHint) == 0)
	{
	    int hint;

	    vl = ListFirst (pprop->values);
	    pval = (PropValue *) vl->thing;

	    hint = (int) *((char *) (pval->value));

	    if (hint == SmRestartIfRunning || hint == SmRestartAnyway ||
		hint == SmRestartImmediately || hint == SmRestartNever)
	    {
		client->restartHint = hint;
	    }
	}
    }
}



void
SetProperty (client, theProp, freeIt)

ClientRec	*client;
SmProp		*theProp;
Bool		freeIt;

{
    List 	*pl;
    Prop	*pprop = NULL;
    int		found = 0, i;

    /*
     * If the property exists, delete the property values.  We can
     * re-use the actual property header.
     */

    for (pl = ListFirst (client->props); pl; pl = ListNext (pl))
    {
	pprop = (Prop *) pl->thing;

	if (strcmp (theProp->name, pprop->name) == 0 &&
	    strcmp (theProp->type, pprop->type) == 0)
	{
	    FreePropValues (pprop->values);
	    found = 1;
	    break;
	}
    }


    /*
     * Add the new property
     */

    if (!found)
    {
	pprop = (Prop *) XtMalloc (sizeof (Prop));
	pprop->name = XtNewString (theProp->name);
	pprop->type = XtNewString (theProp->type);
    }

    pprop->values = ListInit ();

    for (i = 0; i < theProp->num_vals; i++)
    {
	PropValue *pval = (PropValue *) XtMalloc (sizeof (PropValue));

	pval->length = theProp->vals[i].length;
	pval->value = (XtPointer) XtMalloc (theProp->vals[i].length + 1);
	memcpy (pval->value, theProp->vals[i].value, theProp->vals[i].length);
	((char *) pval->value)[theProp->vals[i].length] = '\0';

	ListAddLast (pprop->values, (char *) pval);
    }

    if (pl)
	pl->thing = (char *) pprop;
    else
	ListAddLast (client->props, (char *) pprop);

    if (strcmp (theProp->name, SmDiscardCommand) == 0)
    {
	if (saveInProgress)
	{
	    /*
	     * We are in the middle of a save yourself.  We save the
	     * discard command we get now, and make it the current discard
	     * command when the save is over.
	     */

	    if (client->saveDiscardCommand)
		XtFree (client->saveDiscardCommand);
	    client->saveDiscardCommand =
		(char *) XtNewString (theProp->vals[0].value);

	    client->receivedDiscardCommand = True;
	}
	else
	{
	    if (client->discardCommand)
		XtFree (client->discardCommand);
	    client->discardCommand =
		(char *) XtNewString (theProp->vals[0].value);
	}
    }
    else if (strcmp (theProp->name, SmRestartStyleHint) == 0)
    {
	int hint = (int) *((char *) (theProp->vals[0].value));

	if (hint == SmRestartIfRunning || hint == SmRestartAnyway ||
	    hint == SmRestartImmediately || hint == SmRestartNever)
	{
	    client->restartHint = hint;
	}
    }

    if (freeIt)
	SmFreeProperty (theProp);
}



void
DeleteProperty (client, propname)

ClientRec	*client;
char		*propname;

{
    List *pl;

    for (pl = ListFirst (client->props); pl; pl = ListNext (pl))
    {
	Prop *pprop = (Prop *) pl->thing;

	if (strcmp (pprop->name, propname) == 0)
	{
	    FreeProp (pprop);
	    ListFreeOne (pl);

	    if (strcmp (propname, SmDiscardCommand) == 0)
	    {
		if (client->discardCommand)
		{
		    XtFree (client->discardCommand);
		    client->discardCommand = NULL;
		}

		if (client->saveDiscardCommand)
		{
		    XtFree (client->saveDiscardCommand);
		    client->saveDiscardCommand = NULL;
		}
	    }
	    break;
	}
    }
}



void
SetPropertiesProc (smsConn, managerData, numProps, props)

SmsConn 	smsConn;
SmPointer 	managerData;
int		numProps;
SmProp 		**props;

{
    ClientRec	*client = (ClientRec *) managerData;
    int		updateList, i;

    if (verbose)
    {
	printf ("Client Id = %s, received SET PROPERTIES ", client->clientId);
	printf ("[Num props = %d]\n", numProps);
    }

    updateList = (ListCount (client->props) == 0) &&
	numProps > 0 && client_info_visible;

    for (i = 0; i < numProps; i++)
    {
	SetProperty (client, props[i], True /* free it */);
    }

    free ((char *) props);

    if (updateList)
    {
	/*
	 * We have enough info from the client to display it in our list.
	 */

	UpdateClientList ();
	XawListHighlight (clientListWidget, current_client_selected);
    }
    else if (client_prop_visible && clientListRecs &&
	clientListRecs[current_client_selected] == client)
    {
	DisplayProps (client);
    }
}



void
DeletePropertiesProc (smsConn, managerData, numProps, propNames)

SmsConn 	smsConn;
SmPointer 	managerData;
int		numProps;
char		**propNames;

{
    ClientRec	*client = (ClientRec *) managerData;
    int		i;

    if (verbose) {
	printf ("Client Id = %s, received DELETE PROPERTIES ",
	    client->clientId);
	printf ("[Num props = %d]\n", numProps);
    }

    for (i = 0; i < numProps; i++)
    {
	if (verbose)
	    printf ("   Name:	%s\n", propNames[i]);

	DeleteProperty (client, propNames[i]);

	free (propNames[i]);
    }

    free ((char *) propNames);
}



void
GetPropertiesProc (smsConn, managerData)

SmsConn 	smsConn;
SmPointer 	managerData;

{
    ClientRec	*client = (ClientRec *) managerData;
    SmProp	**propsRet, *propRet;
    SmPropValue *propValRet;
    Prop	*pprop;
    PropValue	*pval;
    List	*pl, *pj;
    int		numProps;
    int		index, i;

    if (verbose)
    {
	printf ("Client Id = %s, received GET PROPERTIES\n", client->clientId);
	printf ("\n");
    }

    /*
     * Unfortunately, we store the properties in a format different
     * from the one required by SMlib.
     */

    numProps = ListCount (client->props);
    propsRet = (SmProp **) XtMalloc (numProps * sizeof (SmProp *));

    index = 0;
    for (pl = ListFirst (client->props); pl; pl = ListNext (pl))
    {
	propsRet[index] = propRet = (SmProp *) XtMalloc (sizeof (SmProp));

	pprop = (Prop *) pl->thing;

	propRet->name = XtNewString (pprop->name);
	propRet->type = XtNewString (pprop->type);
	propRet->num_vals = ListCount (pprop->values);
	propRet->vals = propValRet = (SmPropValue *) XtMalloc (
	    propRet->num_vals * sizeof (SmPropValue));

	for (pj = ListFirst (pprop->values); pj; pj = ListNext (pj))
	{
	    pval = (PropValue *) pj->thing;

	    propValRet->length = pval->length;
	    propValRet->value = (SmPointer) XtMalloc (pval->length);
	    memcpy (propValRet->value, pval->value, pval->length);

	    propValRet++;
	}

	index++;
    }

    SmsReturnProperties (smsConn, numProps, propsRet);

    if (verbose)
    {
	printf ("Client Id = %s, sent PROPERTIES REPLY [Num props = %d]\n",
		client->clientId, numProps);
    }

    for (i = 0; i < numProps; i++)
	SmFreeProperty (propsRet[i]);
    XtFree ((char *) propsRet);
}
