;; Cocoon sitemap translator
;;
;; Author: Ovidiu Predescu <ovidiu@cup.hp.com>
;; Date: December 12, 2001
;;

(load-module "sisc.modules.Regexp")
(load-module "sisc.modules.J2S")
(load-module "org.apache.cocoon.scheme.sitemap.SchemeSitemapFunctions")

;; A Cocoon XML sitemap description is processed by the Scheme code in
;; this file. The processing happens in several steps:
;;
;; - in the first step, the XML sitemap file is translated into an
;; SXML representation. SXML is the Scheme representation of an XML
;; tree. This translation happens in an external Java class which uses
;; Cocoon's XML parser to do the parsing.
;;
;; - in the second step, the SXML representation of the sitemap is
;; translated into Scheme code, which is evaluated. This evaluation
;; process defines a Scheme function, which is invoked at runtime to
;; process the HTTP request.
;;
;; Here is a sample example. Suppose we have the following XML sitemap:
;;
;; <pipelines>
;;  <pipeline>
;;   <match pattern="documentation/(.*).html">
;;    <generate src="docs/{1}.xml" type="file">
;;     <param name="test" value="123"/>
;;    </generate>
;;    <transform src="stylesheets/document2html.xsl">
;;     <param name="test2" value="456"/>
;;    </transform>
;;    <serialize/>
;;   </match>
;;
;;   <match pattern="sites/images/(.*).gif">
;;    <read src="{1}" mime-type="image/gif"/>
;;   </match>
;;  </pipeline>
;; </pipelines>
;;
;; The SXML representation of the above XML fragment looks like this:
;;
;; (pipelines (@ (*line* 1))
;;  (pipeline (@ (*line* 2))
;;   (match (@ (pattern "documentation/(.*).html") (*line* 3))
;;          (generate (@ (src "docs/{1}.xml") (type "file") (*line* 4))
;;                    (param (@ (name "test") (value "123") (*line* 5)))
;;          (transform (@ (src "stylesheets/document2html.xsl") (@ (*line* 6)))
;;                     (param (@ (name "test2") (value "456") (@ (*line* 7))))
;;          (serialize (@ (*line* 8)))
;;          )
;;
;;   (match (@ (pattern "sites/images/(.*).gif") (*line* 10))
;;          (read (@ (src "{1}") (mime-type "image/gif") (@ (*line* 11)))))
;;   )))
;;
;; The line numbers where an element starts are added by the XML
;; parser to the SXML representation as attributes of the element.
;;
;; The Scheme code translates the above SXML representation in the
;; following code. [The code below actually does some optimizations to
;; eliminate as many `let' forms as possible. This speeds up a bit the
;; compilation process, but the code is semantically the same.]
;;
;;(define the-sitemap
;;  (let ((rx1 (regexp "documentation/(.*).html"))
;;        (rx2 (regexp "sites/images/(.*).gif")))
;;    (define (p1 url sitemap env)
;;      (let ((result (regexp-match rx1 url)))
;;        (if result
;;          (apply
;;           (lambda (arg1 . rest)
;;             (sitemap:process
;;              sitemap env '()
;;              (sitemap:serialize
;;               sitemap env '()
;;               (sitemap:transform
;;                sitemap env
;;                (list (cons 'params (list (cons "test" "123")
;;                                          (cons "test2" "456")))
;;                      (cons 'src "stylesheets/document2html.xsl"))
;;                (sitemap:generate
;;                 sitemap env
;;                 (list (cons 'params (list (cons "test" "123")
;;                                           (cons "test2" "456")))
;;                       (cons 'src (string-append "docs/" arg1 ".xml"))
;;                       (cons 'type "file")))))))
;;           (cdr result))
;;          #f)))
;;
;;    (define (p2 url sitemap env)
;;      (let ((result (regexp-match rx2 url)))
;;      (if result
;;          (apply (lambda (arg1 . rest)
;;                   (sitemap:process
;;                    sitemap env '()
;;                    (sitemap:read
;;                     sitemap env
;;                     '(("src" . (string-append "" arg1 ""))
;;                       ("mime-type" . "image/gif")))))
;;                 (cdr result))
;;          #f)))
;;
;;    (lambda (url sitemap env)
;;      (or (p1 url sitemap env)
;;        (p2 url sitemap env)))))
;;
;;
;; Notice that all the {1}, {2}, ...{n} get expanded in the body of
;; the generated function. Thus there is no need to do a runtime
;; replacement of the {n} arguments in the URL string.
;;
;; The effect is that `the-sitemap' will be bound to a Scheme function
;; which, when executed, will process the HTTP request as described in
;; the original XML sitemap.

;; Converts a name to a resource. `name' is either a string or a
;; symbol. The return value is r_<name> and is of the same type as
;; `name'.
(define (name->resource name)
  (cond ((string? name) (string-append "r_" name))
	((symbol? name) (name->resource (symbol->string name)))
	(else #f)))

;; The main function to process an SXML representation of the sitemap,
;; and generate a function which is the executable version of the
;; sitemap.
;;
;; process-sitemap:: SXML -> (URL Sitemap Env -> #<void>)
;;
;; This returned function should be invoked at runtime to process an
;; HTTP request.
;;
;; The side effect of executing this returned function is the
;; processing of the input HTTP request as defined by the sitemap.
;;
(define (process-sitemap sitemap)
  (let ((exit #f)
	;; Regular expressions for matching various types of arguments
	(res-arg-regexp (regexp "/({[^0-9].*})/"))
	(res-arg-regexp-match (regexp "{[^0-9].*}"))
        (num-arg-regexp (regexp "/({[0-9]+})/"))
        (arg-regexp-split (regexp "[{}]"))
        (match-pattern-regexp (regexp "/(\\([^)]+\\))/"))

	;; The number of paranthesised groups in the current pattern
	;; being analyzed by match-match.
        (pattern-regexps-no 0)

	;; The number of pipelines analyzed so far
        (pcount 0)

	;; Identifies all the <map:resource name="..."> elements. It
	;; is a list of pairs, with the car being the function
	;; signature, and the cons being the function body definition:
	;;
	;; ((function-name arguments ...) . function-definition-code)
	;;
	;; This is used in the translating the calls to the pipeline
	;; resources defined.
	(resources '()))

    ;; Search for a resource whose name is `name'. Returns the cons
    ;; entry in the `resources' store, which holds the function
    ;; signature and the function body of the resource. #f is returned
    ;; if no resource `name' is found.
    (define (lookup-resource name)
      (let loop ((resources resources))
	(if (null? resources)
	    #f
	    (let ((elem (caar resources)))
	      (if (eq? elem name)
		  (car resources)
		  (loop (cdr resources)))))))

    ;; Adds a new resource to the available resources. No check is
    ;; done to see if a similarly named resource exists.
    (define (add-resource funsig funbody)
      (set! resources (cons (cons funsig funbody) resources)))

    ;; Return the signature of the resource named `name' or #f if no
    ;; such resource exits.
    (define (resource-function-signature name)
      (let ((resource (lookup-resource name)))
	(if resource
	    (car resource)
	    #f)))

    ;; Return the body of the resource named `name' or #f if no such
    ;; resource exists.
    (define (resource-function-body name)
      (let ((resource (lookup-resource name)))
	(if resource
	    (cdr resource)
	    #f)))

    ;; Return a list of all the function bodies.
    (define (resources-get-function-bodies)
      (map (lambda (res) (cdr res)) resources))

    ;; Print out an error message, showing the line in the XML document
    ;; where the error occured, if such information is present in the
    ;; SXML tree.
    (define (xml-error node message)
      (let ((line (sxml:attr node '*line*)))
        (if line
            (begin (display "In line ") (display line) (display ": ")))
        (display message) (newline) (exit 'error)))

    ;; Returns the remaining pipeline after the first element has been
    ;; removed.
    (define (rest-of-nodes nodelist)
      ((take-after (lambda (node) #t)) nodelist))

    ;; Takes a string value and replaces in it all occurrences of
    ;; '{n}', where 'n' is required to be either a number or a
    ;; name. If `n' is a number, `{n} will be replace with `argN',
    ;; which stands for the function argument with the same name. If
    ;; `n' is a name, `{n}' is replaced with `n', which also stands
    ;; for the function argument with the same name. If such an
    ;; occurrence is found, the value returned is an expression of
    ;; this form:
    ;;
    ;; "...{n}..." -> (string-append "..." argN "...")
    ;;
    ;; or
    ;;
    ;; "...{n}..." -> (string-append "..." n "...")
    ;;
    ;; If no such occurrence is found, the value is simply returned.
    (define (expand-value node value args-are-numbers?)
      (let* ((rx (if args-are-numbers? num-arg-regexp res-arg-regexp))
	     (exp (filter (lambda (x) (if (equal? x "") #f x))
			  (vector->list (regexp-split rx value))))
	     (exp2
	      (reverse
	       (let loop ((lst exp) (acc '()))
		 (if (null? lst)
		     acc
		     (let* ((arg (car lst))
			    (split-arg (regexp-split/delimiter
					arg-regexp-split arg))
			    (n (if (> (vector-length split-arg) 1)
				   (vector-ref split-arg 1)
				   #f)))
		       (if n
			   ;; If we are looking for argument numbers,
			   ;; verify than `n' is greater than the
			   ;; maximum number of paranthesised
			   ;; expressions in the original pattern.
			   (if args-are-numbers?
			       (begin
				 (if (> (string->number n) pattern-regexps-no)
				     (xml-error node (format "Reference to inexistent regexp pattern ~a, maximum allowed is ~s" n pattern-regexps-no)))
				 (if (< (string->number n) 1)
				     (xml-error node (format "Regexp pattern argument should be greater than 1, got ~a" n)))
				 (set! arg (string->symbol (string-append "arg" n))))
			       ;; If the argument is not a number, we
			       ;; want to convert it to a symbol, so
			       ;; that in the generated code
			       ;; expression it will refer to the
			       ;; function argument with the same
			       ;; name.
			       (set! arg (string->symbol n))))
		       (loop (cdr lst) (cons arg acc)))))
		 )))
	(if (= (length exp2) 1)
	    (car exp2)
	    `(string-append ,@exp2))))

    ;; Collect embedded <param> elements into a list of name/value
    ;; pairs and return it.
    (define (get-params elements args-are-numbers?)
      (if (eq? elements '())
          '()
          (let* ((nodelist ((node-pos 1) elements))
                 (node (if (null? nodelist) '() (car nodelist)))
                 (name (sxml:attr node 'name))
                 (value (sxml:attr node 'value)))
            (if (null? name)
                (xml-error node "Attribute 'name' is required in <param>"))
            (if (null? value)
                (xml-error node "Attribute 'value' is required in <param>"))
            (cons `(cons ,name ,(expand-value node value args-are-numbers?))
		  (get-params (rest-of-nodes elements) args-are-numbers?)))
            ))

    ;; Obtain the list of required and optional arguments, as well as
    ;; the parameters, if they are needed.
    (define (get-attributes node required optional allow-params args-are-numbers?)
      (let* ((elem-name (sxml:element-name node))
             (args '())
             (params '())
             (required-attrs
              (map
               (lambda (attr-name)
                 (let ((attr (sxml:attr node attr-name)))
                   (if (not attr)
                       (xml-error node
                                  (format "'~s' attribute required in <~s>"
                                          attr-name elem-name))
                       `(cons ',attr-name
			      ,(expand-value node attr args-are-numbers?)))))
               required))
             (optional-attrs '()))
        (for-each
         (lambda (attr-name)
           (let ((attr (sxml:attr node attr-name)))
             (if attr
                 (set! optional-attrs
                       (cons
			`(cons ',attr-name
			       ,(expand-value node attr args-are-numbers?))
			optional-attrs)))))
         optional)

        (if (not (null? required-attrs))
            (set! args (append args required-attrs)))
        (if (not (null? optional-attrs))
            (set! args (append args optional-attrs)))
        (if allow-params
            (begin
              (set! params (get-params (sxml:content node) args-are-numbers?))
              (if (not (null? params))
                  (set! args (cons `(cons 'params (list ,@params)) args)))))
        (if (null? args) `('()) `((list ,@args)))))

    ;; The following match- functions should probably be transformed
    ;; into a macro, and described at a much higher level than now. A
    ;; grammar like approach seems appropriate here.

    ;; Translate a <generate> element.
    (define (match-generate pipeline args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (if (not (eq? (sxml:element-name node) 'generate))
            #f
            (begin
              (let ((args (get-attributes node '(src) '(type) #t
					  args-are-numbers?)))
                (match-transform
                 (rest-of-nodes pipeline)
                 `(sitemap:generate sitemap env ,@args)
		 args-are-numbers?)
                )))))

    ;; Translate zero or more <transform> elements
    (define (match-transform pipeline compfunc args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (cond
         ((eq? (sxml:element-name node) 'transform)
          (let ((args (get-attributes node '(src) '(type) #t
				      args-are-numbers?)))
            (match-transform
             (rest-of-nodes pipeline)
             `(sitemap:transform sitemap env ,@args ,compfunc)
	     args-are-numbers?)
            ))
        (else (match-serialize pipeline compfunc args-are-numbers?))
        )))

    ;; Transform zero or one <serializer> elements
    (define (match-serialize pipeline compfunc args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (cond
         ;; A serializer has been explicitly defined
         ((eq? (sxml:element-name node) 'serialize)
          (let ((args (get-attributes node '() '(type mime-type) #t
				      args-are-numbers?)))
            (match-pipeline-end
             (rest-of-nodes pipeline)
             `(sitemap:serialize sitemap env ,@args ,compfunc))))

         ;; End of the pipeline with no serializer specified
         ((eq? node '())
          `(sitemap:serialize sitemap env ,compfunc))

         ;; Anything else is an error
         (else
          (xml-error node "Only <transformer> or <serialize> allowed here"))
         )))

    ;; Translate a <read> element
    (define (match-reader pipeline args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (cond
         ((eq? (sxml:element-name node) 'read)
          (let ((args (get-attributes node '(src) '(type mime-type) #t
				      args-are-numbers?)))
            (match-pipeline-end
             (rest-of-nodes pipeline)
             `(sitemap:read sitemap env ,@args))
            ))
         (else #f)
         )))

    ;; Translate a <call function="..."> element
    (define (match-call-function pipeline args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (cond
         ((and (eq? (sxml:element-name node) 'call)
	       (sxml:attr node 'function))
	  (let ((args (get-attributes node '(function) '() #t
				      args-are-numbers?)))
	    (match-pipeline-end
	     (rest-of-nodes pipeline)
	     `(begin (function-call-v sitemap env ,@args) #t))
	    ))
	 (else #f)
	 )))

    ;; Translate a <call resource="..."> element
    (define (match-call-resource pipeline args-are-numbers?)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (cond
         ((and (eq? (sxml:element-name node) 'call)
	       (sxml:attr node 'resource))
	  (let* ((args (get-attributes node '(resource) '() #t
				       args-are-numbers?))
		 (pipeline
		  ((sxpath '(resources (resource (@ (equal? (name ))))))
		   sitemap)))
	    (match-pipeline-end
	     (rest-of-nodes pipeline)
	     `(begin (display (format "calling ~s~%" ,@args)) (newline) (resource-call-v sitemap env ,@args) #t))
	    ))
	 (else #f)
	 )))

    ;; Make sure nothing follows the pipeline definition
    (define (match-pipeline-end pipeline compfunc)
      (let* ((nodelist ((node-pos 1) pipeline))
             (node (if (null? nodelist) '() (car nodelist))))
        (if (null? node)
            compfunc
            (xml-error node "No element allowed in this context"))))

    ;; The entry point in matching a pipeline. Transforms a pipeline
    ;; definition into a Scheme function whose body executes the
    ;; described pipeline.
    (define (match-match pipeline args-are-numbers?)
      (let ((procname (string->symbol (format "p~a" pcount)))
            (rxname (string->symbol (format "rx~a" pcount))))
        `(define (,procname url sitemap env)
           ((lambda (result)
	      (if result
		  (apply
		   (lambda ,(let loop ((index 1))
			      (if (> index pattern-regexps-no)
				  'rest
				  (cons
				   (string->symbol
				    (format "arg~a" (number->string index)))
				   (loop (+ index 1)))))
		     ,(generate-function-body pipeline args-are-numbers?))
		   (cdr result))
		  #f))
	    (regexp-match ,rxname url)))
	))

    ;; Generates the code for the function body to processes a
    ;; pipeline
    (define (generate-function-body pipeline args-are-numbers?)
      (let ((is-call (or (match-call-function pipeline args-are-numbers?)
			 (match-call-resource pipeline args-are-numbers?))))
	(if is-call
	    is-call
	    `(sitemap:process
	      sitemap env '()
	      ,(or
		(match-generate pipeline args-are-numbers?)
		(match-reader pipeline args-are-numbers?)
		(let* ((nodelist ((node-pos 1) pipeline))
		       (node (if (null? nodelist) '() (car nodelist))))
		  (xml-error node "Invalid pipeline definition")))))))

    ;; This is the main processing function for a 'match' node in the
    ;; SXML representation of the sitemap. This function returns an
    ;; entry like this:
    ;;
    ;;   (regexp . matcher-function-representation)
    ;;
    ;; The `apply-templates' function which invokes `process-match'
    ;; will collect all these pairs and return them in a list.
    (define (process-match node)
      ;; Check for the presence of the 'pattern' attribute and signal
      ;; an error if not present
      (let ((pattern (sxml:attr node 'pattern)))
        (if (not pattern)
            (xml-error
             node "required 'pattern' attribute for <match> is not present"))
        ;; Increment the pipelines count
        (set! pcount (+ pcount 1))
        ;; Translate the pipeline definitions into equivalent Scheme
        ;; functions
        (let ((pipeline (reverse (sxml:child-elements node)))
              (exp-pattern (regexp-split match-pattern-regexp pattern))
              (rxname (string->symbol (format "rx~a" pcount))))
          (set! pattern-regexps-no (/ (- (vector-length exp-pattern) 1) 2))
          (list (cons `(,rxname (regexp ,pattern))
                      (match-match pipeline #t)))
          )))

    ;; This function is called by the `apply-templates' function below
    ;; to process SXML nodes corresponding to <map:resource>.
    ;;
    ;; For each named resource we create a function whose name is
    ;; r_<resource-name>, which contains the definition of the
    ;; resource obtained by calling `match-match'.
    ;;
    ;; This function returns an association whose car is a list, whose
    ;; first element is a symbol for the function name, and the rest
    ;; are symbols for the arguments of the function. The cdr of the
    ;; topmost list is the definition of the function:
    ;;
    ;; ((r_<resource-name> arguments ...) . <function-definition>)
    ;;
    ;; Each resource might make use of named parameters, like below:
    ;;
    ;; <map:resource name="document2html">
    ;;   <map:generate src="{source}" type="file"/>
    ;;   ...
    ;; </map:resource>
    ;;
    ;; This function will identify all the parameters used in the
    ;; definition of the resource, and will make them arguments in the
    ;; function definition. The created function will use the
    ;; `function' macro, instead of the normal Scheme `define'. This
    ;; allows for named parameters to be passed to the function at
    ;; runtime.
    (define (process-resource node)
      (let ((resname (sxml:attr node 'name)))
	(if (not resname)
	    (xml-error node "Resource must be named, use a 'name' attribute")
	    (let* ((funname (name->resource resname))
		   (funsymbol (string->symbol funname))
		   ;; Check to see if a similar resource has already
		   ;; been defined.
		   (_ (if (lookup-resource funsymbol)
			  (xml-error node "A resource named ~s already defined"
				     funsymbol)))
		   (attributes (map (lambda (x) (cadr x))
				    ((sxpath '(// @ (*))) node)))
		   (source-arguments
		    (filter (lambda (x)
			      (if (and (string? x)
				       (regexp-match res-arg-regexp-match x))
				  x
				  #f))
			    attributes))
		   ;; `source-arguments' contains all the attributes
		   ;; that contain named parameters. We need to
		   ;; extract the names from within curly braces
		   (arguments
		    (flatten
		     (map
		      (lambda (x)
			(let* ((v (regexp-split/delimiter arg-regexp-split x))
			       (len (vector-length v)))
			  (let loop ((i 1) (acc '()))
			    (if (>= i len)
				acc
				(loop (+ i 2) (cons (vector-ref v i) acc))))
			  ))
		      source-arguments)))
		   (argument-symbols
		    (map (lambda (x) (string->symbol x)) arguments))
		   (funsig `(,funsymbol ,@argument-symbols))
		   (funbody
		    `(function (,funsymbol sitemap env ,@argument-symbols)
			       ,(generate-function-body
				 (reverse (sxml:child-elements node)) #f))))
	      (add-resource funsig funbody)
	      ))))

    ;; Process the SXML representation of the sitemap. This is done by
    ;; invoking the apply-templates function on the SXML representation
    ;; of the sitemap.
    ;;
    ;; We first setup the exit function, which will be called in case we
    ;; encounter semantic errors.
    (call/cc
     (lambda (k)
       (set! exit k)
       (set! pcount 0)
       ;; Compute the available resources first.
       (apply-templates
	sitemap
	`((resources resource . ,(lambda (node) (process-resource node)))))

       ;; `matchers' will contain a list of (regexp . matcher-function)
       ;; We'll iterate on it to construct the top level function that
       ;; represents the sitemap.
       (let* ((matchers
               (apply-templates
                sitemap
                `((pipelines pipeline match
			     . ,(lambda (node) (process-match node))))))
	      (sitemap-code
	       `((lambda (,@(let loop ((ms matchers))
			      (if (null? ms)
				  '()
				  (cons (caaar ms)
					(loop (cdr ms))))))
		   ;; Generate the code for the resource function
		   ;; definitions
		   ,@(resources-get-function-bodies)

		   ;; Generate the code corresponding to the
		   ;; <map:match> definitions
		   ,@(let loop ((ms matchers))
		       (if (null? ms)
			   '()
			   (cons (cdar ms)
				 (loop (cdr ms)))))
		   (lambda (url sitemap env)
		     (or ,@(let loop ((index 1))
			     (if (> index pcount)
				 '()
				 (cons
				  (list (string->symbol (format "p~a" index))
					'url 'sitemap 'env)
				  (loop (+ index 1))))))))
		 ,@(let loop ((ms matchers))
		     (if (null? ms)
			 '()
			 (cons (cadaar ms)
			       (loop (cdr ms))))))
	       ))
         (newline) (pretty-print sitemap-code) (newline)
         (eval sitemap-code (interaction-environment))
         )))
     ))

;; `the-sitemap' will contain the compiled version of the sitemap.
(define the-sitemap
  (lambda (url sitemap env)
    (display "Sitemap was not compiled because of errors!")
    (newline)))

;; Invoked from the Java side to parse the XML representation of the
;; sitemap and update the `sxml-sitemap' variable. The processing of
;; the XML sitemap representation happens in two steps.
;;
;; In the first step the, using the `sitemap:parse' function, the XML
;; representation is translated into SXML. This translation happens in
;; Java, using the XMLtoSXML ContentHandler, which is invoked through
;; `sitemap:parse', a native function defined in
;; SchemeSitemapFunctions.
;;
;; In the second step, the SXML representation of the sitemap is
;; converted to a Scheme function, using the `process-sitemap'
;; function defined above. The result of this processing, the
;; "executable" sitemap function is set as value for `the-sitemap'
;; variable.
;;
;; When an HTTP request is to be processed by Cocoon, the
;; SchemeSitemap#process method will invoke the main function, usually
;; defined as `main'. This will simply call the function stored in
;; `the-sitemap'.
(define (sitemap-parse! manager source)
  (let* ((sxml (sitemap:parse manager source))
	 (xsxml (process-sitemap sxml)))
    (if (not (eq? xsxml 'error))
	(set! the-sitemap xsxml))))

;; This is the main entry point in the Scheme Cocoon sitemap. This
;; function is invoked from the SchemeSitemap#process method.
(define (main url sitemap environment)
  (the-sitemap url sitemap environment))

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
