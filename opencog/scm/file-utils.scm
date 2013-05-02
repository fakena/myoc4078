;
; file-utils.scm
;
; Assorted file and directory utils.
; Copyright (c) 2008 Linas Vepstas
;

(use-modules (ice-9 rdelim))
(use-modules (ice-9 popen))
(use-modules (ice-9 rw))

; ---------------------------------------------------------------------
; Given a directory, return a list of all of the files in the directory
; Do not return anything that is a subdirectory, pipe, special file etc.
;
(define (list-files dir)

	(define (isfile? file)
		(eq? 'regular (stat:type (stat 
				(string-join (list dir file) "/")))
		)
	)

	; suck all the filenames off a port
	(define (suck-in-filenames port lst)
		(let ((one-file (readdir port)))
			(if (eof-object? one-file)
				lst
				(suck-in-filenames port 
					(if (isfile? one-file)
						(cons one-file lst)
						lst
					)
				)
			)
		)
	)
	(let* ((dirport (opendir dir))
			(filelist (suck-in-filenames dirport '()))
		)
		(closedir dirport)
		filelist
	)
)

; ---------------------------------------------------------------------
; exec-scm-from-port port
;
; Load data read from the indicated port
; The port should contain valid scheme; this routine will read and 
; execute that scheme data.
;
(define (exec-scm-from-port port)

	; Suck in a bunch of ASCII text off of a port, until the port is
	; empty (#eof) and return a string holding the port (file) contents.
	; Use read-string!/partial for speed.
	(define (speedy-suck-in-text port str)
		(let* ((str-buff (make-string 1123123))
		       (line-len (read-string!/partial str-buff port)))
			(if (eq? #f line-len)
				str 
				(speedy-suck-in-text port 
					(string-append str (substring/shared str-buff 0 line-len))
				)
			)
		)
	)

	; Suck in a bunch of ASCII text off of a port, until the port is
	; empty (#eof) and return a string holding the port (file) contents.
	; This is *painfully slowwww* !!!
;	(define (suck-in-text port str)
;		(let ((one-line (read-line port)))
;			(if (eof-object? one-line)
;				str
;				(suck-in-text port 
;					(string-join (list str one-line "\n"))
;				)
;			)
;		)
;	)
;
	(let ((data (speedy-suck-in-text port "")))
		; read data from port
		(eval-string data)
	)
)

; ---------------------------------------------------------------------
; exec-scm-from-cmd cmd-string
;
; Load data generated by the system command cmd-string. The command 
; should generate valid scheme, and send its data to stdout; this
; routine will read and execute that scheme data.
;
; Example:
; (exec-scm-from-cmd "cat /tmp/some-file.scm")
; (exec-scm-from-cmd "cat /tmp/some-file.txt | perl ./bin/some-script.pl")
;
(define (exec-scm-from-cmd cmd-string)

	(let ((port (open-input-pipe cmd-string)))
		(exec-scm-from-port port)
		(close-pipe port)
	)
)

; ---------------------------------------------------------------------
; load-scm-from-file filename
;
; Load scheme data from the indicated file, and execute it.
;
; Example Usage:
; (load-scm-from-file "/tmp/some-file.scm")
;
(define (load-scm-from-file filename)
	(exec-scm-from-cmd (string-join (list "cat \"" filename "\"" ) ""))
)

; ---------------------------------------------------------------------
; export-all-atoms filename
;
; Export the entire contents of the atomspace to the file 'filename'
; If an absolute path is not specified, then the filename will be
; written to the directory in which the opencog server was started.
;
(define (export-all-atoms filename)
	(define (prt-atom-list port lst)
		(if (not (null? lst))
			(let ()
				(display (car lst) port)
				(prt-atom-list port (cdr lst))
			)
		)
	)

	(let ((port (open-file filename "w"))
		)
		(for-each 
			(lambda (x) (prt-atom-list port (cog-get-atoms x)))
			(cog-get-types)
		)

		(close-port port)
	)
)
