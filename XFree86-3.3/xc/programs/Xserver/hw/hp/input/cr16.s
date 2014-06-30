; $XConsortium: cr16.s,v 1.2 95/06/15 15:25:19 dpw Exp $
        .SPACE  $TEXT$
        .SUBSPA $CODE$
        .export cr16
        .PROC
        .CALLINFO
        .ENTRY
cr16
        bv      (%rp)
	mfctl	16,%ret0
	.EXIT
        .PROCEND
