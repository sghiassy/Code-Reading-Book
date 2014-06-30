// Buffering_Constraint_Policy.i,v 1.5 2000/04/29 21:08:22 coryan Exp

ACE_INLINE void
TAO_Buffering_Constraint_Policy::get_buffering_constraint (TAO::BufferingConstraint &bc) const
{
  bc = this->buffering_constraint_;
}
