// Messaging_Policy_i.i,v 1.10 2000/11/01 16:45:54 brunsch Exp

#if (TAO_HAS_SYNC_SCOPE_POLICY == 1)

ACE_INLINE void
TAO_Sync_Scope_Policy::get_synchronization (Messaging::SyncScope &s) const
{
  s = this->synchronization_;
}

#endif /* TAO_HAS_SYNC_SCOPE_POLICY == 1 */
