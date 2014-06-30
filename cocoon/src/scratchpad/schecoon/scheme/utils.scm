;; utils.scm
;;
;; Auxiliary functions.
;;
;; Author: Ovidiu Predescu <ovidiu@cup.hp.com>
;; Date: January 24, 2002
;;

;; Filter a list through a predicate; only those elements of the list
;; for whom the predicate is true are returned in resulting list.
(define (filter pred lst)
  (define (filter-helper lst acc)
    (cond ((null? lst) acc)
	  (else
	   (let ((elem (car lst)))
	     (if (pred elem)
		 (filter-helper (cdr lst) (cons elem acc))
		 (filter-helper (cdr lst) acc))))))
  (reverse (filter-helper lst '())))

;; Handy shortcut for a C printf() like function. Use ~s instead of %s
;; and ~% instead of \n.
(define (printf . args)
  (display (apply format args)))

;; Remove the extra paranthesis within a list. E.g.
;; (flatten '(1 2 (a b (c d)) 3 4)) -> (1 2 a b c d 3 4)
(define (flatten lst)
  (define (flatten-helper lst acc)
    (if (null? lst)
	acc
	(let ((elem (car lst)))
	  (if (pair? elem)
	      (flatten-helper (cdr lst) (append (flatten-helper elem '()) acc))
	      (flatten-helper (cdr lst) (cons elem acc))))))
  (reverse (flatten-helper lst '())))
