// Supplier.cpp,v 1.2 2000/04/25 17:38:13 coryan Exp

#include "Supplier.h"
#include "orbsvcs/RtecEventChannelAdminS.h"
#include "orbsvcs/Event_Service_Constants.h"

ACE_RCSID(EC_Examples, Supplier, "Supplier.cpp,v 1.2 2000/04/25 17:38:13 coryan Exp")

Supplier::Supplier (void)
{
}

void
Supplier::connect (RtecEventChannelAdmin::SupplierAdmin_ptr supplier_admin,
                   CORBA::Environment &ACE_TRY_ENV)
{
  this->proxy_ =
    supplier_admin->obtain_push_consumer (ACE_TRY_ENV);
  ACE_CHECK;

  RtecEventComm::PushSupplier_var me =
    this->_this (ACE_TRY_ENV);
  ACE_CHECK;

  // Simple publication, but usually the helper classes in
  // $TAO_ROOT/orbsvcs/Event_Utils.h are a better way to do this.
  RtecEventChannelAdmin::SupplierQOS qos;
  qos.is_gateway = 0;

  qos.publications.length (1);
  RtecEventComm::EventHeader& h0 =
    qos.publications[0].event.header;
  h0.type   = ACE_ES_EVENT_UNDEFINED; // first free event type
  h0.source = 1;                      // first free event source

  this->proxy_->connect_push_supplier (me.in (), qos,
                                       ACE_TRY_ENV);
  ACE_CHECK;
}

void
Supplier::disconnect (CORBA::Environment &ACE_TRY_ENV)
{
  // Disconnect from the EC
  ACE_TRY
    {
      this->proxy_->disconnect_push_consumer (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
    }
  ACE_ENDTRY;

  PortableServer::POA_var poa =
    this->_default_POA (ACE_TRY_ENV);
  ACE_CHECK;
  PortableServer::ObjectId_var id =
    poa->servant_to_id (this, ACE_TRY_ENV);
  ACE_CHECK;
  poa->deactivate_object (id.in (), ACE_TRY_ENV);
  ACE_CHECK;
}

void
Supplier::perform_push (CORBA::Environment &ACE_TRY_ENV)
{
  ACE_TRY
    {
      // The event type and source must match our publications
      RtecEventComm::EventSet event (1);
      event.length (1);
      event[0].header.type   = ACE_ES_EVENT_UNDEFINED;
      event[0].header.source = 1;
      // Avoid loops throught the event channel federations
      event[0].header.ttl    = 1;

      this->proxy_->push (event, ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
    }
  ACE_ENDTRY;
}

void
Supplier::disconnect_push_supplier (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
}

// ****************************************************************

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
#elif defined(ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
