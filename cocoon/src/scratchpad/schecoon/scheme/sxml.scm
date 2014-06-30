;		XML processing in Scheme
;	    SXML to XML/HTML conversions and
;   SXSLT, XML Stylesheet Language Transformations in Scheme
;
; Author: Oleg Kiselyov <oleg@pobox.com>
; Original location: http://okmij.org/ftp/Scheme/SXmanip.scm
;                    http://okmij.org/ftp/Scheme/SXPath.scm
;
; Collected by Ovidiu Predescu <ovidiu@cup.hp.com> for Cocoon


;; First some definitions for SISC, which doesn't support
;; define-macro, which is used throught the SXML implementation.
(define-syntax assert
  (syntax-rules ()
   ((_) #t)
   ((_ x ...)
    (and (or x (error "failed assertion ~s" `x)) ...))))

(define (-- n) (- n 1))

;			XML processing in Scheme
;		     SXPath -- SXML Query Language
;
; This file is examples-stripped version of Oleg Kiselyov's 
; SXPath.scm,v 3.5 2001/01/12 23:20:35 oleg Exp oleg
; 
; Differences from the original sxpath:
;
; Function "filter" is renamed to "sxp:filter" for the sake of
; compatibility with SRFI-1
;
; 1. Criterion '*' doesn't accept COMMENT, ENTITY and NAMESPACES nodes
;  any more
;
; 2. node-typeof? may accept a list of names. A node is accepted if its
;      name is in this list
;
; 3. sxpath has an additional rewriting rule for 'or':
; '(or@ name1 ... ) will be  
; 
; 4. Criterion *data* introduced
;
;         Kirill Lisovsky 
;         lisovsky@acm.org
;
;                                 *  *  *
;
; SXPath is a query language for SXML, an instance of XML Information
; set (Infoset) in the form of s-expressions. See SSAX.scm for the
; definition of SXML and more details. SXPath is also a translation into
; Scheme of an XML Path Language, XPath:
;	http://www.w3.org/TR/xpath
; XPath and SXPath describe means of selecting a set of Infoset's items
; or their properties.
;
; To facilitate queries, XPath maps the XML Infoset into an explicit
; tree, and introduces important notions of a location path and a
; current, context node. A location path denotes a selection of a set of
; nodes relative to a context node. Any XPath tree has a distinguished,
; root node -- which serves as the context node for absolute location
; paths. Location path is recursively defined as a location step joined
; with a location path. A location step is a simple query of the
; database relative to a context node. A step may include expressions
; that further filter the selected set. Each node in the resulting set
; is used as a context node for the adjoining location path. The result
; of the step is a union of the sets returned by the latter location
; paths.
;
; The SXML representation of the XML Infoset (see SSAX.scm) is rather
; suitable for querying as it is. Bowing to the XPath specification,
; we will refer to SXML information items as 'Nodes':
; 	<Node> ::= <Element> | <attributes-coll> | <attrib>
; 		   | "text string" | <PI>
; This production can also be described as
;	<Node> ::= (name . <Nodeset>) | "text string"
; An (ordered) set of nodes is just a list of the constituent nodes:
; 	<Nodeset> ::= (<Node> ...)
; Nodesets, and Nodes other than text strings are both lists. A
; <Nodeset> however is either an empty list, or a list whose head is not
; a symbol.  A symbol at the head of a node is either an XML name (in
; which case it's a tag of an XML element), or an administrative name
; such as '@'.  This uniform list representation makes processing rather
; simple and elegant, while avoiding confusion. The multi-branch tree
; structure formed by the mutually-recursive datatypes <Node> and
; <Nodeset> lends itself well to processing by functional languages.
;
; A location path is in fact a composite query over an XPath tree or
; its branch. A singe step is a combination of a projection, selection
; or a transitive closure. Multiple steps are combined via join and
; union operations. This insight allows us to _elegantly_ implement
; XPath as a sequence of projection and filtering primitives --
; converters -- joined by _combinators_. Each converter takes a node
; and returns a nodeset which is the result of the corresponding query
; relative to that node. A converter can also be called on a set of
; nodes. In that case it returns a union of the corresponding queries over
; each node in the set. The union is easily implemented as a list
; append operation as all nodes in a SXML tree are considered
; distinct, by XPath conventions. We also preserve the order of the
; members in the union. Query combinators are high-order functions:
; they take converter(s) (which is a Node|Nodeset -> Nodeset function)
; and compose or otherwise combine them. We will be concerned with
; only relative location paths [XPath]: an absolute location path is a
; relative path applied to the root node.
;
; Similarly to XPath, SXPath defines full and abbreviated notations
; for location paths. In both cases, the abbreviated notation can be
; mechanically expanded into the full form by simple rewriting
; rules. In case of SXPath the corresponding rules are given as
; comments to a sxpath function, below. The regression test suite at
; the end of this file shows a representative sample of SXPaths in
; both notations, juxtaposed with the corresponding XPath
; expressions. Most of the samples are borrowed literally from the
; XPath specification, while the others are adjusted for our running
; example, tree1.
;
; $Id: sxml.scm,v 1.1 2002/01/17 18:09:12 ovidiu Exp $

(define (nodeset? x)
  (or (and (pair? x) (not (symbol? (car x)))) (null? x)))

;-------------------------
; Basic converters and applicators
; A converter is a function
;	type Converter = Node|Nodeset -> Nodeset
; A converter can also play a role of a predicate: in that case, if a
; converter, applied to a node or a nodeset, yields a non-empty
; nodeset, the converter-predicate is deemed satisfied. Throughout
; this file a nil nodeset is equivalent to #f in denoting a failure.


; The following function implements a 'Node test' as defined in
; Sec. 2.3 of XPath document. A node test is one of the components of a
; location step. It is also a converter-predicate in SXPath.
;
; The function node-typeof? takes a type criterion and returns
; a function, which, when applied to a node, will tell if the node satisfies
; the test.
;	node-typeof? :: Crit -> Node -> Boolean
;
; The criterion 'crit' is 
;  one of the following symbols:
;	id		- tests if the Node has the right name (id)
;	@		- tests if the Node is an <attributes-list>
;	*		- tests if the Node is an <Element>
;	*text*		- tests if the Node is a text node
;	*data*		- tests if the Node is a data node 
;                         (text, number, boolean, etc)
;	*PI*		- tests if the Node is a PI node
;	*COMMENT*	- tests if the Node is a COMMENT node
;	*ENTITY*        - tests if the Node is a ENTITY node
;	*any*		- #t for any type of Node
;  or list of symbols
;	(crit-symbol 
;           ...)	- node-typeof? will return if the Node satisfies
;                         at least one of the criterions listed.
;                         This list of 
(define (node-typeof? crit)
    (define (present? x)
      ((if (list? crit)
	 memq 
	 eq?)
       x crit))
  (lambda (node)
    (cond 
      ((pair? node)
       (or
	 (present? '*any*)
	 ; The line bleow handles id, *PI*, *COMMENT*, etc.
	 (present? (car node))
	 (and (present? '*)
	      (not (memq (car node) 
			 '(@ *PI* *COMMENT* *ENTITY* *NAMESPACES*))))
	 (and (present? '*data*) (not (list? node)))
	 ))
      ((string? node) 
       (or 
	 (present? '*any*)
	 (present? '*text*)
	 (present? '*data*)))
      (else
	(or
	  (present? '*any*)
	  (and (present? '*data*) (not (list? node))))))
    ))	


 ;; The function node-typeof? takes a type criterion and returns a function,
 ;; which, when applied to a node, will tell if the node satisfies
 ;; the test.
 ;;	node-typeof? :: Crit -> Node -> Boolean
 ;;
 ;; The criterion 'crit' is a symbol, one of the following:
 ;;	id		- tests if the Node has the right name (id)
 ;;	@		- tests if the Node is an <attributes-coll>
 ;;	*		- tests if the Node is an <Element>
 ;;	*text*		- tests if the Node is a text node
 ;;	*PI*		- tests if the Node is a PI node
 ;;	*any*		- #t for any type of Node
 ;(define (node-typeof? crit)
 ;  (lambda (node)
 ;    (case crit
 ;      ((*) (and (pair? node) 
 ;		(not (memq (car node) 
 ;			   '(@ *PI* *COMMENT* *ENTITY* *NAMESPACES*)))))
 ;      ((*any*) #t)
 ;      ((*text*) (string? node))
 ;      ((*data*) (not (list? node)))
 ;      (else
 ;	(and (pair? node) 
 ;	     ((if (list? crit)
 ;		memq 
 ;		eq?)
 ;	      (car node) crit)))
 ;      )))
 ;
 ;(define (node-typeof? crit)
 ;  (lambda (node)
 ;    (case crit
 ;      ((*) (and (pair? node) (not (memq (car node) '(@ *PI*)))))
 ;      ((*any*) #t)
 ;      ((*text*) (string? node))
 ;      (else
 ;       (and (pair? node) (eq? crit (car node))))
 ;)))


; Curried equivalence converter-predicates
(define (node-eq? other)
  (lambda (node)
    (eq? other node)))

(define (node-equal? other)
  (lambda (node)
    (equal? other node)))

; node-pos:: N -> Nodeset -> Nodeset, or
; node-pos:: N -> Converter
; Select the N'th element of a Nodeset and return as a singular Nodeset;
; Return an empty nodeset if the Nth element does not exist.
; ((node-pos 1) Nodeset) selects the node at the head of the Nodeset,
; if exists; ((node-pos 2) Nodeset) selects the Node after that, if
; exists.
; N can also be a negative number: in that case the node is picked from
; the tail of the list.
; ((node-pos -1) Nodeset) selects the last node of a non-empty nodeset;
; ((node-pos -2) Nodeset) selects the last but one node, if exists.

(define (node-pos n)
  (lambda (nodeset)
    (cond
     ((not (nodeset? nodeset)) '())
     ((null? nodeset) nodeset)
     ((eqv? n 1) (list (car nodeset)))
     ((negative? n) ((node-pos (+ n 1 (length nodeset))) nodeset))
     (else
      (assert (positive? n))
      ((node-pos (-- n)) (cdr nodeset))))))

; filter:: Converter -> Converter
; A filter applicator, which introduces a filtering context. The argument
; converter is considered a predicate, with either #f or nil result meaning
; failure.
(define (sxp:filter pred?)
  (lambda (lst)	; a nodeset or a node (will be converted to a singleton nset)
    (let loop ((lst (if (nodeset? lst) lst (list lst))) (res '()))
      (if (null? lst)
	  (reverse res)
	  (let ((pred-result (pred? (car lst))))
	    (loop (cdr lst)
		  (if (and pred-result (not (null? pred-result)))
		      (cons (car lst) res)
		      res)))))))

; take-until:: Converter -> Converter, or
; take-until:: Pred -> Node|Nodeset -> Nodeset
; Given a converter-predicate and a nodeset, apply the predicate to
; each element of the nodeset, until the predicate yields anything but #f or
; nil. Return the elements of the input nodeset that have been processed
; till that moment (that is, which fail the predicate).
; take-until is a variation of the filter above: take-until passes
; elements of an ordered input set till (but not including) the first
; element that satisfies the predicate.
; The nodeset returned by ((take-until (not pred)) nset) is a subset -- 
; to be more precise, a prefix -- of the nodeset returned by
; ((filter pred) nset)

(define (take-until pred?)
  (lambda (lst)	; a nodeset or a node (will be converted to a singleton nset)
    (let loop ((lst (if (nodeset? lst) lst (list lst))))
      (if (null? lst) lst
	  (let ((pred-result (pred? (car lst))))
	    (if (and pred-result (not (null? pred-result)))
		'()
		(cons (car lst) (loop (cdr lst)))))
	  ))))


; take-after:: Converter -> Converter, or
; take-after:: Pred -> Node|Nodeset -> Nodeset
; Given a converter-predicate and a nodeset, apply the predicate to
; each element of the nodeset, until the predicate yields anything but #f or
; nil. Return the elements of the input nodeset that have not been processed:
; that is, return the elements of the input nodeset that follow the first
; element that satisfied the predicate.
; take-after along with take-until partition an input nodeset into three
; parts: the first element that satisfies a predicate, all preceding
; elements and all following elements.

(define (take-after pred?)
  (lambda (lst)	; a nodeset or a node (will be converted to a singleton nset)
    (let loop ((lst (if (nodeset? lst) lst (list lst))))
      (if (null? lst) lst
	  (let ((pred-result (pred? (car lst))))
	    (if (and pred-result (not (null? pred-result)))
		(cdr lst)
		(loop (cdr lst))))
	  ))))

; Apply proc to each element of lst and return the list of results.
; if proc returns a nodeset, splice it into the result
;
; From another point of view, map-union is a function Converter->Converter,
; which places an argument-converter in a joining context.

(define (map-union proc lst)
  (if (null? lst) lst
      (let ((proc-res (proc (car lst))))
	((if (nodeset? proc-res) append cons)
	 proc-res (map-union proc (cdr lst))))))

; node-reverse :: Converter, or
; node-reverse:: Node|Nodeset -> Nodeset
; Reverses the order of nodes in the nodeset
; This basic converter is needed to implement a reverse document order
; (see the XPath Recommendation).
(define node-reverse 
  (lambda (node-or-nodeset)
    (if (not (nodeset? node-or-nodeset)) (list node-or-nodeset)
	(reverse node-or-nodeset))))

; node-trace:: String -> Converter
; (node-trace title) is an identity converter. In addition it prints out
; a node or nodeset it is applied to, prefixed with the 'title'.
; This converter is very useful for debugging.

(define (node-trace title)
  (lambda (node-or-nodeset)
    (display "\n-->")
    (display title)
    (display " :")
    (pretty-print node-or-nodeset)
    node-or-nodeset))


;-------------------------
; Converter combinators
;
; Combinators are higher-order functions that transmogrify a converter
; or glue a sequence of converters into a single, non-trivial
; converter. The goal is to arrive at converters that correspond to
; XPath location paths.
;
; From a different point of view, a combinator is a fixed, named
; _pattern_ of applying converters. Given below is a complete set of
; such patterns that together implement XPath location path
; specification. As it turns out, all these combinators can be built
; from a small number of basic blocks: regular functional composition,
; map-union and filter applicators, and the nodeset union.



; select-kids:: Pred -> Node -> Nodeset
; Given a Node, return an (ordered) subset its children that satisfy
; the Pred (a converter, actually)
; select-kids:: Pred -> Nodeset -> Nodeset
; The same as above, but select among children of all the nodes in
; the Nodeset
;
; More succinctly, the signature of this function is
; select-kids:: Converter -> Converter

(define (select-kids test-pred?)
  (lambda (node)		; node or node-set
    (cond 
     ((null? node) node)
     ((not (pair? node)) '())   ; No children
     ((symbol? (car node))
      ((sxp:filter test-pred?) (cdr node)))	; it's a single node
     (else (map-union (select-kids test-pred?) node)))))


; node-self:: Pred -> Node -> Nodeset, or
; node-self:: Converter -> Converter
; Similar to select-kids but apply to the Node itself rather
; than to its children. The resulting Nodeset will contain either one
; component, or will be empty (if the Node failed the Pred).
(define node-self sxp:filter)


; node-join:: [LocPath] -> Node|Nodeset -> Nodeset, or
; node-join:: [Converter] -> Converter
; join the sequence of location steps or paths as described
; in the title comments above.
(define (node-join . selectors)
  (lambda (nodeset)		; Nodeset or node
    (let loop ((nodeset nodeset) (selectors selectors))
      (if (null? selectors) nodeset
	  (loop 
	   (if (nodeset? nodeset)
	       (map-union (car selectors) nodeset)
	       ((car selectors) nodeset))
	   (cdr selectors))))))


; node-reduce:: [LocPath] -> Node|Nodeset -> Nodeset, or
; node-reduce:: [Converter] -> Converter
; A regular functional composition of converters.
; From a different point of view,
;    ((apply node-reduce converters) nodeset)
; is equivalent to
;    (foldl apply nodeset converters)
; i.e., folding, or reducing, a list of converters with the nodeset
; as a seed.
(define (node-reduce . converters)
  (lambda (nodeset)		; Nodeset or node
    (let loop ((nodeset nodeset) (converters converters))
      (if (null? converters) nodeset
	  (loop ((car converters) nodeset) (cdr converters))))))


; node-or:: [Converter] -> Converter
; This combinator applies all converters to a given node and
; produces the union of their results.
; This combinator corresponds to a union, '|' operation for XPath
; location paths.
; (define (node-or . converters)
;   (lambda (node-or-nodeset)
;     (if (null? converters) node-or-nodeset
; 	(append 
; 	 ((car converters) node-or-nodeset)
; 	 ((apply node-or (cdr converters)) node-or-nodeset)))))
; More optimal implementation follows
(define (node-or . converters)
  (lambda (node-or-nodeset)
    (let loop ((result '()) (converters converters))
      (if (null? converters) result
	  (loop (append result (or ((car converters) node-or-nodeset) '()))
		(cdr converters))))))


; node-closure:: Converter -> Converter
; Select all _descendants_ of a node that satisfy a converter-predicate.
; This combinator is similar to select-kids but applies to
; grand... children as well.
; This combinator implements the "descendant::" XPath axis
; Conceptually, this combinator can be expressed as
; (define (node-closure f)
;      (node-or
;        (select-kids f)
;	 (node-reduce (select-kids (node-typeof? '*)) (node-closure f))))
; This definition, as written, looks somewhat like a fixpoint, and it
; will run forever. It is obvious however that sooner or later
; (select-kids (node-typeof? '*)) will return an empty nodeset. At
; this point further iterations will no longer affect the result and
; can be stopped.

(define (node-closure test-pred?)	    
  (lambda (node)		; Nodeset or node
    (let loop ((parent node) (result '()))
      (if (null? parent) result
	  (loop ((select-kids (node-typeof? '*)) parent)
		(append result
			((select-kids test-pred?) parent)))
	  ))))

; node-parent:: RootNode -> Converter
; (node-parent rootnode) yields a converter that returns a parent of a
; node it is applied to. If applied to a nodeset, it returns the list
; of parents of nodes in the nodeset. The rootnode does not have
; to be the root node of the whole SXML tree -- it may be a root node
; of a branch of interest.
; Given the notation of Philip Wadler's paper on semantics of XSLT,
;  parent(x) = { y | y=subnode*(root), x=subnode(y) }
; Therefore, node-parent is not the fundamental converter: it can be
; expressed through the existing ones.  Yet node-parent is a rather
; convenient converter. It corresponds to a parent:: axis of SXPath.
; Note that the parent:: axis can be used with an attribute node as well!

(define (node-parent rootnode)
  (lambda (node)		; Nodeset or node
    (if (nodeset? node) (map-union (node-parent rootnode) node)
	(let ((pred
	       (node-or
		(node-reduce
		 (node-self (node-typeof? '*))
		 (select-kids (node-eq? node)))
		(node-join
		 (select-kids (node-typeof? '@))
		 (select-kids (node-eq? node))))))
	  ((node-or
	    (node-self pred)
	    (node-closure pred))
	   rootnode)))))

;-------------------------
; Evaluate an abbreviated SXPath
;	sxpath:: AbbrPath -> Converter, or
;	sxpath:: AbbrPath -> Node|Nodeset -> Nodeset
; AbbrPath is a list. It is translated to the full SXPath according
; to the following rewriting rules
; (sxpath '()) -> (node-join)
; (sxpath '(path-component ...)) ->
;		(node-join (sxpath1 path-component) (sxpath '(...)))
; (sxpath1 '//) -> (node-or 
;		     (node-self (node-typeof? '*any*))
;		      (node-closure (node-typeof? '*any*)))
; (sxpath1 '(equal? x)) -> (select-kids (node-equal? x))
; (sxpath1 '(eq? x))    -> (select-kids (node-eq? x))
; (sxpath1 '(or@ ...))  -> (select-kids (node-typeof?
;                                          (cdr '(or@ ...))))
; (sxpath1 ?symbol)     -> (select-kids (node-typeof? ?symbol))
; (sxpath1 procedure)   -> procedure
; (sxpath1 '(?symbol ...)) -> (sxpath1 '((?symbol) ...))
; (sxpath1 '(path reducer ...)) ->
;		(node-reduce (sxpath path) (sxpathr reducer) ...)
; (sxpathr number)      -> (node-pos number)
; (sxpathr path-filter) -> (filter (sxpath path-filter))

(define (sxpath path)
  (lambda (nodeset)
    (let loop ((nodeset nodeset) (path path))
    (cond
     ((null? path) nodeset)
     ; (or@ ... ) handler
     ((and (list? (car path)) 
	   (not (null? (car path)))
	   (eq? 'or@ (caar path)))
      (loop ((select-kids (node-typeof? (cdar path))) nodeset)
	    (cdr path)))
     ((nodeset? nodeset)
      (map-union (sxpath path) nodeset))
     ((procedure? (car path))
      (loop ((car path) nodeset) (cdr path)))
     ((eq? '// (car path))
      (loop
       ((if (nodeset? nodeset) append cons) nodeset
	((node-closure (node-typeof? '*any*)) nodeset))
       (cdr path)))
     ((symbol? (car path))
      (loop ((select-kids (node-typeof? (car path))) nodeset)
	    (cdr path)))
     ((and (pair? (car path)) (eq? 'equal? (caar path)))
      (loop ((select-kids (apply node-equal? (cdar path))) nodeset)
	    (cdr path)))
     ((and (pair? (car path)) (eq? 'eq? (caar path)))
      (loop ((select-kids (apply node-eq? (cdar path))) nodeset)
	    (cdr path)))
     ((pair? (car path))
      (let reducer ((nodeset 
		     (if (symbol? (caar path))
			 ((select-kids (node-typeof? (caar path))) nodeset)
			 (loop nodeset (caar path))))
		    (reducing-path (cdar path)))
	(cond
	 ((null? reducing-path) (loop nodeset (cdr path)))
	 ((number? (car reducing-path))
	  (reducer ((node-pos (car reducing-path)) nodeset)
		   (cdr reducing-path)))
	 (else
	  (reducer ((sxp:filter (sxpath (car reducing-path))) nodeset)
		   (cdr reducing-path))))))
     (else
      (error "Invalid path step: " (car path)))
))))


; Pre-order traversal of a tree and creation of a new tree:
;	apply-templates:: tree x <templates> -> <new-tree>
; where
; <templates> ::= (<template> ...)
; <template>  ::= (<node-test> <node-test> ... <node-test> . <handler>)
; <node-test> ::= an argument to node-typeof? above
; <handler>   ::= <tree> -> <new-tree>
;
; This procedure does a _normal_, pre-order traversal of an SXML
; tree.  It walks the tree, checking at each node against the list of
; matching templates.
; If the match is found (which must be unique, i.e., unambiguous), the
; corresponding handler is invoked and given the current node as an
; argument.  The result from the handler, which must be a <tree>,
; takes place of the current node in the resulting tree.
; The name of the function is not accidental: it resembles rather closely
; an 'apply-templates' function of XSLT.

(define (apply-templates tree templates)

		; Filter the list of templates. If a template does not
		; contradict the given node (that is, its head matches
		; the type of the node), chop off the head and keep the
		; rest as the result. All contradicting templates are removed.
  (define (filter-templates node templates)
    (cond
     ((null? templates) templates)
     ((not (pair? (car templates)))  ; A good template must be a list
      (filter-templates node (cdr templates)))
     (((node-typeof? (caar templates)) node)
      (cons (cdar templates) (filter-templates node (cdr templates))))
     (else
      (filter-templates node (cdr templates)))))

		; Here <templates> ::= [<template> | <handler>]
		; If there is a <handler> in the above list, it must
		; be only one. If found, return it; otherwise, return #f
  (define (find-handler templates)
    (and (pair? templates)
	 (cond
	  ((procedure? (car templates))
	   (if (find-handler (cdr templates))
	       (error "ambiguous template match"))
	   (car templates))
	  (else (find-handler (cdr templates))))))

  (let loop ((tree tree) (active-templates '()))
   ;(cout "active-templates: " active-templates nl "tree: " tree nl)
    (if (nodeset? tree)
	(map-union (lambda (a-tree) (loop a-tree active-templates)) tree)
	(let ((still-active-templates 
	       (append 
		(filter-templates tree active-templates)
		(filter-templates tree templates))))
	  (cond 
	   ;((null? still-active-templates) '())
	   ((find-handler still-active-templates) =>
	    (lambda (handler) (handler tree)))
	   ((not (pair? tree)) '())
	   (else
	    (loop (cdr tree) still-active-templates))))))
)

;;                                               
;;                            S X M L   T o o l s
;;                                               
;;                             Swiss Army Knife  
;;                              for SXML data    
;;                                               

;; Some of the functions are dealing with normalized
;; or minimized form of SXML data.
;; Normalized form is defined in SXML Specification.
;; Minimized form is just the same, but doesn't require the presence 
;; of empty attribute lists.

;; $Revision: 1.1 $ from $Date: 2002/01/17 18:09:12 $:

;; Copyright (c) 2000-2001 Kirill Lisovsky
;;
;; This software is free,
;; MIT-style license is attached in the file COPYRIGHTS

;==============================================================================
; Predicates

; Predicate which returns #t if <obj> is SXML node, otherwise returns #f. 
(define (sxml:node? obj)	
  (and                            ; The object is SXML node if:
       (list? obj)                ; - it is a list and
       (not (null? obj))          ; - it is not empty
       (symbol? (car obj))        ; - its car is symbol
       ))

; Predicate which returns #t if <obj> is SXML element, otherwise returns #f. 
(define (sxml:element? obj)	
  (and                            ; The object is SXML element if:
       (list? obj)                ; - it is a list and
       (not (null? obj))          ; - it is not empty
       (symbol? (car obj))        ; - its car is symbol (name)
       (not (member (car obj) 
	     '(@                  ; - it is not a list of attributes, etc.
		*TOP* 		  
		*PI*
		*COMMENT* 
		*ENTITY*
		*NAMESPACES*)))))

; Predicate which returns #t if <obj> is SXML element, otherwise returns #f. 
(define (sxml:attr-list? obj)	
  (and                            ; The object is SXML element if:
       (list? obj)                ; - it is a list and
       (not (null? obj))          ; - it is not empty
       (symbol? (car obj))        ; - its car is symbol (name)
       (eq? (car obj) '@)))       ; - it is a list of attributes, etc.


; Predicate which returns #t if given element <obj> is empty. 
; Empty element has no content but may contain some attributes.
; It is a counterpart of XML empty-element.
(define (sxml:empty-element? obj)
  (and (sxml:element? obj)
       (null? (sxml:content obj))))

; Predicate which returns #t if given element <obj> is void.
; Void element has neither content nor attributes, namespaces or comments,
; So, the every void element is also the empty one.
(define (sxml:void-element? obj)
  (and 
    (sxml:element? obj)
    (or 
      (null? (cdr obj)) 
      (and 
	(eq? '@ (cadr obj)) 
	(null? (cdadr obj)) 
	(null? (cddr obj))))))


; Returns #t if the given <obj> is shallow-normalized SXML element.
;  This means that this element itself has a list of attributes
;  in the second position, but it doesn't  test its nested elements.
(define (sxml:shallow-normalized? obj)
  (and 
    (sxml:element? obj)
    (not 
      (null? (cdr obj))) 
    (list? (cadr obj)) 
    (eq? (car (cadr obj)) 
	 '@)))


;==============================================================================
; Accessors  

; Returns a name of the given element
; Sample call: (sxml:name element)
; It is introduced for the sake of encapsulation.
(define sxml:name car)

; Safe version of sxml:name, which returns #f if the given <obj> is 
; not a SXML element.
; Otherwise returns its name.
(define (sxml:element-name obj)
  (if (sxml:element? obj)
    (car obj)
    #f))

; Safe version of sxml:name, which returns #f if the given <obj> is 
; not a SXML node.
; Otherwise returns its name.
(define (sxml:node-name obj)
  (if (sxml:node? obj)
    (car obj)
    #f))

; Returns the content of given SXML element, representing it
; as a list of values and nested elements in document (depth-first) order. 
; This list is empty for an empty element.
; <obj> is minimized or normalized SXML element.
(define (sxml:content obj)
  ((select-kids (node-typeof-list? '(* *text*))) obj))
  

; Returns the list whose car is the list of attributes for given <obj>
; and whose cdr is the content of given <obj>
; <obj> is minimized or normalized SXML element
(define (sxml:attr-content obj)
  (let rpt ((cl (cdr obj)) (cn '()))
    (if (null? cl)
       (cons '(@) cn)
      ((lambda (h)
	 (if (and (list? h)
		  (eq? (car h) '@))
	   (cons h (append cn (cdr cl)))
	   (rpt (cdr cl) (append cn (list h))))) 
       (car cl)))))

; Returns a list of all the children elements for the given <obj>,
; <obj> which have to be minimized or normalized SXML element
(define (sxml:child-elements obj)
  (let rpt ((ns obj)
	    (rez '()))
    (if (null? ns)
      rez
      (rpt (cdr ns) 
	   (if (sxml:element? (car ns))
	     (cons (car ns) rez) 
	     rez)))))
	   
; Returns the list of attributes for the given element.
; Analog of ((sxpath '(@ *)) obj)
; Empty list is returned if there is no attributes for the element.
(define (sxml:attr-list obj)
  (let rpt ((cl (cdr obj)))
    (if (null? cl)
      '()
	 (if (and (pair? (car cl))
		  (eq? (caar cl) '@))
	   (cdar cl)
	   (rpt (cdr cl)))) 
       ))


; Accessor for the attribute named <attr-name> for the given SXML element
; <obj> which may also be an  attributes-list
;
; Analog of ((if-car-sxpath '(@ attr-name *text*)) obj)
;
; It returns: 
;    the value of the attribute if the attribute is present and has a value
;    #f if there is no such an attribute in the given element
;    attribute name (as a string) if the attribute is singular (boolean)
(define (sxml:attr obj attr-name)
  (cond 
    ((assv attr-name 
	   (cond
	     ((and (not (null? (cdr obj))) 
		   (list? (cadr obj))
		   (eq? '@ (caadr obj)))
	      (cdadr obj))   ; fast track for normalized elements 
	     ((eq? '@ (car obj))
	     (cdr obj))
	     (else
	     (sxml:attr-list obj)))) => 
					   (lambda(x)
					     (if (null? (cdr x))
					       (symbol->string attr-name)
					       (cadr x))))
    (else #f)))

;==============================================================================
; Data modification functions
;
; Constructors and mutators

; if the <new-content> is an empty list then the <obj> is transformed 
; to an empty element
; The resulting SXML element is normalized
(define (sxml:content! obj new-content)
  (set-cdr! obj (cons 
			 (cons '@ (sxml:attr-list obj)) 
			     new-content))
  obj)

; Ground function for attribute manipulations
; The resulting SXML element is normalized, if <new-attrlist> is empty,
; the cadr of <obj> is (@)
(define (sxml:attrlist! obj new-attrlist)
       (set-cdr! obj 
		 (cons
			(cons '@ new-attrlist) 
			(sxml:content obj)))
       obj) 
       
; Change name of a SXML element
(define (sxml:name! obj new-name)
  (set-car! obj new-name)
  obj)

; Add an attribute <attr-name> with a value <attr-value> for an element 
; <obj>
; Returns #f if the attribute with given name already exists, 
; or the modified SXML node in case of success
; The resulting SXML node is normalized
(define (sxml:add-attr! obj attr-name attr-value)
  (let ((x (sxml:attr-list obj)))
    (cond
      ((null? x)
       (set-cdr! obj (cons 
			(list '@ (list  attr-name attr-value)) 
			(sxml:content obj)))
       obj)
      ((assv attr-name x)
       #f)
      (else
	(set-car! (cdr obj) 
		  (append (list '@ (list attr-name attr-value)) x))
	obj))))

; Set value of the attribute <attr-name> of element <obj> to <attr-value>
; Returns #f if there is no such attribute, or the modified SXML element 
; in case of success
; The resulting SXML element is normalized
(define (sxml:set-attr! obj attr-name attr-value)
  (let ((x (sxml:attr-list obj)))
     (if (null? x)
       #f
       (cond 
	 ((assv attr-name x) => (lambda (y)
				  (set-car! (cdr y) attr-value) obj))
	 (else #f)))))

; Set the value of the attribute <attr-name> of element <obj> to <attr-value>
; If there is no such attribute the new one is added
; Returns: the SXML element modified 
(define (sxml:attr! obj attr-name attr-value)
  ((lambda (x)
     (if (null? x)
       (set-cdr! obj (cons 
			     (list '@ (list attr-name attr-value)) 
			     (sxml:content obj)))
       (cond 
	 ((assv attr-name x) => (lambda (y)
				  (set-car! (cdr y) attr-value)))
	 (else (set-car! (cdr obj) 
			 (append (list '@ (list attr-name attr-value)) 
				x)))))
     obj)
   (sxml:attr-list obj)))


; Appends <new-data> to the content of the <obj>
; The resulting SXML element is normalized
(define (sxml:append-content! obj . new-data)
   (sxml:content! obj
 		     (append (sxml:content obj) 
 			     new-data))
   obj)

; Appends <new-data> to the content of the <obj>
; The resulting SXML element is normalized
(define (sxml:append-content1! obj new-data)
  (sxml:content! obj
		     (append (sxml:content obj) 
			     new-data))
  obj)

; This function inserts new sub-elements in sxml node <obj>
; if there is no elements with the same name yet
; <element> is a sxml element to insert
;
; Absolete name: if-insert-node!
(define (sxml:insert-unique! obj element)
  (cond
    ((null? ((sxpath `(,(sxml:name element))) obj))
	    (sxml:append-content! obj element))
    (else #f)))

; Normalize SXML data structure for the given <obj> and its descendants
; Returns: normalized SXML element
(define (sxml:normalize! obj)
  (if (not (sxml:shallow-normalized? obj))
    ((lambda(x) 
       (if (null? x)
          (set-cdr! obj (cons '(@) (sxml:content obj)))
	  (set-cdr! obj (cons (cons '@ x) (sxml:content obj)))
	  ))
     (sxml:attr-list obj)))
  (for-each
    sxml:normalize!
    (sxml:child-elements obj))
   obj)

; Eliminates empty lists of attributes for the given SXML element 
; <obj> and its descendants ("minimize" it)
; Returns: minimized SXML element
(define (sxml:squeeze! obj)
  (if (null? (sxml:attr-list obj))
    (set-cdr! obj (sxml:content obj)))
    (for-each 
      sxml:squeeze!
	(sxml:child-elements obj))
    obj)


;==============================================================================
; SXPath-related 

;------------------------------------------------------------------------------
; extensions

; The function node-typeof-list? takes a type criterion and returns
; a function, which, when applied to a node, will tell if the node satisfies
; the test.
;	node-typeof? :: Crit -> Node -> Boolean
;
; The criterion 'crit' is 
;  one of the following symbols:
;	id		- tests if the Node has the right name (id)
;	@		- tests if the Node is an <attributes-coll>
;	*		- tests if the Node is an <Element>
;	*text*		- tests if the Node is a text node
;	*data*		- tests if the Node is a data node 
;                         (text, number, boolean, etc)
;	*PI*		- tests if the Node is a PI node
;	*any*		- #t for any type of Node
;  or list of symbols
;	(id ...)	- tests if the Node has the right name, 
;                         listed in (id ...) list
(define (node-typeof-list? crit)
    (define (present? x)
      ((if (list? crit)
	 memq 
	 eq?)
       x crit))
  (lambda (node)
    (cond 
      ((pair? node)
       (or
	 (present? '*any*)
	 (present? (car node))
	 (and (present? '*)
	      (not (memq (car node) 
			 '(@ *PI* *COMMENT* *ENTITY* *NAMESPACES*))))
	 (and (present? '*data*) (not (list? node)))
	 ))
      ((string? node) 
       (or 
	 (present? '*any*)
	 (present? '*text*)
	 (present? '*data*)))
      (else
	(or
	  (present? '*any*)
	  (and (present? '*data*) (not (list? node))))))
    ))	
	   
