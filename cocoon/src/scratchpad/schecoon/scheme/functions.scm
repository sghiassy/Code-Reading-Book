;; functions.scm
;;
;; Support for functions with named arguments.
;;
;; Author: Ovidiu Predescu <ovidiu@cup.hp.com>
;; Date: January 24, 2002
;;

(define-syntax function
  (lambda (x)
    (syntax-case x ()
     ((_ (name sitemap env arg ...) body ...)
      (syntax
       (define (name sitemap env params)
	 (let ((arg (extract-param params 'arg))
	       ...)
	   body ...)))))))

(define (extract-param params arg)
  (if (pair? params)
      (let ((r (assoc (symbol->string arg) params)))
	(if (pair? r)
	    (cdr r)
	    '()))
      '()))

(define (function-call func sitemap env args)
  (let ((params (cdr (assq 'params args))))
    (apply func sitemap env (list params))))

(define (function-call-v sitemap env args)
  (let ((funname (cdr (assq 'function args)))
	(params (cdr (assq 'params args))))
    (eval `(,(string->symbol funname) ,sitemap ,env (quote ,params))
	  (interaction-environment))))

(define (resource-call-v sitemap env args)
  (let ((funname (name->resource (cdr (assq 'resource args))))
	(params (cdr (assq 'params args))))
    (printf "resource-call-v: funname ~s, params ~s~%" funname params)
    (eval `(,(string->symbol funname) ,sitemap ,env (quote ,params))
	  (interaction-environment))))

(function (my-function sitemap env a b)
 (display (format "a = ~s, b = ~s~%" a b)))

(function-call-v 1 2 '((function . "my-function")
		       (params . (("a" . "3") ("b" . "4")))))

(function-call my-function 1 2 '((params . (("a" . "3") ("b" . "4")))))

(my-function 's 'e '(("a" . "1") ("b" . "2")))

(define test-sitemap
 '(sitemap (@ (*line* 3))
   (resources (@ (*line* 5))
    (resource (@ (name "document2html") (*line* 7))
       (generate (@ (src "{source}") (type "file") (*line* 8)))
       (transform (@ (src "stylesheets/document2html.xsl") (*line* 9)))
       (serialize (@ (*line* 10)))
       ))

   (pipelines (@ (*line* 15))
     (pipeline (@ (*line* 16))
      (match (@ (pattern "a") (*line* 18))
       (generate (@ (src "docs/{1}.xml") (type "file") (*line* 19)))
       (transform (@ (src "stylesheets/document2html.xsl") (*line* 20)))
       (serialize (@ (*line* 21)))
       )
      
      (match (@ (pattern "sites/images/(.*).gif") (*line* 23))
	(read (@ (src "{1}") (mime-type "image/gif") (*line* 24))))
    ))))

((sxpath '(resources (resource (@ (equal? (name "document2html"))))))
 test-sitemap)

((sxpath '(resources resource @ (*))) test-sitemap)
