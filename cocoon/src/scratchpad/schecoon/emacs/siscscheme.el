;;; Mode to interact with the SISC Scheme command line tool.
;;;
;;; Author: Ovidiu Predescu <ovidiu@cup.hp.com>
;;; Date: December 9, 2001
;;;
;;; Adapted from cmuscheme48.el from the XEmacs distribution, which in
;;; turn was adapted from cmuscheme.el, which in turn was adapted from
;;; other things...
;;;
;;; Add the following in your ~/.emacs (change the location of
;;; sisc-servlet script to suit your needs):
;;;
;;; (require 'cl)
;;; (defvar sisc-location "~/src/schecoon/emacs")
;;; (pushnew sisc-location load-path)
;;; (require 'siscscheme)
;;; (setq sisc-program-name (concat sisc-location "/sisc-servlet"))
;;;
;;; Then visit a Scheme file. Type M-x run-sisc to start the inferior
;;; Scheme. Use the keys defined below to evaluate in the servlet the
;;; definitions you want.
;;;
;;; The first time you evaluate something, you will be asked for a
;;; username and a password. Switch to the *scheme* buffer to enter
;;; them.

(provide 'siscscheme)
(require 'cmuscheme)

;;;###autoload
(defvar sisc-program-name "sisc-servlet"
  "The program name and arguments to be invoked for the `run-sisc'
command.")

;;;###autoload
(defun run-sisc (arg)
  "Run inferiour `sisc'.  See the documentation to `run-scheme' after
`cmuscheme' has been `require'd."
  (interactive (list (if current-prefix-arg
			 (read-string "Run Sisc: " sisc-program-name)
		       sisc-program-name)))
  (run-scheme sisc-program-name))

(define-key scheme-mode-map "\M-\C-x" 'sisc-send-definition);gnu convention
(define-key scheme-mode-map "\C-x\C-e" 'sisc-send-last-sexp);gnu convention
(define-key scheme-mode-map "\C-ce"    'sisc-send-definition)
(define-key scheme-mode-map "\C-c\C-e" 'sisc-send-definition-and-go)
(define-key scheme-mode-map "\C-cr"    'sisc-send-region)
(define-key scheme-mode-map "\C-c\C-r" 'sisc-send-region-and-go)
(define-key scheme-mode-map "\C-cl"    'sisc-load-file)

(defun sisc-remove-newlines (start end)
  "Obtain from the current buffer the string between START and END,
remove all the newlines and return it. Takes care of removing all the
comments before removing the newlines."
  ;; FIXME: Ineficient, processes the string multiple times.
  (concat
   (replace-in-string
    (replace-in-string (buffer-substring start end) ";[^\n]*$" "")
    "\n" " " t)
   "\n"))

(defun sisc-send-buffer-substring (start end)
  "Send to the inferior the buffer substring between START and END."
  (comint-send-string (scheme-proc) (sisc-remove-newlines start end)))

(defun sisc-send-region (start end)
  "Send the current region to the inferior Scheme process."
  (interactive "r")
  (sisc-send-buffer-substring start end))

(defun sisc-send-definition ()
  "Send the current definition to the inferior Sisc process."
  (interactive)
  (save-excursion
   (end-of-defun)
   (let ((end (point)))
     (beginning-of-defun)
     (sisc-send-buffer-substring (point) end))))

(defun sisc-send-last-sexp ()
  "Send the previous sexp to the inferior Scheme process."
  (interactive)
  (sisc-send-buffer-substring (save-excursion (backward-sexp) (point))
			      (point)))

(defun sisc-send-region-and-go (start end)
  "Send the current region to the inferior Sisc process,
and switch to the process buffer."
  (interactive "r")
  (sisc-send-region start end)
  (switch-to-scheme t))

(defun sisc-send-definition-and-go ()
  "Send the current definition to the inferior Sisc,
and switch to the process buffer."
  (interactive)
  (sisc-send-definition)
  (switch-to-scheme t))

(defun sisc-load-file ()
  "Load a Scheme file into the inferior Sisc process."
  (interactive)
  (sisc-send-buffer-substring (save-excursion (beginning-of-buffer) (point))
			      (save-excursion (end-of-buffer) (point))))
